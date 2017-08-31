#include "SimulationWrapper.h"
#include <sstream> 
#include "stochsim_interfaces.h"
#include "State.h"
#include "ComposedState.h"
#include "DelayReaction.h"
#include "PropensityReaction.h"
#include "ComposedStateLogger.h"
#include <iomanip>
#include <functional>

class MatlabProgressLoggerTask :
	public stochsim::ILogger
{
public:
	MatlabProgressLoggerTask() : runtime_(1)
	{
	}
	virtual void WriteLog(double time) override
	{
		int progress = (int)(time / runtime_ * 100);
		std::stringstream message;
		message << "\b\b\b\b\b\b" << std::setw(5) << std::fixed << std::setprecision(1) << (time / runtime_ * 100) << "%%";
		::mexPrintf(message.str().c_str());
	}
	virtual void Initialize(std::string baseFolder, stochsim::ISimInfo& simInfo) override
	{
		runtime_ = simInfo.RunTime();
		::mexPrintf("Simulating model:   0.0%");
	}
	virtual void Uninitialize() override
	{
		::mexPrintf("\b\b\b\b\b\b Finished!\n");
	}

private:
	double runtime_;
};

void SimulationWrapper::parseSimulationCommand(const std::string & methodName, MatlabParams& params)
{
	if (methodName == "CreateState")
	{
		std::string name = params.Get<std::string>(0);
		unsigned long initialCondition = params.Get<unsigned long>(1);
		auto state = CreateState<stochsim::State>(name, initialCondition);
		stateLogger_->AddState(state);
		params.Set(0, name);
	}
	else if (methodName == "CreateComposedState")
	{
		std::string name = params.Get<std::string>(0);
		unsigned long initialCondition = params.Get<unsigned long>(1);
		std::shared_ptr<stochsim::ComposedState<stochsim::Molecule>> state;
		if (params.NumParams() >= 3)
		{
			unsigned long capacity = params.Get<unsigned long>(2);
			state = CreateState<stochsim::ComposedState<stochsim::Molecule>>(name, initialCondition, capacity);
		}
		else
			state = CreateState<stochsim::ComposedState<stochsim::Molecule>>(name, initialCondition);
		stateLogger_->AddState(state);
		params.Set(0, name);
	}
	else if (methodName == "CreatePropensityReaction")
	{
		std::string name = params.Get<std::string>(0);
		double rateConstant = params.Get<double>(1);
		auto reaction = CreateReaction<stochsim::PropensityReaction>(name, rateConstant);
		params.Set(0, name);
	}
	else if (methodName == "CreateDelayReaction")
	{
		std::string name = params.Get<std::string>(0);

		std::string stateName = params.Get<std::string>(1);
		std::shared_ptr<stochsim::IState> state = GetState(stateName);
		if (!state)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		auto composedState = std::static_pointer_cast<stochsim::ComposedState<stochsim::Molecule>>(state);
		if (!composedState)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " is not a composed state. Only composed states can be set as reactants to delayed reactions.";
			throw std::exception(errorMessage.str().c_str());
		}

		double delay = params.Get<double>(2);
		auto reaction = CreateReaction<stochsim::DelayReaction<stochsim::Molecule>>(name, composedState, delay);
		params.Set(0, name);
	}
	else if (methodName == "Run")
	{
		double runtime = params.Get<double>(0);
		Run(runtime);
	}
	else if (methodName == "SetLogPeriod")
	{
		double logPeriod = params.Get<double>(0);
		SetLogPeriod(logPeriod);
	}
	else if (methodName == "GetLogPeriod")
	{
		double logPeriod = GetLogPeriod();
		params.Set(0, logPeriod);
	}
	else if (methodName == "SetBaseFolder")
	{
		std::string baseFolder = params.Get<std::string>(0);
		SetBaseFolder(baseFolder);
	}
	else if (methodName == "GetBaseFolder")
	{
		std::string baseFolder = GetBaseFolder();
		params.Set(0, baseFolder);
	}
	else if (methodName == "SetUniqueSubfolder")
	{
		bool uniqueSubFolder = params.Get<bool>(0);
		SetUniqueSubfolder(uniqueSubFolder);
	}
	else if (methodName == "IsUniqueSubfolder")
	{
		bool uniqueSubFolder = IsUniqueSubfolder();
		params.Set(0, uniqueSubFolder);
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class "<< simulationPrefix_<<".";
		throw std::exception(errorMessage.str().c_str());
	}
}

