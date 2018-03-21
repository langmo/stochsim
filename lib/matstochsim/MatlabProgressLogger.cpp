#include "MatlabProgressLogger.h"
#include "mex.h"
#include <sstream> 
#include <iomanip>

// Undocumented Matlab method to detect user to have pressed CTRL-C. Requires to include libut.lib.
#ifdef __cplusplus 
extern "C" bool utIsInterruptPending();
#else
extern bool utIsInterruptPending();
#endif

MatlabProgressLogger::MatlabProgressLogger() : runtime_(1), shouldLog_(true), lastProgressPermille_(0), lastMessageLength_(0), lastProgressTime_(std::chrono::steady_clock::now()), firstProgressTime_(lastProgressTime_)
{
}
void MatlabProgressLogger::WriteLog(stochsim::ISimInfo& simInfo, double time)
{
	// check if user pressed Ctrl-C 
	if (utIsInterruptPending()) 
	{        
		if (shouldLog_)
			::mexPrintf("\n");
		throw std::exception("User interrupt.");
	}

	if (!shouldLog_)
		return;
	int progressPermille = (int)(time / runtime_ * 1000);
	if (progressPermille == lastProgressPermille_)
		return;

	auto currentTime = std::chrono::steady_clock::now();
	if (time == 0)
	{
		lastProgressTime_ = currentTime;
		firstProgressTime_ = currentTime;
	}
	auto remainingDuration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastProgressTime_).count() * (1000- progressPermille) / 1000;
	auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - firstProgressTime_).count();
	
	std::string delMessage(lastMessageLength_, '\b');
	
	std::stringstream messageStream;
	messageStream << std::setw(5) << std::fixed << std::setprecision(1) << (progressPermille / 10.0) << "%%, Elapsed: " << totalDuration << "s, Remaining: " << remainingDuration << "s.";

	auto message = messageStream.str();
	lastMessageLength_ = message.size()-1; // minus one, because Matlab requires %%, but only counts as one character.

	::mexPrintf((delMessage+message).c_str());
	::mexEvalString("drawnow;");
	lastProgressPermille_ = progressPermille;
	lastProgressTime_ = currentTime;
}
void MatlabProgressLogger::Initialize(stochsim::ISimInfo& simInfo)
{
	if (!shouldLog_)
		return;

	lastProgressTime_ = std::chrono::steady_clock::now();
	firstProgressTime_ = std::chrono::steady_clock::now();

	runtime_ = simInfo.GetRunTime();
	::mexPrintf("Simulating model:   0.0%");
	::mexEvalString("drawnow;");
	lastProgressPermille_ = 0;
	lastMessageLength_ = 6;
}
void MatlabProgressLogger::Uninitialize(stochsim::ISimInfo& simInfo)
{
	if (!shouldLog_)
		return;

	auto currentTime = std::chrono::steady_clock::now();
	auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - firstProgressTime_).count();

	std::string delMessage(lastMessageLength_, '\b');

	std::stringstream messageStream;
	messageStream << std::setw(5) << std::fixed << std::setprecision(1) << "100%%, Elapsed: " << totalDuration << "s, Finished!\n";

	auto message = messageStream.str();

	::mexPrintf((delMessage + message).c_str());
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
