#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "CmdlParser.h"
#include "StateLogger.h"
#include "ProgressLogger.h"

std::string cmdGetOption(int &argc, char **argv, const std::string & option)
{
	char** const end = argv + argc;
	char ** itr = std::find(argv+1, end, option);// +1 to ignore exe name
	if (itr != end && ++itr != end)
	{
		return std::string(*itr);
	}
	return std::string();
}

bool cmdOptionExists(int &argc, char **argv, const std::string& option)
{
	char** const end = argv + argc;
	return std::find(argv+1, end, option) != end; // +1 to ignore exe name
}

void cmdHelp(std::ostream& stream, int &argc, char **argv)
{
	stream << "Stochsim command line interface, v0.8.0" << std::endl;
	stream << "Created by Moritz Lang" << std::endl;
	stream << "License: GNU General Public License, Version 3" << std::endl;
	stream << "Visit:   http://langmo.github.io/stochsim/" << std::endl;
	stream << "Usage:" << std::endl;
	stream << "         "<< argv[0] << " [-options] cmdlfile" << std::endl;
	stream << "with:" << std::endl;
	stream << "         cmdlfile\tpath to the CMDL file describing a stochastic model" << std::endl;
	
	stream << "options:" << std::endl;
	stream << "         -o    path of folder to save simulation results" << std::endl;
	stream << "               default: \"simulations\"" << std::endl;

	stream << "         -t    runtime of simulation" << std::endl;
	stream << "               default: 100" << std::endl;

	stream << "         -dt   stepsize of saving state to disk" << std::endl;
	stream << "               default: 1" << std::endl;
	stream << "         -h,-? display this help" << std::endl;
}

void runCustomModel(std::string modelPath, std::string folder, double runtime, double stepTime)
{
	// Construct simulation
	stochsim::Simulation sim;
	sim.SetBaseFolder(folder);
	sim.SetLogPeriod(stepTime);

	// Logging state values
	auto logger = sim.CreateLogger<stochsim::StateLogger>("states.csv");

	// Display simulation progress in console
	sim.CreateLogger<stochsim::ProgressLogger>();
	cmdlparser::CmdlParser cmdlParser;
	cmdlParser.Parse(modelPath, sim);
	for (auto& state : sim.GetStates())
	{
		logger->AddState(state);
	}
	sim.Run(runtime);
}


int main(int argc, char *argv[])
{
	if (argc<=1 || cmdOptionExists(argc, argv, "-h")
		|| cmdOptionExists(argc, argv, "-help")
		|| cmdOptionExists(argc, argv, "--help")
		|| cmdOptionExists(argc, argv, "-?"))
	{
		cmdHelp(std::cout, argc, argv);
		return 0;
	}
	std::string outputFolder = cmdGetOption(argc, argv, "-o");
	if (outputFolder.empty())
		outputFolder = "simulations";

	std::string endTimeStr = cmdGetOption(argc, argv, "-t");
	double endTime;
	if (endTimeStr.empty())
		endTime = 100;
	else
	{
		auto stream = endTimeStr.c_str();
		errno = 0; // strtod sets errno to ERANGE if number too large.
		char* pEnd;
		endTime = ::strtod(stream, &pEnd);
		if (errno != 0)
		{
			errno = 0;
			throw std::runtime_error("Number too large or number format invalid.");
		}
	}

	std::string stepTimeStr = cmdGetOption(argc, argv, "-dt");
	double stepTime;
	if (stepTimeStr.empty())
		stepTime = 1;
	else
	{
		auto stream = stepTimeStr.c_str();
		errno = 0; // strtod sets errno to ERANGE if number too large.
		char* pEnd;
		stepTime = ::strtod(stream, &pEnd);
		if (errno != 0)
		{
			errno = 0;
			throw std::runtime_error("Number too large or number format invalid.");
		}
	}

	// The last parameter must be the model path
	std::string model(argv[argc - 1]);
	try
	{
		runCustomModel(model, outputFolder, endTime, stepTime);
	}
	catch (const std::runtime_error& re)
	{
		std::cerr << "Runtime error: " << re.what() << std::endl;
		return 1;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error occurred: " << ex.what() << std::endl;
		return 1;
	}
	catch (std::string message)
	{
		std::cerr << "Error occured: " << message << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown error occured." << std::endl;
		return 1;
	}
	return 0;
}

