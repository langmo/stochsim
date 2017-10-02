#pragma once
#include "cmdl_grammar.h"
#include <string>
#include "Simulation.h"
namespace stochsim
{
	

	class CmdlParser
	{
	public:
		CmdlParser() = delete;

		static void Parse(const std::string& fileName, Simulation& sim);
	};
}
