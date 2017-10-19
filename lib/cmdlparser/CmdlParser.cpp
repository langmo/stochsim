#include <fstream>
#include <memory>
#include <set>
#include "cmdl_grammar.h"
#include "CmdlParser.h"
#include "cmdl_symbols.h"
#include "CmdlParseTree.h"
#include "ComposedState.h"
#include "State.h"
#include "Choice.h"
#include "PropensityReaction.h"
#include "DelayReaction.h"
#include "NumberExpression.h"
#include "CmdlCodecs.h"


// Forward declaration parser functions.
// These are defined in cmdl_grammar.c, a file which is automatically generated from cmdl_grammar.y.
void cmdl_internal_Parse(
	void *yyp,                   /* The parser */
	int yymajor,                 /* The major token code number */
	cmdlparser::TerminalSymbol* yyminor,       /* The value for the token */
	cmdlparser::CmdlParseTree* parse_tree               /* Optional %extra_argument parameter */
);

void* cmdl_internal_ParseAlloc(void* (*mallocProc)(size_t));

void cmdl_internal_ParseFree(
	void *p,                    /* The parser to be deleted */
	void(*freeProc)(void*)     /* Function used to reclaim memory */
);
#ifndef NDEBUG
void internal_ParseTrace(FILE *TraceFILE, char *zTracePrompt);
#endif


namespace cmdlparser
{
	void ParseToken(void* handle, int tokenID, TerminalSymbol* token, CmdlParseTree& parseTree)
	{
		try
		{
			cmdl_internal_Parse(handle, tokenID, token, &parseTree);
		}
		catch (const std::exception& ex)
		{
			throw ex;
		}
		catch (...)
		{
			throw std::exception("Unknown error");
		}
	}

