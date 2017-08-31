#pragma once
#include "Simulation.h"
#include "mex.h"
#include "MatlabParams.h"
#include "PropensityReaction.h"
#include "DelayReaction.h"
#include "stochsim_interfaces.h"
#include <string>
#include "StateLogger.h"
class SimulationWrapper :
	public stochsim::Simulation
{
private:
	static constexpr char stateLoggerFile_[] = "states.csv";
	static constexpr char prefixSeparator_[] = "::";
	static constexpr char simulationPrefix_[] = "Simulation";
	static constexpr char statePrefix_[] = "State";
	static constexpr char simpleReactionPrefix_[] = "PropensityReaction";
	static constexpr char delayReactionPrefix_[] = "DelayReaction";
	void parseSimulationCommand(const std::string& methodName, MatlabParams& params);
	void parseStateCommand(std::shared_ptr<stochsim::IState>& state, const std::string& methodName, MatlabParams& params);
	void parsePropensityReactionCommand(std::shared_ptr<stochsim::PropensityReaction>& simpleReaction, const std::string& methodName, MatlabParams& params); 
	void parseDelayReactionCommand(std::shared_ptr<stochsim::DelayReaction<stochsim::Molecule>>& reaction, const std::string & methodName, MatlabParams & params);
public:
	SimulationWrapper();
	virtual ~SimulationWrapper();

	void parseCommand(const std::string& command, MatlabParams& params);

private:
	std::shared_ptr<stochsim::StateLogger> stateLogger_;
};

