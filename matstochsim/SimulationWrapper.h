#pragma once
#include "Simulation.h"
#include "mex.h"
#include "MatlabParams.h"
#include "SimpleReaction.h"
#include "types.h"
#include <string>
#include "SimpleStateLoggerTask.h"
class SimulationWrapper :
	public stochsim::Simulation
{
private:
	static constexpr char stateLoggerFile_[] = "states.csv";
	static constexpr char prefixSeparator_[] = "::";
	static constexpr char simulationPrefix_[] = "Simulation";
	static constexpr char statePrefix_[] = "State";
	static constexpr char simpleReactionPrefix_[] = "SimpleReaction";
	void parseSimulationCommand(const std::string& methodName, MatlabParams& params);
	void parseStateCommand(std::shared_ptr<stochsim::State>& state, const std::string& methodName, MatlabParams& params);
	void parseSimpleReactionCommand(std::shared_ptr<stochsim::SimpleReaction>& simpleReaction, const std::string& methodName, MatlabParams& params); 

public:
	SimulationWrapper();
	virtual ~SimulationWrapper();

	void parseCommand(const std::string& command, MatlabParams& params);

private:
	std::shared_ptr<stochsim::SimpleStateLoggerTask> stateLogger_;
};

