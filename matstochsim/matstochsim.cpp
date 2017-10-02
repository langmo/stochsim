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
	// Get the command string
    /*char commandRaw[256];
	if (nrhs < 1 || mxGetString(prhs[0], commandRaw, sizeof(commandRaw)))
		mexErrMsgTxt("First input must be a command string less than 256 characters long.");
	std::string command(commandRaw);*/

    // Create new simulation
	if(command =="new")
	{
        /*// Check parameters
        if (nlhs != 1)
            mexErrMsgTxt("One output expected for command \"new\".");
        // Return a handle to a new C++ instance
        plhs[0] = convertPtr2Mat<SimulationWrapper>(new SimulationWrapper());*/
		SimulationWrapper* simulationWrapper = new SimulationWrapper();
		if (params.NumParams() > 1)
		{
			try
			{
				stochsim::CmdlParser::Parse(params.Get<std::string>(1), *simulationWrapper);
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

    // Check there is a second input, which should be the class instance handle
    /*if (nrhs < 2)
		mexErrMsgTxt("Second input should be a handle of a simulation object.");
    */
    // Delete
    if (command == "delete") 
	{
        // Destroy the C++ object
        //destroyObject<SimulationWrapper>(prhs[1]);
		destroyObject<SimulationWrapper>(simulationRaw);
        // Warn if other commands were ignored
        /*if (nlhs != 0 || nrhs != 2)
            mexWarnMsgTxt("Unexpected left or right hand side arguments ignored for command delete.");*/
        return;
    }
    
    // Get the class instance pointer from the second input
	//SimulationWrapper* simulation = convertMat2Ptr<SimulationWrapper>(prhs[1]);
	SimulationWrapper* simulation = convertMat2Ptr<SimulationWrapper>(simulationRaw);
	try
	{
		simulation->ParseCommand(command, /*MatlabParams(nlhs, plhs, nrhs-2, prhs+2)*/params.ShiftInputs(2));
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
