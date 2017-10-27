#include "MatlabProgressLogger.h"
#include "mex.h"
#include <sstream> 
#include <iomanip>

MatlabProgressLogger::MatlabProgressLogger() : runtime_(1), shouldLog_(true), lastProgressPermille(0)
{
}
void MatlabProgressLogger::WriteLog(stochsim::ISimInfo& simInfo, double time)
{
	if (!shouldLog_)
		return;
	int progressPermille = (int)(time / runtime_ * 1000);
	if (progressPermille == lastProgressPermille)
		return;
	std::stringstream message;
	message << "\b\b\b\b\b\b" << std::setw(5) << std::fixed << std::setprecision(1) << (progressPermille/10.0) << "%%";
	::mexPrintf(message.str().c_str());
	::mexEvalString("drawnow;");
	lastProgressPermille = progressPermille;
}
void MatlabProgressLogger::Initialize(stochsim::ISimInfo& simInfo)
{
	if (!shouldLog_)
		return;
	runtime_ = simInfo.GetRunTime();
	::mexPrintf("Simulating model:   0.0%");
	::mexEvalString("drawnow;");
	lastProgressPermille = 0;
}
void MatlabProgressLogger::Uninitialize(stochsim::ISimInfo& simInfo)
{
	if (!shouldLog_)
		return;
	::mexPrintf("\b\b\b\b\b\b Finished!\n");
	::mexEvalString("drawnow;");
}

void MatlabProgressLogger::SetShouldLog(bool shouldLog)
{
	shouldLog_ = shouldLog;
}

bool MatlabProgressLogger::IsShouldLog()
{
	return shouldLog_;
}

bool MatlabProgressLogger::WritesToDisk() const
{
	return false;
}
