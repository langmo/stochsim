#pragma once
#include <string>
#include <memory>

#include "stochsim_interfaces.h"
#include "Simulation.h"
#include "mex.h"
#include "MatlabParams.h"

#include "State.h"
#include "ComposedState.h"

#include "PropensityReaction.h"
#include "DelayReaction.h"
#include "TimerReaction.h"

#include "StateLogger.h"
#include "MatlabProgressLogger.h"
#include "MatlabStateLogger.h"

class SimulationWrapper :
	public stochsim::Simulation
{
public:
	static constexpr char stateLoggerFile_[] = "states.csv";
	static constexpr char prefixSeparator_[] = "::";
	static constexpr char simulationPrefix_[] = "Simulation";
	static constexpr char statePrefix_[] = "State";
	static constexpr char composedStatePrefix_[] = "ComposedState";
	static constexpr char unknownPrefix_[] = "Unknown";
	static constexpr char propensityReactionPrefix_[] = "PropensityReaction";
	static constexpr char delayReactionPrefix_[] = "DelayReaction";
	static constexpr char timerReactionPrefix_[] = "TimerReaction";
private:
	void ParseSimulationCommand(const std::string& methodName, MatlabParams& params);
	void ParseStateCommand(std::shared_ptr<stochsim::State>& state, const std::string& methodName, MatlabParams& params);
	void ParseComposedStateCommand(std::shared_ptr<stochsim::ComposedState>& state, const std::string& methodName, MatlabParams& params);
	void ParsePropensityReactionCommand(std::shared_ptr<stochsim::PropensityReaction>& simpleReaction, const std::string& methodName, MatlabParams& params); 
	void ParseDelayReactionCommand(std::shared_ptr<stochsim::DelayReaction>& reaction, const std::string & methodName, MatlabParams & params);
	void ParseTimerReactionCommand(std::shared_ptr<stochsim::TimerReaction>& reaction, const std::string & methodName, MatlabParams & params);
public:
	SimulationWrapper();
	virtual ~SimulationWrapper();

	void ParseCommand(const std::string& command, MatlabParams& params);

private:
	std::shared_ptr<stochsim::StateLogger> stateLogger_;
	std::shared_ptr<MatlabProgressLogger> progressLogger_;
	std::shared_ptr<MatlabStateLogger> resultLogger_;
};

