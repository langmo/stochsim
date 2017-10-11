#pragma once
#include <string>
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
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="cmdlFilePath">Full path to the CMDL file which should be parsed.</param>
		/// <param name="logFilePath">Full path of a log file which is created while parsing. Set to an empty string to not print logs. Only has an effect if NDEBUG is not set.</param>
		CmdlParser(std::string cmdlFilePath, std::string logFilePath = "");
		~CmdlParser();
		/// <summary>
		/// Parses the CMDL file passed to the constructor, and initializes the provided simulation with the reactions and states defined in the CMDL file.
		/// Usually, the provided simulation file is empty. If it is not, and if it contains already reactions or species with the same names as defined in the CMDL file, the behavior is undefined.
		/// </summary>
		/// <param name="sim">Simulation to initialize with the reactions and states defined in the CMDL file.</param>
		void Parse(stochsim::Simulation& sim);

	private:
		std::string cmdlFilePath_;
		std::string logFilePath_;
		void* handle_;
		FILE* logFile_;

		void ParseToken(int tokenID, TerminalSymbol* token, CmdlParseTree& parseTree);
		void InitializeInternal();
		void UninitializeInternal();
	};
}
