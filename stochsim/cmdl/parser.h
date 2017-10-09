#pragma once
#include "grammar.h"
#include "symbols.h"
#include "parse_tree.h"
#include <string>
#include "../Simulation.h"
namespace cmdl
{
	

	class parser
	{
	public:
		parser(std::string cmdlFilePath, std::string logFilePath = "");
		~parser();
		void parse(stochsim::Simulation& sim);

	private:
		std::string cmdlFilePath_;
		std::string logFilePath_;
		void* handle_;
		FILE* logFile_;

		void parse_token(int tokenID, cmdl::terminal_symbol* token, cmdl::parse_tree& parseTree);
		void initialize_internal();
		void uninitialize_internal();
	};
}
