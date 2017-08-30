#include "SimulationWrapper.h"
#include <sstream> 
#include "types.h"
#include "SimpleState.h"
#include "SimpleReaction.h"
#include <iomanip>

class MatlabProgressLoggerTask :
	public stochsim::LoggerTask
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
	virtual void Initialize(std::string baseFolder, stochsim::SimInfo& simInfo) override
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
	if (methodName == "CreateSimpleState")
	{
		std::string name = params.GetString(0);
		unsigned long initialCondition = params.GetNumber<unsigned long>(1);
		auto state = CreateState<stochsim::SimpleState>(name, initialCondition);
		stateLogger_->AddState(state);
		params.Set(0, name);
	}
	else if (methodName == "CreateSimpleReaction")
	{
		std::string name = params.GetString(0);
		double rateConstant = params.GetDouble(1);
		auto state = CreateReaction<stochsim::SimpleReaction>(name, rateConstant);
		params.Set(0, name);
	}
	else if (methodName == "Run")
	{
		double runtime = params.GetDouble(0);
		Run(runtime);
	}
	else if (methodName == "SetLogPeriod")
	{
		double logPeriod = params.GetDouble(0);
		GetLogger().SetLogPeriod(logPeriod);
	}
	else if (methodName == "GetLogPeriod")
	{
		double logPeriod = GetLogger().GetLogPeriod();
		params.Set(0, logPeriod);
	}
	else if (methodName == "SetBaseFolder")
	{
		std::string baseFolder = params.GetString(0);
		GetLogger().SetBaseFolder(baseFolder);
	}
	else if (methodName == "GetBaseFolder")
	{
		std::string baseFolder = GetLogger().GetBaseFolder();
		params.Set(0, baseFolder);
	}
	else if (methodName == "SetUniqueSubfolder")
	{
		bool uniqueSubFolder = params.GetBool(0);
		GetLogger().SetUniqueSubfolder(uniqueSubFolder);
	}
	else if (methodName == "IsUniqueSubfolder")
	{
		bool uniqueSubFolder = GetLogger().IsUniqueSubfolder();
		params.Set(0, uniqueSubFolder);
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class "<< simulationPrefix_<<".";
		throw std::exception(errorMessage.str().c_str());
	}
}

void SimulationWrapper::parseStateCommand(std::shared_ptr<stochsim::State>& state, const std::string & methodName, MatlabParams & params)
{
	if (methodName == "InitialCondition")
	{
		params.Set(0, state->InitialCondition());
	}
	else if (methodName == "Name")
	{
		params.Set(0, state->Name());
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class " << statePrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}

void SimulationWrapper::parseSimpleReactionCommand(std::shared_ptr<stochsim::SimpleReaction>& simpleReaction, const std::string & methodName, MatlabParams & params)
{
	if (methodName == "Name")
	{
		params.Set(0, simpleReaction->Name());
	}
	else if (methodName == "AddReactant")
	{
		std::string stateName = params.GetString(0);
		std::shared_ptr<stochsim::State> state = GetState(stateName);
		if (!state)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		unsigned int stochiometry;
		if (params.NumParams() > 1)
			stochiometry = params.GetNumber<unsigned int>(1);
		else
			stochiometry = 1;
		bool modifier;
		if (params.NumParams() > 2)
			modifier = params.GetNumber<bool>(2);
		else
			modifier = false;
		simpleReaction->AddReactant(state, stochiometry, modifier);
	}
	else if (methodName == "AddProduct")
	{
		std::string stateName = params.GetString(0);
		std::shared_ptr<stochsim::State> state = GetState(stateName);
		if (!state)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		unsigned int stochiometry;
		if (params.NumParams() > 1)
			stochiometry = params.GetNumber<unsigned int>(1);
		else
			stochiometry = 1;
		bool modifier;
		if (params.NumParams() > 2)
			modifier = params.GetNumber<bool>(2);
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

SimulationWrapper::SimulationWrapper()
{
	GetLogger().CreateTask<MatlabProgressLoggerTask>();
	stateLogger_ = GetLogger().CreateTask<stochsim::SimpleStateLoggerTask>(stateLoggerFile_);
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
		std::string stateName = params.GetString(0);
		std::shared_ptr<stochsim::State> state = GetState(stateName);
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
		std::string reactionName = params.GetString(0);
		std::shared_ptr<stochsim::PropensityReaction> reaction = GetPropensityReaction(reactionName);
		if (!reaction)
		{
			std::stringstream errorMessage;
			errorMessage << "Reaction with name " << reactionName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		auto simpleReaction = std::static_pointer_cast<stochsim::SimpleReaction>(reaction);
		if (!simpleReaction)
		{
			std::stringstream errorMessage;
			errorMessage << "Reaction with name " << reactionName << " is not a simple reaction.";
			throw std::exception(errorMessage.str().c_str());
		}
		parseSimpleReactionCommand(simpleReaction, methodName, params.ShiftInputs(1));
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Class " << className << " not known. Supported class names are " << simulationPrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}
