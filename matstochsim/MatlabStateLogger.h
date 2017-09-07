#pragma once
#include "stochsim_interfaces.h"
#include "mex.h"
#include <memory>
#include <vector>
#include "MatlabParams.h"
class MatlabStateLogger :
	public stochsim::ILogger
{
public:
	MatlabStateLogger();
	virtual ~MatlabStateLogger();

	virtual void WriteLog(double time) override;
	virtual void Initialize(std::string baseFolder, stochsim::ISimInfo& simInfo) override;
	virtual void Uninitialize() override;

	void AddState(std::shared_ptr<stochsim::IState> state);

	MatlabParams::MatlabVariable GetResults();
	MatlabParams::MatlabVariable GetHeaders();

	void SetShouldLog(bool shouldLog);
	bool IsShouldLog();
	virtual bool WritesToDisk() const override;
private:
	MatlabParams::MatlabVariable result_;
	MatlabParams::MatlabVariable headers_;
	size_t rows_;
	size_t columns_;
	size_t currentRow_;
	std::vector<std::shared_ptr<stochsim::IState>> states_;
	bool shouldLog_;
};

