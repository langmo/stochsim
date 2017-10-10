#include <fstream>
#include <memory>
#include <set>
#include "parser.h"
#include "symbols.h"
#include "parse_tree.h"
#include "../ComposedState.h"
#include "../State.h"
#include "../Choice.h"
#include "../PropensityReaction.h"
#include "../DelayReaction.h"
#include "../expression/conditional_expression.h"

/// <summary>
/// Class for static helper methods to parse a cmdl file.
/// </summary>
class codecs
{
public:
	codecs() = delete;
	/// <summary>
	/// Returns c in [0-9]. 
	/// </summary>
	/// <param name="c">Character to test.</param>
	/// <returns>c in [0-9]</returns>
	inline static bool IsDigit(std::string::value_type c) noexcept
	{
		return c >= '0' && c <= '9';
	}
	/// <summary>
	/// Returns c in [a-zA-Z_].
	/// </summary>
	/// <param name="c">Character to test.</param>
	/// <returns>c in [a-zA-Z_]</returns>
	inline static bool IsAlpha(std::string::value_type c) noexcept
	{
		return (c>='a' && c<= 'z')		// lower case character
			|| (c >= 'A' && c <= 'Z')	// upper case character
			|| c=='_';					// underscore
	}
	/// <summary>
	/// Returns c in [0-9a-zA-Z_].
	/// </summary>
	/// <param name="c">Character to test.</param>
	/// <returns>c in [0-9a-zA-Z_]</returns>
	inline static bool IsAlphaNum(std::string::value_type c) noexcept
	{
		return IsAlpha(c) || IsDigit(c);
	}
	inline static bool IsSpace(std::string::value_type c) noexcept
	{
		return c == ' '		// space
			|| c == '\f'	// form feed
			|| c == '\n'	// line feed
			|| c == '\r'	// carriage return
			|| c == '\t'	// horizontal tab
			|| c == '\v';	// vertical tab
	}
	inline static bool isLineComment(std::string::value_type first, std::string::value_type next)  noexcept
	{
		return first == '/' && next == '/'; // C++ style line comments ("//").
	}


	inline static const std::string::value_type* GetIdentifier(const std::string::value_type* stream, int* tokenID, std::string::value_type* buffer, unsigned int bufferLength)
	{
		// Test if valid start of identifier. Otherwise, return empty buffer and current position in character stream.
		if (!IsAlpha(*stream))
		{
			buffer[0] = '\0';
			*tokenID = 0;
			return stream;
		}
		// copy characters
		unsigned int i = 0;
		while (IsAlphaNum(*stream))
		{
			buffer[i] = *stream;
			i++;
			stream++;
			if (i >= bufferLength)
				throw std::exception("Identifier too long.");
		}
		buffer[i] = '\0';
		*tokenID = TOKEN_IDENTIFIER;
		return stream;
	}
	inline static const std::string::value_type* GetDouble(const std::string::value_type* stream, int* tokenID, double* value)
	{
		// Test if valid start of double value. Otherwise, return 0 and current position in character stream.
		if (!IsDigit(*stream) && *stream != '.')
		{
			*value = 0;
			*tokenID = 0;
			return stream;
		}
		errno = 0; // strtod sets errno to ERANGE if number too large.
		char* pEnd;
		*value = strtod(stream, &pEnd);
		stream = pEnd;
		if (errno != 0)
		{
			errno = 0;
			throw std::exception("Number too large.");
		}
		// check if after the double value there is a valid character.
		if (IsAlphaNum(*stream))
		{
			throw std::exception("Number format incorrect.");
		}

		*tokenID = TOKEN_VALUE;
		return stream;
	}
		
