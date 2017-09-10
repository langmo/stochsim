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

void SimulationWrapper::ParseSimulationCommand(const std::string & methodName, MatlabParams& params)
{
	if (methodName == "CreateState")
	{
		std::string name = params.Get<std::string>(0);
		unsigned long initialCondition = params.Get<unsigned long>(1);
		auto state = CreateState<stochsim::State>(name, initialCondition);
		stateLogger_->AddState(state);
		resultLogger_->AddState(state);
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
		resultLogger_->AddState(state);
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
		auto composedState = std::dynamic_pointer_cast<stochsim::ComposedState<stochsim::Molecule>>(state);
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
	else if (methodName == "CreateTimerReaction")
	{
		std::string name = params.Get<std::string>(0);
		double fireTime = params.Get<double>(1);
		auto reaction = CreateReaction<stochsim::TimerReaction>(name, fireTime);
		params.Set(0, name);
	}
	else if (methodName == "Run")
	{
		resultLogger_->SetShouldLog(params.NumReturns() > 0);
		double runtime = params.Get<double>(0);
		Run(runtime);
		if (params.NumReturns() >= 2)
		{
			params.Set(0, resultLogger_->GetHeaders());
			params.Set(1, resultLogger_->GetResults());
		}
		else if (params.NumReturns() == 1)
		{
			params.Set(0, resultLogger_->GetResults());
		}
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
	else if (methodName == "SetResultFile")
	{
		std::string resultFile = params.Get<std::string>(0);
		if (resultFile.empty())
		{
			stateLogger_->SetShouldLog(false);
		}
		else
		{
			stateLogger_->SetShouldLog(true);
			stateLogger_->SetFileName(resultFile);
		}
	}
	else if (methodName == "GetResultFile")
	{
		std::string resultFile;
		if (stateLogger_->IsShouldLog())
			resultFile = stateLogger_->GetFileName();
		else
			resultFile = "";
		params.Set(0, resultFile);
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
	else if (methodName == "SetLogConsole")
	{
		bool logConsole = params.Get<bool>(0);
		progressLogger_->SetShouldLog(logConsole);
	}
	else if (methodName == "IsLogConsole")
	{
		bool logConsole = progressLogger_->IsShouldLog();
		params.Set(0, logConsole);
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class "<< simulationPrefix_<<".";
		throw std::exception(errorMessage.str().c_str());
	}
}

void SimulationWrapper::ParseStateCommand(std::shared_ptr<stochsim::IState>& state, const std::string & methodName, MatlabParams & params)
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
		auto composedState = std::dynamic_pointer_cast<stochsim::ComposedState<stochsim::Molecule>>(state);
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

void SimulationWrapper::ParsePropensityReactionCommand(std::shared_ptr<stochsim::PropensityReaction>& reaction, const std::string & methodName, MatlabParams & params)
{
	if (methodName == "Name")
	{
		params.Set(0, reaction->Name());
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
		reaction->AddReactant(state, stochiometry);
	}
	else if (methodName == "AddModifier")
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
		reaction->AddModifier(state, stochiometry);
	}
	else if (methodName == "AddTransformee")
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
		reaction->AddTransformee(state, stochiometry);
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
		reaction->AddProduct(state, stochiometry);
	}
	else if (methodName == "SetRateConstant")
	{
		double rateConstant = params.Get<double>(0);
		reaction->SetRateConstant(rateConstant);
	}
	else if (methodName == "GetRateConstant")
	{
		double rateConstant = reaction->GetRateConstant();
		params.Set(0, rateConstant);
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class " << simpleReactionPrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}

void SimulationWrapper::ParseDelayReactionCommand(std::shared_ptr<stochsim::DelayReaction<stochsim::Molecule>>& reaction, const std::string & methodName, MatlabParams & params)
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
		reaction->AddProduct(state, stochiometry);
	}
	else if (methodName == "SetDelay")
	{
		double delay = params.Get<double>(0);
		reaction->SetDelay(delay);
	}
	else if (methodName == "GetDelay")
	{
		double delay = reaction->GetDelay();
		params.Set(0, delay);
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class " << delayReactionPrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}

void SimulationWrapper::ParseTimerReactionCommand(std::shared_ptr<stochsim::TimerReaction>& reaction, const std::string & methodName, MatlabParams & params)
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

		reaction->AddProduct(state, stochiometry);
	}
	else if (methodName == "SetFireTime")
	{
		double fireTime = params.Get<double>(0);
		reaction->SetFireTime(fireTime);
	}
	else if (methodName == "GetFireTime")
	{
		double fireTime = reaction->GetFireTime();
		params.Set(0, fireTime);
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
	progressLogger_ = CreateLogger<MatlabProgressLogger>();
	stateLogger_ = CreateLogger<stochsim::StateLogger>(stateLoggerFile_);
	stateLogger_->SetShouldLog(false);
	resultLogger_ = CreateLogger<MatlabStateLogger>();
}


SimulationWrapper::~SimulationWrapper()
{
}

void SimulationWrapper::ParseCommand(const std::string & command, MatlabParams& params)
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
		ParseSimulationCommand(methodName, params);
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
		ParseStateCommand(state, methodName, params.ShiftInputs(1));
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
		auto simpleReaction = std::dynamic_pointer_cast<stochsim::PropensityReaction>(reaction);
		if (!simpleReaction)
		{
			std::stringstream errorMessage;
			errorMessage << "Reaction with name " << reactionName << " is not a simple reaction.";
			throw std::exception(errorMessage.str().c_str());
		}
		ParsePropensityReactionCommand(simpleReaction, methodName, params.ShiftInputs(1));
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
		auto composedReaction = std::dynamic_pointer_cast<stochsim::DelayReaction<stochsim::Molecule>>(reaction);
		if (!composedReaction)
		{
			std::stringstream errorMessage;
			errorMessage << "Reaction with name " << reactionName << " is not a delayed reaction.";
			throw std::exception(errorMessage.str().c_str());
		}
		ParseDelayReactionCommand(composedReaction, methodName, params.ShiftInputs(1));
	}
	else if (className == timerReactionPrefix_)
	{
		std::string reactionName = params.Get<std::string>(0);
		std::shared_ptr<stochsim::IDelayedReaction> reaction = GetDelayedReaction(reactionName);
		if (!reaction)
		{
			std::stringstream errorMessage;
			errorMessage << "Timer reaction with name " << reactionName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		auto timerReaction = std::dynamic_pointer_cast<stochsim::TimerReaction>(reaction);
		if (!timerReaction)
		{
			std::stringstream errorMessage;
			errorMessage << "Reaction with name " << reactionName << " is not a timer reaction.";
			throw std::exception(errorMessage.str().c_str());
		}
		ParseTimerReactionCommand(timerReaction, methodName, params.ShiftInputs(1));
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Class " << className << " not known. Supported class names are " << simulationPrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}
