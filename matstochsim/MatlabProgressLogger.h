#pragma once
#include "stochsim_interfaces.h"
class MatlabProgressLogger :
	public stochsim::ILogger
{
public:
	MatlabProgressLogger();
	virtual void WriteLog(double time) override;
	virtual void Initialize(std::string baseFolder, stochsim::ISimInfo& simInfo) override;
	virtual void Uninitialize() override;
	void SetShouldLog(bool shouldLog);
	bool IsShouldLog();
private:
	double runtime_;
	bool shouldLog_;
};

