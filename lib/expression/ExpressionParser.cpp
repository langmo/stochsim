#include <fstream>
#include <memory>
#include <set>
#include "ExpressionParser.h"
#include "ExpressionCodecs.h"
#include "expression_symbols.h"
#include "ExpressionParseTree.h"
namespace expression
{

	ExpressionParser::ExpressionParser(std::string logFilePath) : logFilePath_(std::move(logFilePath)), handle_(nullptr), logFile_(nullptr)
	{
	}
	expression::ExpressionParser::~ExpressionParser()
	{
		UninitializeInternal();
	}
	std::unique_ptr<IExpression> expression::ExpressionParser::Parse(std::string expressionStr, bool bind, bool simplify)
	{
		expression::ExpressionParseTree parseTree;

		// Create effective parser
		InitializeInternal();

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
					ParseToken(tokenID, new TerminalSymbol(), parseTree);
					continue;
				}

				// check if identifier
				stringValue[0] = '\0';
				currentCharPtr = ExpressionCodecs::GetIdentifier(currentCharPtr, &tokenID, stringValue, maxStringValueLength);
				if (tokenID != 0)
				{
					ParseToken(tokenID, new expression::TerminalSymbol(stringValue), parseTree);
					continue;
				}

				// check if double value
				doubleValue = 0;
				currentCharPtr = ExpressionCodecs::GetDouble(currentCharPtr, &tokenID, &doubleValue);
				if (tokenID != 0)
				{
					ParseToken(tokenID, new expression::TerminalSymbol(doubleValue), parseTree);
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
			UninitializeInternal();

			std::stringstream errorMessage;
			errorMessage << "Parse error in expression close to position " << (lastCharPtr - startCharPtr + 1) << ": " << ex.what();
			errorMessage << '\n' << expressionStr << '\n';
			for (int i = 0; i < lastCharPtr - startCharPtr; i++)
				errorMessage << ' ';
			errorMessage << "|___ close to here.";

			throw std::exception(errorMessage.str().c_str());
		}
		catch (...)
		{
			UninitializeInternal();

			std::stringstream errorMessage;
			errorMessage << "Parse error in expression close to position " << (lastCharPtr - startCharPtr + 1) << ": Unknown error.";
			errorMessage << '\n' << expressionStr << '\n';
			for (int i = 0; i < lastCharPtr - startCharPtr; i++)
				errorMessage << ' ';
			errorMessage << "|___ close to here.";

			throw std::exception(errorMessage.str().c_str());
		}
		

		// finish parsing
		try
		{
			ParseToken(0, nullptr, parseTree);
		}
		catch (const std::exception& ex)
		{
			UninitializeInternal();

			std::stringstream errorMessage;
			errorMessage << "Parse error while finishing parsing: " << ex.what();
			throw std::exception(errorMessage.str().c_str());
		}
		catch (...)
		{
			UninitializeInternal();

			std::stringstream errorMessage;
			errorMessage << "Parse error while finishing parsing: Unknown error.";
			throw std::exception(errorMessage.str().c_str());
		}

		UninitializeInternal();
		
		
		// Prepare result
		auto orgResult = parseTree.GetResult();
		if (!orgResult)
			throw std::exception("Parse error while finishing parsing: No expression obtained.");
		auto result = orgResult->Clone();
		if (bind)
		{
			result->Bind(parseTree.GetBindingLookup());
		}
		if (simplify)
		{
			result = result->Simplify();
		}
		return std::move(result); 
	}
}