void SimulationWrapper::parseStateCommand(std::shared_ptr<stochsim::IState>& state, const std::string & methodName, MatlabParams & params)
{
	if (methodName == "InitialCondition")
	{
		params.Set(0, state->InitialCondition());
	}
	else if (methodName == "Name")
	{
		params.Set(0, state->Name());
	}
	else if (methodName == "SaveFinalNumModificationsToFile")
	{
		auto composedState = std::static_pointer_cast<stochsim::ComposedState<stochsim::Molecule>>(state);
		if (!composedState)
		{
			std::stringstream errorMessage;
			errorMessage << "State " << state->Name() << " is not a ComposedState.";
			throw std::exception(errorMessage.str().c_str());
		}
		std::string fileName = params.Get<std::string>(0);
		size_t initialMaxModified;
		if (params.NumParams() > 1)
			initialMaxModified = params.Get<size_t>(1);
		else
			initialMaxModified = 20;
		auto logger = CreateLogger<stochsim::ComposedStateLogger>(fileName, initialMaxModified);
		composedState->AddRemoveListener(std::bind(&stochsim::ComposedStateLogger::RemoveListener, logger, std::placeholders::_1, std::placeholders::_2));
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class " << statePrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}

void SimulationWrapper::parsePropensityReactionCommand(std::shared_ptr<stochsim::PropensityReaction>& simpleReaction, const std::string & methodName, MatlabParams & params)
{
	if (methodName == "Name")
	{
		params.Set(0, simpleReaction->Name());
	}
	else if (methodName == "AddReactant")
	{
		std::string stateName = params.Get<std::string>(0);
		std::shared_ptr<stochsim::IState> state = GetState(stateName);
		if (!state)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		unsigned int stochiometry;
		if (params.NumParams() > 1)
			stochiometry = params.Get<unsigned int>(1);
		else
			stochiometry = 1;
		bool modifier;
		if (params.NumParams() > 2)
			modifier = params.Get<bool>(2);
		else
			modifier = false;
		simpleReaction->AddReactant(state, stochiometry, modifier);
	}
	else if (methodName == "AddProduct")
	{
		std::string stateName = params.Get<std::string>(0);
		std::shared_ptr<stochsim::IState> state = GetState(stateName);
		if (!state)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		unsigned int stochiometry;
		if (params.NumParams() > 1)
			stochiometry = params.Get<unsigned int>(1);
		else
			stochiometry = 1;
		bool modifier;
		if (params.NumParams() > 2)
			modifier = params.Get<bool>(2);
		else
			modifier = false;
		simpleReaction->AddProduct(state, stochiometry, modifier);
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class " << simpleReactionPrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}

void SimulationWrapper::parseDelayReactionCommand(std::shared_ptr<stochsim::DelayReaction<stochsim::Molecule>>& reaction, const std::string & methodName, MatlabParams & params)
{
	if (methodName == "Name")
	{
		params.Set(0, reaction->Name());
	}
	else if (methodName == "AddProduct")
	{
		std::string stateName = params.Get<std::string>(0);
		std::shared_ptr<stochsim::IState> state = GetState(stateName);
		if (!state)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		unsigned int stochiometry;
		if (params.NumParams() > 1)
			stochiometry = params.Get<unsigned int>(1);
		else
			stochiometry = 1;
		bool modifier;
		if (params.NumParams() > 2)
			modifier = params.Get<bool>(2);
		else
			modifier = false;
		reaction->AddProduct(state, stochiometry, modifier);
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class " << delayReactionPrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}

SimulationWrapper::SimulationWrapper()
{
	CreateLogger<MatlabProgressLoggerTask>();
	stateLogger_ = CreateLogger<stochsim::StateLogger>(stateLoggerFile_);
}


SimulationWrapper::~SimulationWrapper()
{
}

void SimulationWrapper::parseCommand(const std::string & command, MatlabParams& params)
{
	std::string::size_type prefixEnd = command.find(prefixSeparator_);
	if (prefixEnd == std::string::npos)
	{
		std::stringstream errorMessage;
		errorMessage << "All command strings except new and delete must be prefixed with the class whose method should be called, followed by "<<prefixSeparator_ << " and the method to be called, e.g. " << simulationPrefix_ <<prefixSeparator_<<"methodName";
		throw std::exception(errorMessage.str().c_str());
	}
	std::string className = command.substr(0, prefixEnd);
	if (className.empty())
	{
		throw std::exception("Provided class name is empty.");
	}
	std::string methodName = command.substr(prefixEnd + strlen(prefixSeparator_));
	if (methodName.empty())
	{
		throw std::exception("Provided method name is empty.");
	}
	// Switch between supported classes
	if (className == simulationPrefix_)
	{
		parseSimulationCommand(methodName, params);
		return;
	}
	else if (className == statePrefix_)
	{
		std::string stateName = params.Get<std::string>(0);
		std::shared_ptr<stochsim::IState> state = GetState(stateName);
		if (!state)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		parseStateCommand(state, methodName, params.ShiftInputs(1));
	}
	else if (className == simpleReactionPrefix_)
	{
		std::string reactionName = params.Get<std::string>(0);
		std::shared_ptr<stochsim::IPropensityReaction> reaction = GetPropensityReaction(reactionName);
		if (!reaction)
		{
			std::stringstream errorMessage;
			errorMessage << "Reaction with name " << reactionName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		auto simpleReaction = std::static_pointer_cast<stochsim::PropensityReaction>(reaction);
		if (!simpleReaction)
		{
			std::stringstream errorMessage;
			errorMessage << "Reaction with name " << reactionName << " is not a simple reaction.";
			throw std::exception(errorMessage.str().c_str());
		}
		parsePropensityReactionCommand(simpleReaction, methodName, params.ShiftInputs(1));
	}
	else if (className == delayReactionPrefix_)
	{
		std::string reactionName = params.Get<std::string>(0);
		std::shared_ptr<stochsim::IDelayedReaction> reaction = GetDelayedReaction(reactionName);
		if (!reaction)
		{
			std::stringstream errorMessage;
			errorMessage << "Delay reaction with name " << reactionName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		auto composedReaction = std::static_pointer_cast<stochsim::DelayReaction<stochsim::Molecule>>(reaction);
		if (!composedReaction)
		{
			std::stringstream errorMessage;
			errorMessage << "Delay reaction with name " << reactionName << " is not a delayed reaction.";
			throw std::exception(errorMessage.str().c_str());
		}
		parseDelayReactionCommand(composedReaction, methodName, params.ShiftInputs(1));
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Class " << className << " not known. Supported class names are " << simulationPrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}