	void Interpret(cmdlparser::CmdlParseTree& parseTree, stochsim::Simulation& sim)
	{
		struct state_definition
		{
			enum type
			{
				type_simple,
				type_composed,
				type_choice
			};
			state_definition() noexcept: type_(type_simple)
			{
			}
			state_definition(type type) noexcept : type_(type)
			{
			}
			bool require_type(type type) noexcept
			{
				if (type_ == type_simple)
				{
					type_ = type;
					return true;
				}
				else if (type_ == type_composed)
				{
					if (type == type_composed || type == type_simple)
						return true;
					else
						return false;
				}
				else
				{
					if (type == type_choice || type == type_simple)
						return true;
					else
						return false;
				}
			}
			type type_;
		};
		// get all state names used in reactions.
		std::unordered_map<expression::identifier, state_definition> states;
		for (auto& choice : parseTree.GetChoices())
		{
			states.emplace(choice.first, state_definition::type::type_choice);
		}
		for (auto& choice : parseTree.GetChoices())
		{
			for (auto& elem : *choice.second->GetComponentsIfTrue())
			{
				// define state if yet not existent.
				states[elem.first];
			}
			for (auto& elem : *choice.second->GetComponentsIfFalse())
			{
				// define state if yet not existent.
				states[elem.first];
			}
		}
		for (auto& reaction : parseTree.GetReactions())
		{
			bool isDirectDelay = reaction.second->GetSpecifiers()->HasDelay() && !reaction.second->GetSpecifiers()->HasRate();
			for (auto& elem : *reaction.second->GetReactants())
			{
				// define state if yet not existent, or get it if already existent.
				auto name = elem.first;
				state_definition& state = states[name];
				if (isDirectDelay && !state.require_type(state_definition::type_composed))
				{
					std::stringstream errorMessage;
					errorMessage << "Cannot initialize state '" << name << "': In one reaction it is used as the species determining the delay of a reaction and in another as a choice, which is invalid.";
					throw std::exception(errorMessage.str().c_str());
				}

			}
			for (auto& elem : *reaction.second->GetProducts())
			{
				// define state if yet not existent, or get it if already existent.
				auto name = elem.first;
				state_definition& state = states[name];
				// if a state on the RHS has the modifier flag, this means it is a transformee. Transformees must be composed states. Switch flag to composed, which only fails if state is already defined as a choice.
				if (elem.second->IsModifier() && !state.require_type(state_definition::type_composed))
				{
					std::stringstream errorMessage;
					errorMessage << "Cannot initialize state '" << name << "': In one reaction it is used as a transformee and in another as a choice, which is invalid.";
					throw std::exception(errorMessage.str().c_str());
				}
			}
		}

		// Create states, but not yet choices
		for (auto& state : states)
		{
			if (state.second.type_ == state_definition::type_choice)
			{
				continue;
			}
			auto initialCondition = parseTree.FindVariableValue(state.first);
			if (initialCondition + 0.5 < 0)
			{
				std::stringstream errorMessage;
				errorMessage << "Initial condition for state '" << state.first << "' is negative.";
				throw std::exception(errorMessage.str().c_str());
			}
			if (state.second.type_ == state_definition::type_simple)
				sim.CreateState<stochsim::State>(state.first, static_cast<size_t>(initialCondition + 0.5));
			else if (state.second.type_ == state_definition::type_composed)
				sim.CreateState<stochsim::ComposedState>(state.first, static_cast<size_t>(initialCondition + 0.5));
			else
			{
				std::stringstream errorMessage;
				errorMessage << "State '" << state.first << "' has unknown type.";
				throw std::exception(errorMessage.str().c_str());
			}
		}

		auto variableRegister = [&parseTree, &states](const expression::identifier variableName) -> std::unique_ptr<expression::IExpression>
		{
			// We want to simplify everything away which is not a state name, and not one of the standard variables.
			if (states.find(variableName) == states.end())
				return parseTree.FindVariableExpression(variableName);
			else
				return nullptr;
		};
		auto functionRegister = [&parseTree](const expression::identifier name)->std::unique_ptr<expression::IFunctionHolder>
		{
			if (name[name.size() - 1] == ')' && name[name.size() - 2] == '(')
			{
				return parseTree.FindFunctionHandler(name.substr(0, name.size() - 2));
			}
			return nullptr;
		};

		// create choices in order of definition
		for (auto& choice : parseTree.GetChoices())
		{
			auto condition = choice.second->GetCondition()->Simplify(variableRegister);
			condition->Bind(functionRegister);
			condition = condition->Simplify(variableRegister);

			auto choiceState = sim.CreateState<stochsim::Choice>(choice.first, std::move(condition));
			for (auto& elem : *choice.second->GetComponentsIfTrue())
			{
				choiceState->AddProductIfTrue(sim.GetState(elem.first), elem.second->GetStochiometry());
			}
			for (auto& elem : *choice.second->GetComponentsIfFalse())
			{
				choiceState->AddProductIfFalse(sim.GetState(elem.first), elem.second->GetStochiometry());
			}
		}

		// Create reactions
		for (auto& reactionDefinition : parseTree.GetReactions())
		{
			auto rateDef = reactionDefinition.second->GetSpecifiers()->GetRate();
			auto delayDef = reactionDefinition.second->GetSpecifiers()->GetDelay();
			if (!rateDef && !delayDef)
			{
				std::stringstream errorMessage;
				errorMessage << "Reaction \"" << reactionDefinition.first << "\" has neither a rate nor a delay defined.";
				throw std::exception(errorMessage.str().c_str());
			}
			else if (rateDef && delayDef)
			{
				//TODO: implement
				std::stringstream errorMessage;
				errorMessage << "Yet not implemented.";
				throw std::exception(errorMessage.str().c_str());
			}
			else if (rateDef)
			{
				auto rate = rateDef->Simplify(variableRegister);
				rate->Bind(functionRegister);
				rate = rate->Simplify(variableRegister);
				std::shared_ptr<stochsim::PropensityReaction> reaction;
				if (dynamic_cast<expression::NumberExpression*>(rate.get()))
				{
					auto rateConstant = static_cast<expression::NumberExpression*>(rate.get())->GetValue();
					reaction = sim.CreateReaction<stochsim::PropensityReaction>(reactionDefinition.first, rateConstant);
				}
				else
				{
					reaction = sim.CreateReaction<stochsim::PropensityReaction>(reactionDefinition.first, std::move(rate));
				}
				for (auto& component : *reactionDefinition.second->GetReactants())
				{
					if (component.second->IsModifier())
						reaction->AddModifier(sim.GetState(component.first), component.second->GetStochiometry());
					else
						reaction->AddReactant(sim.GetState(component.first), component.second->GetStochiometry());
				}
				for (auto& component : *reactionDefinition.second->GetProducts())
				{
					if (component.second->IsModifier())
						reaction->AddTransformee(sim.GetState(component.first), component.second->GetStochiometry());
					else
						reaction->AddProduct(sim.GetState(component.first), component.second->GetStochiometry());
				}
			}
			else if (delayDef)
			{
				auto delay = parseTree.GetExpressionValue(delayDef);
				auto& reactants = *reactionDefinition.second->GetReactants();
				if (reactants.GetNumComponents() != 1)
				{
					std::stringstream errorMessage;
					errorMessage << "Reaction " << reactionDefinition.first << " is a delay reaction, which are required to have exactly one reactant.";
					throw std::exception(errorMessage.str().c_str());
				}
				auto& reactant = *reactants.begin()->second;
				if (reactant.IsModifier())
				{
					std::stringstream errorMessage;
					errorMessage << "Reaction " << reactionDefinition.first << " is a delay reaction, which requires that the only reactant " << reactant.GetState() << " is not marked as a modifier.";
					throw std::exception(errorMessage.str().c_str());
				}
				if (reactant.GetStochiometry() != 1)
				{
					std::stringstream errorMessage;
					errorMessage << "Reaction " << reactionDefinition.first << " is a delay reaction, which requires that the only reactant " << reactant.GetState() << " has a stochiometry of one.";
					throw std::exception(errorMessage.str().c_str());
				}
				auto stateBase = sim.GetState(reactant.GetState());
				auto state = std::dynamic_pointer_cast<stochsim::ComposedState>(stateBase);
				if (!state)
				{
					std::stringstream errorMessage;
					errorMessage << "Reaction " << reactionDefinition.first << " is a delay reaction, which requires that the only reactant " << reactant.GetState() << " is a composed state. This should be ensured automatically, but something seems to have gone wrong.";
					throw std::exception(errorMessage.str().c_str());
				}
				auto reaction = sim.CreateReaction<stochsim::DelayReaction>(reactionDefinition.first, state, delay);
				for (auto& component : *reactionDefinition.second->GetProducts())
				{
					if (component.second->IsModifier())
					{
						std::stringstream errorMessage;
						errorMessage << "Reaction " << reactionDefinition.first << " is a delay reaction, which requires that the product " << component.first << " is not marked as a transformee.";
						throw std::exception(errorMessage.str().c_str());
					}
					else
						reaction->AddProduct(sim.GetState(component.first), component.second->GetStochiometry());
				}
			}
		}
	}

