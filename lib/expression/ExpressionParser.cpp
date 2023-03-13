#include <fstream>
#include <memory>
#include <set>
#include "ExpressionParser.h"
#include "ExpressionCodecs.h"
#include "expression_symbols.h"
#include "ExpressionParseTree.h"

// Forward declaration parser functions.
// These are defined in expression_grammar.c, a file which is automatically generated from expression_grammar.y.
void expression_Parse(
	void *yyp,                   /* The parser */
	int yymajor,                 /* The major token code number */
	expression::TerminalSymbol* yyminor,       /* The value for the token */
	expression::ExpressionParseTree* parse_tree               /* Optional %extra_argument parameter */
);

void *expression_ParseAlloc(void* (*mallocProc)(size_t));

void expression_ParseFree(
	void *p,                    /* The parser to be deleted */
	void(*freeProc)(void*)     /* Function used to reclaim memory */
);
#ifndef NDEBUG
void expression_ParseTrace(FILE *TraceFILE, char *zTracePrompt);
#endif

namespace expression
{
	void ParseToken(void* handle, int tokenID, TerminalSymbol* token, ExpressionParseTree& parseTree)
	{
		try
		{
			expression_Parse(handle, tokenID, token, &parseTree);
		}
		catch (const std::runtime_error& ex)
		{
			throw ex;
		}
		catch (...)
		{
			throw std::runtime_error("Unknown error");
		}
	}
	ExpressionParser::ExpressionParser() noexcept
	{
	}
	expression::ExpressionParser::~ExpressionParser()
	{
	}
	std::unique_ptr<IExpression> ParseInternal(std::string expressionStr, bool bind, bool simplify, expression::ExpressionParseTree& parseTree, void* handle)
	{
		// Variables to store values and types of tokens
		int tokenID;
		double doubleValue;
		constexpr int maxStringValueLength = 200;
		std::string::value_type stringValue[maxStringValueLength]; // buffer for token values. must be zero terminated.

		auto currentCharPtr = expressionStr.c_str();
		auto startCharPtr = currentCharPtr;
		auto lastCharPtr = currentCharPtr;
		try
		{
			while (*currentCharPtr)
			{
				lastCharPtr = currentCharPtr;
				// Discard spaces/tabs/...
				if (ExpressionCodecs::IsSpace(currentCharPtr[0]))
				{
					currentCharPtr++;
					continue;
				}
				// Discard rest of the line if comment
				else if (ExpressionCodecs::isLineComment(currentCharPtr[0], currentCharPtr[1]))
					break;

				// Initialize all token values and identifiers.
				tokenID = 0;

				// check if simple token
				currentCharPtr = ExpressionCodecs::GetSimpleToken(currentCharPtr, &tokenID);
				if (tokenID != 0)
				{
					ParseToken(handle, tokenID, new TerminalSymbol(), parseTree);
					continue;
				}

				// check if identifier
				stringValue[0] = '\0';
				currentCharPtr = ExpressionCodecs::GetIdentifier(currentCharPtr, &tokenID, stringValue, maxStringValueLength);
				if (tokenID != 0)
				{
					ParseToken(handle, tokenID, new expression::TerminalSymbol(stringValue), parseTree);
					continue;
				}

				// check if double value
				doubleValue = 0;
				currentCharPtr = ExpressionCodecs::GetDouble(currentCharPtr, &tokenID, &doubleValue);
				if (tokenID != 0)
				{
					ParseToken(handle, tokenID, new expression::TerminalSymbol(doubleValue), parseTree);
					continue;
				}

				// if we are here, we got an unexpected character...
				std::stringstream errorMessage;
				errorMessage << "Character '" << *currentCharPtr << "' invalid.";
				throw std::runtime_error(errorMessage.str().c_str());
			}
		}
		catch (const std::runtime_error& ex)
		{
			std::stringstream errorMessage;
			errorMessage << "Parse error in expression close to position " << (lastCharPtr - startCharPtr + 1) << ": " << ex.what();
			errorMessage << '\n' << expressionStr << '\n';
			for (int i = 0; i < lastCharPtr - startCharPtr; i++)
				errorMessage << ' ';
			errorMessage << "|___ close to here.";

			throw std::runtime_error(errorMessage.str().c_str());
		}
		catch (...)
		{
			std::stringstream errorMessage;
			errorMessage << "Parse error in expression close to position " << (lastCharPtr - startCharPtr + 1) << ": Unknown error.";
			errorMessage << '\n' << expressionStr << '\n';
			for (int i = 0; i < lastCharPtr - startCharPtr; i++)
				errorMessage << ' ';
			errorMessage << "|___ close to here.";

			throw std::runtime_error(errorMessage.str().c_str());
		}
		

		// finish parsing
		try
		{
			ParseToken(handle, 0, nullptr, parseTree);
		}
		catch (const std::runtime_error& ex)
		{
			std::stringstream errorMessage;
			errorMessage << "Parse error while finishing parsing: " << ex.what();
			throw std::runtime_error(errorMessage.str().c_str());
		}
		catch (...)
		{
			std::stringstream errorMessage;
			errorMessage << "Parse error while finishing parsing: Unknown error.";
			throw std::runtime_error(errorMessage.str().c_str());
		}
		
		// Prepare result
		auto orgResult = parseTree.GetResult();
		if (!orgResult)
			throw std::runtime_error("Parse error while finishing parsing: No expression obtained.");
		auto result = orgResult->Clone();
		if (bind)
		{
			result->Bind(parseTree.GetBindingRegister());
		}
		if (simplify)
		{
			result = result->Simplify();
		}
		return std::move(result); 
	}

	std::unique_ptr<IExpression> expression::ExpressionParser::Parse(std::string expressionStr, bool bind, bool simplify, std::string logFilePath)
	{
		// Initialize parse tree
		expression::ExpressionParseTree parseTree;

		// Initialize the lemon parser
		auto handle = expression_ParseAlloc(malloc);
		if (!handle)
			throw std::runtime_error("Could not initialize expression parser.");

		// Setup log file if in debug mode.
		// Note that if not in debug mode, this functionality is deactivated per #ifndef in the expression_grammar.template.
		// Since we try not to change this template, we thus cannot call it.
		FILE* logFile = nullptr;
#ifndef NDEBUG
		if (!logFilePath.empty())
		{
			logFile = fopen(logFilePath.c_str(), "w");
			char prompt[] = "expression_";
			if (logFile)
				expression_ParseTrace(logFile, prompt);
			else
				expression_ParseTrace(0, prompt);
		}
#endif

		// Do the actual parsing.
		// We only catch errors to quickly close the log file (which requires C logic), and then rethrow them.
		bool isError = false;
		std::runtime_error exception{"Unknown error"};
		std::unique_ptr<IExpression> result;
		try
		{
			result = ParseInternal(expressionStr, bind, simplify, parseTree, handle);
		}
		catch (const std::runtime_error& ex)
		{
			isError = true;
			exception = ex;
		}
		catch (...)
		{
			isError = true;
			exception = std::runtime_error("Unknown error");
		}
		expression_ParseFree(handle, free);
#ifndef NDEBUG
		char prompt[] = "expression_";
		expression_ParseTrace(0, prompt);
		if (logFile)
			fclose(logFile);
#endif
		if (isError)
			throw exception;

		return std::move(result);
	}
}