#include "mex.h"
#include "communicationhelper.h"
#include <string>
#include "SimulationWrapper.h"
#include "MatlabParams.h"
#include "CmdlParser.h"
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{	
	MatlabParams params(nlhs, plhs, nrhs, prhs);
	std::string command;
	try
	{
		command = params.Get<std::string>(0);
	}
	catch (const std::exception& ex)
	{
		std::stringstream errorMessage;
		errorMessage << "First parameter must be the name of the class which should be called, or 'new': " << ex.what();
		mexErrMsgTxt(errorMessage.str().c_str());
	}
	catch (...)
	{
		std::stringstream errorMessage;
		errorMessage << "First parameter must be the name of the class which should be called, or 'new': Unexpected error.";
		mexErrMsgTxt(errorMessage.str().c_str());
	}

    // Create new simulation
	if(command =="new")
	{
        SimulationWrapper* simulationWrapper = new SimulationWrapper();
		if (params.NumParams() > 1)
		{
			try
			{
				auto fileName = params.Get<std::string>(1);
				std::string logName = fileName + ".log";

				MatlabParams::MatlabStruct variables;
				if (params.NumParams() > 2)
				{
					variables = params.Get<MatlabParams::MatlabStruct>(2);
				}
			
				cmdlparser::CmdlParser cmdlParser;
				for (auto variable : variables)
				{
					cmdlParser.AddVariable(expression::identifier(variable.first), static_cast<expression::number>(variable.second), false); 
				}
				cmdlParser.Parse(fileName, *simulationWrapper, logName);
			}
			catch (const std::exception& ex)
			{
				mexErrMsgTxt(ex.what());
			}
			catch (...)
			{
				mexErrMsgTxt("Unexpected error while trying to parse CMDL file.");
			}
		}
		params.Set(0, convertPtr2Mat<SimulationWrapper>(simulationWrapper));
        return;
    }
	const mxArray* simulationRaw;
    try
	{
		simulationRaw = params.Get<const mxArray*>(1);
	}
	catch (const std::exception& ex)
	{
		std::stringstream errorMessage;
		errorMessage << "Second parameter must be a handle to a simulation object, except when first parameter is 'new': " << ex.what();
		mexErrMsgTxt(errorMessage.str().c_str());
	}
	catch (...)
	{
		std::stringstream errorMessage;
		errorMessage << "Second parameter must be a handle to a simulation object, except when first parameter is 'new': Unexpected error.";
		mexErrMsgTxt(errorMessage.str().c_str());
	}

    // Delete
    if (command == "delete") 
	{
        // Destroy the C++ object
		destroyObject<SimulationWrapper>(simulationRaw);
        return;
    }
    
    // Get the class instance pointer from the second input
	SimulationWrapper* simulation = convertMat2Ptr<SimulationWrapper>(simulationRaw);
	try
	{
		simulation->ParseCommand(command, params.ShiftInputs(2));
	}
	catch (const std::runtime_error& re)
	{
		mexErrMsgTxt(re.what());
	}
	catch (const std::exception& ex)
	{
		mexErrMsgTxt(ex.what());
	}
	catch (std::string message)
	{
		mexErrMsgTxt(message.c_str());
	}
	catch (...)
	{
		mexErrMsgTxt("Unknown error occured.");
	}
}
