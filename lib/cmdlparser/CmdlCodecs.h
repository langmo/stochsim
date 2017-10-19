#include <string>
#include "cmdl_grammar.h"
namespace cmdlparser
{
	/// <summary>
	/// Class for static helper methods to parse a cmdl file.
	/// </summary>
	class CMDLCodecs
	{
	public:
		CMDLCodecs() = delete;
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
			return (c >= 'a' && c <= 'z')		// lower case character
				|| (c >= 'A' && c <= 'Z')	// upper case character
				|| c == '_';					// underscore
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
		inline static bool isBlockCommentStart(std::string::value_type first, std::string::value_type next)  noexcept
		{
			return first == '/' && next == '*'; // C++ style line comments ("/*").
		}
		inline static bool isBlockCommentEnd(std::string::value_type first, std::string::value_type next)  noexcept
		{
			return first == '*' && next == '/'; // C++ style line comments ("/*").
		}
		inline static const std::string::value_type* GetPreprocessor(const std::string::value_type* stream, int* tokenID, std::string::value_type* buffer, unsigned int bufferLength)
		{
			// Test if valid preprocessor directive. Otherwise, return empty buffer and current position in character stream.
			if (stream[0] != '#')
			{
				buffer[0] = '\0';
				*tokenID = 0;
				return stream;
			}
			stream++;
			stream = GetIdentifier(stream, tokenID, buffer, bufferLength);
			std::string name(buffer);
			buffer[0] = '\0';
			if (tokenID == 0 || name.size() <= 0)
			{
				throw std::exception("Hashtag ('#') starting preprocessor directive must be immediately followed by the name of the directive.");
			}
			if (name == "include")
			{
				*tokenID = TOKEN_INCLUDE;
			}
			else if (name == "model")
			{
				*tokenID = TOKEN_MODEL_NAME;
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "Preprocessor directive #" << name << " unknown or not supported.";
				throw std::exception(errorMessage.str().c_str());
			}
			return stream;
		}
		inline static const std::string::value_type* GetIdentifier(const std::string::value_type* stream, int* tokenID, std::string::value_type* buffer, unsigned int bufferLength)
		{
			// Test for identifiers in quotation marks
			if (stream[0] == '"')
			{
				stream++;
				// copy characters
				unsigned int i = 0;
				while (*stream)
				{
					if (*stream == '"')
					{
						buffer[i] = '\0';
						*tokenID = TOKEN_IDENTIFIER;
						return stream+1;
					}
					buffer[i] = *stream;
					i++;
					stream++;
					if (i >= bufferLength)
						throw std::exception("Identifier (string) too long.");
				}
				throw std::exception("Quoted identifier (string) does not end in current line. Did you forget a quotation mark ('\"')?");
			}
			else
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
						throw std::exception("Identifier (string) too long.");
				}
				buffer[i] = '\0';
				*tokenID = TOKEN_IDENTIFIER;
				return stream;
			}
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
}