#include "mex.h"
#include "communicationhelper.h"
#include <string>
#include "SimulationWrapper.h"
#include "MatlabParams.h"
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{	
	// Get the command string
    char commandRaw[256];
	if (nrhs < 1 || mxGetString(prhs[0], commandRaw, sizeof(commandRaw)))
		mexErrMsgTxt("First input must be a command string less than 256 characters long.");
	std::string command(commandRaw);

    // Create new simulation
	if(command =="new")
	{
        // Check parameters
        if (nlhs != 1)
            mexErrMsgTxt("One output expected for command \"new\".");
        // Return a handle to a new C++ instance
        plhs[0] = convertPtr2Mat<SimulationWrapper>(new SimulationWrapper());
        return;
    }
    
    // Check there is a second input, which should be the class instance handle
    if (nrhs < 2)
		mexErrMsgTxt("Second input should be a handle of a simulation object.");
    
    // Delete
    if (command == "delete") 
	{
        // Destroy the C++ object
        destroyObject<SimulationWrapper>(prhs[1]);
        // Warn if other commands were ignored
        if (nlhs != 0 || nrhs != 2)
            mexWarnMsgTxt("Unexpected left or right hand side arguments ignored for command delete.");
        return;
    }
    
    // Get the class instance pointer from the second input
	SimulationWrapper* simulation = convertMat2Ptr<SimulationWrapper>(prhs[1]);
	try
	{
		simulation->ParseCommand(command, MatlabParams(nlhs, plhs, nrhs-2, prhs+2));
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
