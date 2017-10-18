#pragma once
#include <string>
#include <unordered_map>
#include "expression_common.h"
#include "Simulation.h"
namespace cmdlparser
{
	class TerminalSymbol;
	class CmdlParseTree;
	/// <summary>
	/// A parser which parses CMDL files to initialize a given simulation.
	/// </summary>
	class CmdlParser
	{
	private:
		struct Variable
		{
			expression::number value_;
			bool overwritable_;
		};
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="logFilePath">Full path of a log file which is created while parsing. Set to an empty string to not print logs. Only has an effect if NDEBUG is not set.</param>
		CmdlParser(std::string logFilePath = "") noexcept;
		~CmdlParser();
		/// <summary>
		/// Parses the CMDL file passed to the constructor, and initializes the provided simulation with the reactions and states defined in the CMDL file.
		/// Usually, the provided simulation file is empty. If it is not, and if it contains already reactions or species with the same names as defined in the CMDL file, the behavior is undefined.
		/// </summary>
		/// <param name="cmdlFilePath">Full path to the CMDL file which should be parsed.</param>
		/// <param name="sim">Simulation to initialize with the reactions and states defined in the CMDL file.</param>
		void Parse(std::string cmdlFilePath, stochsim::Simulation& sim);
		/// <summary>
		/// Pre-defines a variable with the given name and value, which is treated as if it was defined at the top of the CMDL file. If overwritable is false, subsequent
		/// redefinitions of the variable inside the CMDL file are ignored (but not subsequent redefinitions using this function), simplifying e.g. parameter optimization and similar.
		/// </summary>
		/// <param name="name">Name of the variable</param>
		/// <param name="value">Value of the variable</param>
		/// <param name="overwritable">If true, variable can be redefined in the CMDL file. If false, redefinitions in the CMDL file are ignored.</param>
		/// <returns></returns>
		void AddVariable(expression::identifier name, expression::number value, bool overwritable = true) noexcept;
	private:
		std::string logFilePath_;
		void* handle_;
		FILE* logFile_;
		std::unordered_map<expression::identifier, Variable> variables_;

		void ParseToken(int tokenID, TerminalSymbol* token, CmdlParseTree& parseTree);
		void InitializeInternal();
		void UninitializeInternal();
	};
}
