#pragma once
#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include "expression_common.h"

namespace expression
{
	/// <summary>
	/// Terminal symbol. Either a string, a double, or a terminal symbol without value (e.g. the terminal symbols '+', '-', '+', '/', ',', '->', ...).
	/// </summary>
	class TerminalSymbol
	{
	public:
		enum terminal_type
		{
			/// <summary>
			/// Type for terminal symbols being identifiers, i.e. strings.
			/// </summary>
			type_identifier,
			/// <summary>
			/// Type for numbers, i.e. doubles.
			/// </summary>
			type_number,
			/// <summary>
			/// Type for terminal symbols, like '+', '-', '+', '/', ',', '->', etc., holding no specific value.
			/// </summary>
			type_empty
		};

	public:
		operator expression::number() const
		{
			if (type_ != type_number)
				throw std::runtime_error("Terminal symbol is not a number.");
			return numberValue_;
		}

		operator const expression::identifier() const
		{
			if (type_ != type_identifier)
				throw std::runtime_error("Terminal symbol is not an identifier.");
			return identifierValue_;
		}

		TerminalSymbol(expression::number numberValue) : numberValue_(std::move(numberValue)), type_(type_number)
		{
		}
		TerminalSymbol(expression::identifier identifierValue) : identifierValue_(std::move(identifierValue)), type_(type_identifier)
		{
		}
		TerminalSymbol() : type_(type_empty)
		{
		}
		terminal_type GetType()
		{
			return type_;
		}
	private:
		expression::number numberValue_;
		expression::identifier identifierValue_;
		terminal_type type_;
	};

	typedef std::vector<std::unique_ptr<expression::IExpression>> FunctionArguments;	
}
