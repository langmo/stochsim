#include "MatlabStateLogger.h"
#include "mex.h"
#include <string>
#include <sstream>


MatlabStateLogger::MatlabStateLogger() : columns_(0), rows_(0), currentRow_(0), shouldLog_(true)
{
}


MatlabStateLogger::~MatlabStateLogger()
{
}

void MatlabStateLogger::WriteLog(double time)
{
	if (!shouldLog_)
		return;
	if (currentRow_ >= rows_)
		return;
		//throw std::exception("Cannot add row to simulation results since array holding simulation results is already full.");
	MatlabParams::AssignArrayElement(*result_, currentRow_, 0, time);
	for (size_t i = 0; i < states_.size(); i++)
	{
		MatlabParams::AssignArrayElement(*result_, currentRow_, i + 1, static_cast<double>(states_[i]->Num()));
	}
	currentRow_++;
}

void MatlabStateLogger::Initialize(std::string baseFolder, stochsim::ISimInfo & simInfo)
{
	if (!shouldLog_)
		return;
	currentRow_ = 0;
	columns_ = states_.size() + 1; // time is first column.
	rows_= static_cast<size_t>(::ceil(simInfo.RunTime() / simInfo.LogPeriod())+1); // we also have to count initial and final state.
	result_ = MatlabParams::CreateDoubleMatrix(rows_, columns_);
	headers_ = MatlabParams::CreateCell(1, columns_);
	MatlabParams::AssignCellElement(*headers_, 0, 0, "Time");
	for (size_t i = 0; i < states_.size(); i++)
	{
		MatlabParams::AssignCellElement(*headers_, 0, i+1, states_[i]->Name());
	}
}
void MatlabStateLogger::SetShouldLog(bool shouldLog)
{
	shouldLog_ = shouldLog;
}

bool MatlabStateLogger::IsShouldLog()
{
	return shouldLog_;
}
bool MatlabStateLogger::WritesToDisk() const
{
	return false;
}
void MatlabStateLogger::Uninitialize()
{
}
void MatlabStateLogger::AddState(std::shared_ptr<stochsim::IState> state)
{
	states_.push_back(std::move(state));
}

MatlabParams::MatlabVariable MatlabStateLogger::GetResults()
{
	rows_ = 0;
	return std::move(result_);
}

MatlabParams::MatlabVariable MatlabStateLogger::GetHeaders()
{
	return std::move(headers_);
}