	inline static const std::string::value_type* GetSimpleToken(const std::string::value_type* stream, int* tokenID)
	{
		switch (stream[0])
		{
		case ',':
			*tokenID = TOKEN_COMMA;
			return ++stream;
		case ';':
			*tokenID = TOKEN_SEMICOLON;
			return ++stream;
		case '+':
			*tokenID = TOKEN_PLUS;
			return ++stream;
		case '^':
			*tokenID = TOKEN_EXP;
			return ++stream;
		case '*':
			*tokenID = TOKEN_MULTIPLY;
			return ++stream;
		case ':':
			*tokenID = TOKEN_COLON;
			return ++stream;
		case '?':
			*tokenID = TOKEN_QUESTIONMARK;
			return ++stream;
		case '=':
			if (stream[1] == '=')
			{
				*tokenID = TOKEN_EQUAL;
				stream += 2;
				return stream;
			}
			else
			{
				*tokenID = TOKEN_ASSIGN;
				return ++stream;
			}
		case '&':
			if (stream[1] == '&')
			{
				*tokenID = TOKEN_AND;
				stream += 2;
				return stream;
			}
			else
			{
				*tokenID = 0;
				return stream;
			}
		case '|':
			if (stream[1] == '|')
			{
				*tokenID = TOKEN_OR;
				stream += 2;
				return stream;
			}
			else
			{
				*tokenID = 0;
				return stream;
			}
		case '-':
			if (stream[1] == '>')
			{
				*tokenID = TOKEN_ARROW;
				stream += 2;
				return stream;
			}
			else
			{
				*tokenID = TOKEN_MINUS;
				return ++stream;
			}
		case '/':
			*tokenID = TOKEN_DIVIDE;
			return ++stream;
		case '(':
			*tokenID = TOKEN_LEFT_ROUND;
			return ++stream;
		case ')':
			*tokenID = TOKEN_RIGHT_ROUND;
			return ++stream;
		case '[':
			*tokenID = TOKEN_LEFT_SQUARE;
			return ++stream;
		case ']':
			*tokenID = TOKEN_RIGHT_SQUARE;
			return ++stream;
		case '!':
			if (stream[1] == '=')
			{
				*tokenID = TOKEN_NOT_EQUAL;
				stream += 2;
				return stream;
			}
			else
			{
				*tokenID = TOKEN_NOT;
				return ++stream;
			}
		case '>':
			if (stream[1] == '=')
			{
				*tokenID = TOKEN_GREATER_EQUAL;
				stream += 2;
				return stream;
			}
			else
			{
				*tokenID = TOKEN_GREATER;
				return ++stream;
			}
		case '<':
			if (stream[1] == '=')
			{
				*tokenID = TOKEN_LESS_EQUAL;
				stream += 2;
				return stream;
			}
			else
			{
				*tokenID = TOKEN_LESS;
				return ++stream;
			}
		default:
			*tokenID = 0;
			return stream;
		}
	}
};
void Interpret(cmdl::parse_tree& parseTree, stochsim::Simulation& sim)
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
			else if (type_ == type_choice)
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
	for (auto& choice : parseTree.get_choices())
	{
		states.emplace(choice.first, state_definition::type::type_choice);
	}
	for (auto& choice : parseTree.get_choices())
	{
		for (auto& elem : *choice.second->get_components_if_true())
		{
			// define state if yet not existent.
			states[elem.first];
		}
		for (auto& elem : *choice.second->get_components_if_false())
		{
			// define state if yet not existent.
			states[elem.first];
		}
	}
	for (auto& reaction : parseTree.get_reactions())
	{
		for (auto& elem : *reaction.second->get_reactants())
		{
			// construct state if yet not existent.
			states[elem.first];
			
		}
		for (auto& elem : *reaction.second->get_products())
		{
			// define state if yet not existent, or get it if already existent.
			auto name = elem.first;
			state_definition& state = states[name];
			// if a state on the RHS has the modifier flag, this means it is a transformee. Transformees must be composed states. Switch flag to composed, which only fails if state is already defined as a choice.
			if (elem.second->is_modifier() && !state.require_type(state_definition::type_composed))
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
		auto expression = parseTree.get_variable_expression(state.first);		
		auto initialCondition = parseTree.get_expression_value(expression.get());
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

	// create choices in order of definition
	for (auto& choice : parseTree.get_choices())
	{
		auto choiceState = sim.CreateState<stochsim::Choice>(choice.first, choice.second->get_condition()->clone());
		for (auto& elem : *choice.second->get_components_if_true())
		{
			choiceState->AddProductIfTrue(sim.GetState(elem.first), elem.second->get_stochiometry());
		}
		for (auto& elem : *choice.second->get_components_if_false())
		{
			choiceState->AddProductIfFalse(sim.GetState(elem.first), elem.second->get_stochiometry());
		}
	}

	// Create reactions
	auto variableLookup = [&parseTree, &states](const expression::identifier variableName) -> std::unique_ptr<expression::expression_base>
	{
		// We want to simplify everything away which is not a state name, and not one of the standard variables.
		if (states.find(variableName) == states.end())
			return parseTree.get_variable_expression(variableName);
		std::stringstream errorMessage;
		errorMessage << "Variable with name \"" << variableName << "\" not defined";
		throw std::exception(errorMessage.str().c_str());
	};
	auto functionLookup = [&parseTree](const expression::identifier name)->std::unique_ptr<expression::function_holder_base>
	{
		if (name[name.size() - 1] == ')' && name[name.size() - 2] == '(')
		{
			return parseTree.get_function_handler(name.substr(0, name.size() - 2));
		}
		throw std::exception("Only binding functions, not variables (we are substituting them instead).");
	};
	for (auto& reactionDefinition : parseTree.get_reactions())
	{
		auto rate = reactionDefinition.second->get_rate()->simplify(variableLookup);
		rate->bind(functionLookup);
		rate = rate->simplify(variableLookup);
		std::shared_ptr<stochsim::PropensityReaction> reaction;
		if (dynamic_cast<expression::number_expression*>(rate.get()))
		{
			auto rateConstant = static_cast<expression::number_expression*>(rate.get())->get_number();
			reaction = sim.CreateReaction<stochsim::PropensityReaction>(reactionDefinition.first, rateConstant);
		}
		else
		{
			reaction = sim.CreateReaction<stochsim::PropensityReaction>(reactionDefinition.first, std::move(rate));
		}
		for (auto& component : *reactionDefinition.second->get_reactants())
		{
			if (component.second->is_modifier())
				reaction->AddModifier(sim.GetState(component.first), component.second->get_stochiometry());
			else
				reaction->AddReactant(sim.GetState(component.first), component.second->get_stochiometry());
		}
		for (auto& component : *reactionDefinition.second->get_products())
		{
			if (component.second->is_modifier())
				reaction->AddTransformee(sim.GetState(component.first), component.second->get_stochiometry());
			else
				reaction->AddProduct(sim.GetState(component.first), component.second->get_stochiometry());
		}
			
	}
}

cmdl::parser::parser(std::string cmdlFilePath, std::string logFilePath) : cmdlFilePath_(std::move(cmdlFilePath)), logFilePath_(std::move(logFilePath)), handle_(nullptr), logFile_(nullptr)
{
}
cmdl::parser::~parser()
{
	uninitialize_internal();
}
void cmdl::parser::parse(stochsim::Simulation& sim)
{
	cmdl::parse_tree parseTree;

	// Open file
	std::ifstream infile(cmdlFilePath_); 
	if (infile.fail())
	{
		std::stringstream errorMessage;
		errorMessage << "File \"" << cmdlFilePath_ << "\" does not exist or could not be opened.";
		throw std::exception(errorMessage.str().c_str());
	}

	// Create effective parser
	initialize_internal();
	
	// Variables to store values and types of tokens
	int tokenID;
	double doubleValue;
	constexpr int maxStringValueLength = 200;
	std::string::value_type stringValue[maxStringValueLength]; // buffer for token values. must be zero terminated.
		
	// Parse lines
	std::string line;
	unsigned int currentLine = 0;
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
				if (codecs::IsSpace(currentCharPtr[0]))
				{
					currentCharPtr++;
					continue;
				}
				// Discard rest of the line if comment
				else if (codecs::isLineComment(currentCharPtr[0], currentCharPtr[1]))
					break;

				// Initialize all token values and identifiers.
				tokenID = 0;					

				// check if simple token
				currentCharPtr = codecs::GetSimpleToken(currentCharPtr, &tokenID);
				if (tokenID != 0)
				{
					parse_token(tokenID, new cmdl::terminal_symbol(), parseTree);
					continue;
				}

				// check if identifier
				stringValue[0] = '\0';
				currentCharPtr = codecs::GetIdentifier(currentCharPtr, &tokenID, stringValue, maxStringValueLength);
				if (tokenID != 0)
				{
					parse_token(tokenID, new cmdl::terminal_symbol(stringValue), parseTree);
					continue;
				}

				// check if double value
				doubleValue = 0;
				currentCharPtr = codecs::GetDouble(currentCharPtr, &tokenID, &doubleValue);
				if (tokenID != 0)
				{
					parse_token(tokenID, new cmdl::terminal_symbol(doubleValue), parseTree);
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
			uninitialize_internal();

			std::stringstream errorMessage;
			errorMessage << "Parse error in file " << cmdlFilePath_ << ", line " << currentLine << "-" << (lastCharPtr - startCharPtr+1) << ": " << ex.what();
			errorMessage << '\n' << line << '\n';
			for (int i = 0; i < lastCharPtr - startCharPtr; i++)
				errorMessage << ' ';
			errorMessage << "|___ close to here.";

			throw std::exception(errorMessage.str().c_str());
		}
		catch (...)
		{
			uninitialize_internal();

			std::stringstream errorMessage;
			errorMessage << "Parse error in file " << cmdlFilePath_ << ", line " << currentLine << "-"<<(lastCharPtr-startCharPtr+1)<<": Unknown error.";
			errorMessage << '\n' << line << '\n';
			for (int i = 0; i < lastCharPtr - startCharPtr; i++)
				errorMessage << ' ';
			errorMessage << "|___ close to here.";

			throw std::exception(errorMessage.str().c_str());
		}
	}

	// finish parsing
	try
	{
		parse_token(0, nullptr, parseTree);
	}
	catch (const std::exception& ex)
	{
		uninitialize_internal();

		std::stringstream errorMessage;
		errorMessage << "Parse error in file " << cmdlFilePath_ << " while finishing parsing: " << ex.what();
		throw std::exception(errorMessage.str().c_str());
	}
	catch (...)
	{
		uninitialize_internal();

		std::stringstream errorMessage;
		errorMessage << "Parse error in file " << cmdlFilePath_ << " while finishing parsing: Unknown error.";
		throw std::exception(errorMessage.str().c_str());
	}

	uninitialize_internal();

	// Interpret parse tree
	Interpret(std::move(parseTree), sim);
}