	cmdlparser::CmdlParser::CmdlParser() noexcept
	{
	}
	cmdlparser::CmdlParser::~CmdlParser()
	{
	}
	void cmdlparser::CmdlParser::AddVariable(expression::identifier name, expression::number value, bool overwritable) noexcept
	{
		variables_.emplace(std::move(name), Variable({ value, overwritable }));
	}
	
	void ParseFileInternal(std::string cmdlFilePath, stochsim::Simulation& sim, cmdlparser::CmdlParseTree& parseTree, void* handle)
	{
		// Open file
		std::ifstream infile(cmdlFilePath);
		if (infile.fail())
		{
			std::stringstream errorMessage;
			errorMessage << "File \"" << cmdlFilePath << "\" does not exist or could not be opened.";
			throw std::exception(errorMessage.str().c_str());
		}

		// Variables to store values and types of tokens
		int tokenID;
		double doubleValue;
		constexpr int maxStringValueLength = 200;
		std::string::value_type stringValue[maxStringValueLength]; // buffer for token values. must be zero terminated.

		// Parse lines
		std::string line;
		unsigned int currentLine = 0;
		bool inBlockComment = false;
		while (std::getline(infile, line))
		{
			currentLine++;
			auto currentCharPtr = line.c_str();
			auto startCharPtr = currentCharPtr;
			auto lastCharPtr = currentCharPtr;
			try
			{
				while (*currentCharPtr)
				{
					lastCharPtr = currentCharPtr;
					// Discard spaces/tabs/...
					if (CMDLCodecs::IsSpace(currentCharPtr[0]))
					{
						currentCharPtr++;
						continue;
					}
					else if (inBlockComment)
					{
						if (CMDLCodecs::isBlockCommentEnd(currentCharPtr[0], currentCharPtr[1]))
						{
							currentCharPtr+=2;
							inBlockComment = false;
							continue;
						}
						else
						{
							currentCharPtr++;
							continue;
						}
					}
					else if (CMDLCodecs::isBlockCommentStart(currentCharPtr[0], currentCharPtr[1]))
					{
						currentCharPtr += 2;
						inBlockComment = true;
						continue;
					}
					// Discard rest of the line if line comment
					else if (CMDLCodecs::isLineComment(currentCharPtr[0], currentCharPtr[1]))
						break;

					// Initialize all token values and identifiers.
					tokenID = 0;

					// check if simple token
					currentCharPtr = CMDLCodecs::GetSimpleToken(currentCharPtr, &tokenID);
					if (tokenID != 0)
					{
						ParseToken(handle, tokenID, new cmdlparser::TerminalSymbol(), parseTree);
						continue;
					}

					// check if identifier
					stringValue[0] = '\0';
					currentCharPtr = CMDLCodecs::GetIdentifier(currentCharPtr, &tokenID, stringValue, maxStringValueLength);
					if (tokenID != 0)
					{
						ParseToken(handle, tokenID, new cmdlparser::TerminalSymbol(stringValue), parseTree);
						continue;
					}

					// check if preprocessor directive (everything starting with an '#').
					stringValue[0] = '\0';
					currentCharPtr = CMDLCodecs::GetPreprocessor(currentCharPtr, &tokenID, stringValue, maxStringValueLength);
					if (tokenID != 0)
					{
						ParseToken(handle, tokenID, new cmdlparser::TerminalSymbol(), parseTree);
						continue;
					}

					// check if double value
					doubleValue = 0;
					currentCharPtr = CMDLCodecs::GetDouble(currentCharPtr, &tokenID, &doubleValue);
					if (tokenID != 0)
					{
						ParseToken(handle, tokenID, new cmdlparser::TerminalSymbol(doubleValue), parseTree);
						continue;
					}

					// if we are here, we got an unexpected character...
					std::stringstream errorMessage;
					errorMessage << "Character '" << *currentCharPtr << "' invalid.";
					throw std::exception(errorMessage.str().c_str());
				}
			}
			catch (const std::exception& ex)
			{
				std::stringstream errorMessage;
				errorMessage << "Parse error in file " << cmdlFilePath << ", line " << currentLine << "-" << (lastCharPtr - startCharPtr + 1) << ": " << ex.what();
				errorMessage << '\n' << line << '\n';
				for (int i = 0; i < lastCharPtr - startCharPtr; i++)
					errorMessage << ' ';
				errorMessage << "|___ close to here.";

				throw std::exception(errorMessage.str().c_str());
			}
			catch (...)
			{
				std::stringstream errorMessage;
				errorMessage << "Parse error in file " << cmdlFilePath << ", line " << currentLine << "-" << (lastCharPtr - startCharPtr + 1) << ": Unknown error.";
				errorMessage << '\n' << line << '\n';
				for (int i = 0; i < lastCharPtr - startCharPtr; i++)
					errorMessage << ' ';
				errorMessage << "|___ close to here.";

				throw std::exception(errorMessage.str().c_str());
			}
		}

		if (inBlockComment)
		{
			std::stringstream errorMessage;
			errorMessage << "Reached end of file " << cmdlFilePath << " while block comment was still active. Did you forget to write \"*/\" somewhere?";
			throw std::exception(errorMessage.str().c_str());
		}

		// finish parsing
		try
		{
			ParseToken(handle, 0, nullptr, parseTree);
		}
		catch (const std::exception& ex)
		{
			std::stringstream errorMessage;
			errorMessage << "Parse error in file " << cmdlFilePath << " while finishing parsing: " << ex.what();
			throw std::exception(errorMessage.str().c_str());
		}
		catch (...)
		{
			std::stringstream errorMessage;
			errorMessage << "Parse error in file " << cmdlFilePath << " while finishing parsing: Unknown error.";
			throw std::exception(errorMessage.str().c_str());
		}

		
	}
	void ParseFile(const std::string& cmdlFilePath, stochsim::Simulation& sim, cmdlparser::CmdlParseTree& parseTree)
	{
		// Initialize the lemon parser
		auto handle = cmdl_internal_ParseAlloc(malloc);
		if(!handle)
			throw std::exception("Could not initialize cmdl parser.");

		// do the actual parsing of the file.
		// We only catch errors to quickly close the lemon parser (which requires C logic), and then rethrow them.
		// Indeed, this wrapper around ParseFileInternal only exists for exactly this reason...
		bool isError = false;
		std::exception exception;
		try
		{
			ParseFileInternal(cmdlFilePath, sim, parseTree, handle);
		}
		catch (const std::exception& ex)
		{
			isError = true;
			exception = ex;
		}
		catch (...)
		{
			isError = true;
			exception = std::exception("Unknown error");
		}
		cmdl_internal_ParseFree(handle, free);

		if (isError)
			throw exception;

	}
	void cmdlparser::CmdlParser::Parse(std::string cmdlFilePath, stochsim::Simulation& sim, std::string logFilePath)
	{
		// Initialize parse tree
		cmdlparser::CmdlParseTree parseTree;
		for (auto& variable : variables_)
		{
			if (variable.second.overwritable_)
			{
				parseTree.CreateVariable(variable.first, variable.second.value_);
			}
			else
			{
				parseTree.CreateFinalVariable(variable.first, variable.second.value_);
			}
		}
		parseTree.SetIncludeFileCallback([&parseTree, &sim](expression::identifier file)
		{
			ParseFile(file, sim, parseTree);
		});

		// Setup log file if in debug mode.
		// Note that if not in debug mode, this functionality is deactivated per #ifndef in the cmdl_grammar.template.
		// Since we try not to change this template, we thus cannot call it.
		FILE* logFile = nullptr;
#ifndef NDEBUG
		if (!logFilePath.empty())
		{
			fopen_s(&logFile, logFilePath.c_str(), "w");
			if (logFile_)
				cmdl_internal_ParseTrace(logFile, "cmdl_");
			else
				cmdl_internal_ParseTrace(0, "cmdl_");
		}
#endif

		// Do the actual parsing.
		// We only catch errors to quickly close the log file (which requires C logic), and then rethrow them.
		bool isError = false;
		std::exception exception;
		try
		{
			ParseFile(cmdlFilePath, sim, parseTree);
		}
		catch (const std::exception& ex)
		{
			isError = true;
			exception = ex;
		}
		catch (...)
		{
			isError = true;
			exception = std::exception("Unknown error");
		}
#ifndef NDEBUG
		cmdl_internal_ParseTrace(0, "cmdl_");
		if (logFile)
			fclose(logFile);
#endif
		if (isError)
			throw exception;

		// Interpret parse tree
		Interpret(std::move(parseTree), sim);
	}
}