#pragma once
#include <string>
#include "expression_common.h"
namespace expression
{
	class TerminalSymbol;
	class ExpressionParseTree;
	/// <summary>
	/// A parser which parses mathematical expressions.
	/// </summary>
	class ExpressionParser
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="logFilePath">Full path of a log file which is created while parsing. Set to an empty string to not print logs. Only has an effect if NDEBUG is not set.</param>
		ExpressionParser(std::string logFilePath="");
		~ExpressionParser();
		/// <summary>
		/// Parses the mathematical expression passed to the constructor, and returns a class representation of the expression which can be executed to evaluate the expression.
		/// If the expression can not be parsed, a std::exception is thrown.
		/// </summary>
		/// <param name="expression">Mathematical expression which should be parsed.</param>
		/// <param name="bind">Set to true if all common variables (like "pi", "e", "true", "false", ...) and all common functions (like "max", "min", "exp", "rand", ..) should be
		/// bound to their respective values. This implies that these function and variable names are substituted by their respective conventional definitions when evaluating the expression.
		/// However, before evaluating the expression it is possible to re-bind these function/variable names, for example if the variable "pi" might have a different value than 3.141... in the
		/// context of the specific expression.</param>
		/// <param name="simplify">If true, it is tried to simplify the expression. This involves substituting all common variables and functions which were bound (given that the parameter "bound" is true) by their respective values. 
		/// If this is not wanted, because e.g. "pi" can be the name of a variable in the expression which is not equal to 3.14..., set either this parameter or the parameter "bind" to false. 
		///</param>
		/// <returns>An object representing the parsed expression.</returns>
		std::unique_ptr<IExpression> Parse(std::string expression, bool bind = true, bool simplify=false);

	private:
		std::string logFilePath_;
		void* handle_;
		FILE* logFile_;

		void ParseToken(int tokenID, TerminalSymbol* token, ExpressionParseTree& parseTree);
		void InitializeInternal();
		void UninitializeInternal();
	};
}
