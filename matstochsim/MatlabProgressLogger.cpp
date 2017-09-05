#include "MatlabProgressLogger.h"
#include "mex.h"
#include <sstream> 
#include <iomanip>

MatlabProgressLogger::MatlabProgressLogger() : runtime_(1), shouldLog_(true)
{
}
void MatlabProgressLogger::WriteLog(double time)
{
	if (!shouldLog_)
		return;
	int progress = (int)(time / runtime_ * 100);
	std::stringstream message;
	message << "\b\b\b\b\b\b" << std::setw(5) << std::fixed << std::setprecision(1) << (time / runtime_ * 100) << "%%";
	::mexPrintf(message.str().c_str());
}
void MatlabProgressLogger::Initialize(std::string baseFolder, stochsim::ISimInfo& simInfo)
{
	if (!shouldLog_)
		return;
	runtime_ = simInfo.RunTime();
	::mexPrintf("Simulating model:   0.0%");
}
void MatlabProgressLogger::Uninitialize()
{
	if (!shouldLog_)
		return;
	::mexPrintf("\b\b\b\b\b\b Finished!\n");
}

void MatlabProgressLogger::SetShouldLog(bool shouldLog)
{
	shouldLog_ = shouldLog;
}

bool MatlabProgressLogger::IsShouldLog()
{
	return shouldLog_;
}
