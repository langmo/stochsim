#include "mex.h"
#include "communicationhelper.h"
#include <string>
#include "SimulationWrapper.h"
#include "MatlabParams.h"
#include "CmdlParser.h"
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{	
	MatlabParams params(nlhs, plhs, nrhs, prhs);
	auto command = params.Get<std::string>(0);

    // Create new simulation
	if(command =="new")
	{
        SimulationWrapper* simulationWrapper = new SimulationWrapper();
		if (params.NumParams() > 1)
		{
			auto fileName = params.Get<std::string>(1);
			std::string logName = fileName + ".log";
			try
			{
				cmdlparser::CmdlParser cmdlParser(fileName, logName);
				cmdlParser.Parse(*simulationWrapper);
			}
			catch (const std::exception& ex)
			{
				mexErrMsgTxt(ex.what());
			}
			catch (...)
			{
				mexErrMsgTxt("Unexpected error occured.");
			}
		}
		params.Set(0, convertPtr2Mat<SimulationWrapper>(simulationWrapper));
        return;
    }
    
	auto simulationRaw = params.Get<const mxArray*>(1);

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
