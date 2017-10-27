#pragma once
#include "stochsim_common.h"
class MatlabProgressLogger :
	public stochsim::ILogger
{
public:
	MatlabProgressLogger();
	virtual void WriteLog(stochsim::ISimInfo& simInfo, double time) override;
	virtual void Initialize(stochsim::ISimInfo& simInfo) override;
	virtual void Uninitialize(stochsim::ISimInfo& simInfo) override;
	void SetShouldLog(bool shouldLog);
	bool IsShouldLog();

	virtual bool WritesToDisk() const override;
private:
	double runtime_;
	bool shouldLog_;
	int lastProgressPermille;
};

