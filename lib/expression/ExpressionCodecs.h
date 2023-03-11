#include <string>
#include "expression_grammar.h"

namespace expression
{
	/// <summary>
	/// Class for static helper methods to parse an expression. 
	/// </summary>
	class ExpressionCodecs
	{
	public:
		ExpressionCodecs() = delete;
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
			return (c >= 'a' && c <= 'z')	// lower case character
				|| (c >= 'A' && c <= 'Z')	// upper case character
				|| c == '_';				// underscore
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
					throw std::runtime_error("Identifier too long.");
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
				throw std::runtime_error("Number too large or number format invalid.");
			}
			// check if after the double value there is a valid character.
			if (IsAlphaNum(*stream))
			{
				throw std::runtime_error("Number format incorrect.");
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
					*tokenID = 0;
					return stream;
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
				*tokenID = TOKEN_MINUS;
				return ++stream;
			case '/':
				*tokenID = TOKEN_DIVIDE;
				return ++stream;
			case '(':
				*tokenID = TOKEN_LEFT_ROUND;
				return ++stream;
			case ')':
				*tokenID = TOKEN_RIGHT_ROUND;
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
}