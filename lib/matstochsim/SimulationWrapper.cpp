#include "SimulationWrapper.h"
#include <sstream> 
#include <iomanip>
#include <functional>
#include <utility>

#include "ComposedStateLogger.h"

std::string GetStateReference(const std::shared_ptr<stochsim::IState>& state)
{
	std::string stateRef;
	if (dynamic_cast<stochsim::ComposedState*>(state.get()))
	{
		stateRef = SimulationWrapper::composedStatePrefix_;
	}
	else if (dynamic_cast<stochsim::State*>(state.get()))
	{
		stateRef = SimulationWrapper::statePrefix_;
	}
	else if (dynamic_cast<stochsim::Choice*>(state.get()))
	{
		stateRef = SimulationWrapper::choicePrefix_;
	}
	else
	{
		stateRef = SimulationWrapper::unknownPrefix_;
	}
	stateRef += SimulationWrapper::prefixSeparator_;
	stateRef += state->GetName();
	return stateRef;
}
std::string GetReactionReference(const std::shared_ptr<stochsim::IPropensityReaction>& reaction)
{
	std::string reactionRef;
	if (dynamic_cast<stochsim::PropensityReaction*>(reaction.get()))
	{
		reactionRef = SimulationWrapper::propensityReactionPrefix_;
	}
	else
	{
		reactionRef = SimulationWrapper::unknownPrefix_;
	}
	reactionRef += SimulationWrapper::prefixSeparator_;
	reactionRef += reaction->GetName();
	return reactionRef;
}
std::string GetReactionReference(const std::shared_ptr<stochsim::IEventReaction>& reaction)
{
	std::string reactionRef;
	if (dynamic_cast<stochsim::DelayReaction*>(reaction.get()))
	{
		reactionRef = SimulationWrapper::delayReactionPrefix_;
	}
	else if (dynamic_cast<stochsim::TimerReaction*>(reaction.get()))
	{
		reactionRef = SimulationWrapper::timerReactionPrefix_;
	}
	else
	{
		reactionRef = SimulationWrapper::unknownPrefix_;
	}
	reactionRef += SimulationWrapper::prefixSeparator_;
	reactionRef += reaction->GetName();
	return reactionRef;
}

std::pair<MatlabParams::MatlabVariable, MatlabParams::MatlabVariable> toMatlab(const stochsim::Collection<stochsim::ReactionElement>& reactionElements)
{
	size_t numElements = reactionElements.size();
	auto stateRefs = MatlabParams::CreateCell(1, numElements);
	auto stochs = MatlabParams::CreateDoubleMatrix(1, numElements);
	size_t i = 0;
	for (auto element : reactionElements)
	{
		if (i >= numElements)
			break;
		MatlabParams::AssignCellElement(*stateRefs, 0, i, GetStateReference(element.state_));
		MatlabParams::AssignArrayElement(*stochs, 0, i, element.stochiometry_);
		i++;
	}
	return std::pair<MatlabParams::MatlabVariable, MatlabParams::MatlabVariable>(std::move(stateRefs), std::move(stochs));
}



void SimulationWrapper::ParseSimulationCommand(const std::string & methodName, MatlabParams& params)
{
	if (methodName == "CreateState")
	{
		std::string name = params.Get<std::string>(0);
		unsigned long initialCondition = params.Get<unsigned long>(1);
		auto state = CreateState<stochsim::State>(name, initialCondition);
		params.Set(0, GetStateReference(state));
	}
	else if (methodName == "CreateChoice")
	{
		std::string name = params.Get<std::string>(0);
		auto choiceEquation = params.Get<std::string>(1);
		auto state = CreateState<stochsim::Choice>(name, choiceEquation);
		params.Set(0, GetStateReference(state));
	}
	else if (methodName == "CreateComposedState")
	{
		std::string name = params.Get<std::string>(0);
		unsigned long initialCondition = params.Get<unsigned long>(1);
		std::shared_ptr<stochsim::ComposedState> state;
		if (params.NumParams() >= 3)
		{
			unsigned long capacity = params.Get<unsigned long>(2);
			state = CreateState<stochsim::ComposedState>(name, initialCondition, capacity);
		}
		else
			state = CreateState<stochsim::ComposedState>(name, initialCondition);
		params.Set(0, GetStateReference(state));
	}
	else if (methodName == "CreatePropensityReaction")
	{
		std::string name = params.Get<std::string>(0);
		std::shared_ptr<stochsim::PropensityReaction> reaction;
		if (params.IsString(1))
		{
			std::string rateEquation = params.Get<std::string>(1);
			reaction = CreateReaction<stochsim::PropensityReaction>(name, rateEquation);
		}
		else
		{
			double rateConstant = params.Get<double>(1);
			reaction = CreateReaction<stochsim::PropensityReaction>(name, rateConstant);
		}
		params.Set(0, GetReactionReference(reaction));
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
		auto composedState = std::dynamic_pointer_cast<stochsim::ComposedState>(state);
		if (!composedState)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " is not a composed state. Only composed states can be set as reactants to delayed reactions.";
			throw std::exception(errorMessage.str().c_str());
		}

		double delay = params.Get<double>(2);
		auto reaction = CreateReaction<stochsim::DelayReaction>(name, composedState, delay);
		params.Set(0, GetReactionReference(reaction));
	}
	else if (methodName == "CreateTimerReaction")
	{
		std::string name = params.Get<std::string>(0);
		double fireTime = params.Get<double>(1);
		auto reaction = CreateReaction<stochsim::TimerReaction>(name, fireTime);
		params.Set(0, GetReactionReference(reaction));
	}
	else if (methodName == "GetStates")
	{
		auto states = GetStates();
		size_t numStates = states.size();
		auto stateRefs = params.CreateCell(1, numStates);
		size_t i = 0;
		for (auto state : states)
		{
			if (i >= numStates)
				break;
			MatlabParams::AssignCellElement(*stateRefs, 0, i, GetStateReference(state));
			i++;
		}
		params.Set(0, stateRefs.release());
	}
	else if (methodName == "GetState")
	{
		std::string stateName = params.Get<std::string>(0);
		auto state = GetState(stateName);
		if (!state)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		params.Set(0, GetStateReference(state));
	}
	else if (methodName == "GetReactions")
	{
		auto propensityReactions = GetPropensityReactions();
		auto eventReactions = GetEventReactions();
		size_t numReactions = propensityReactions.size() + eventReactions.size();
		auto reactionRefs = params.CreateCell(1, numReactions);
		size_t i = 0;
		for (auto reaction : propensityReactions)
		{
			if (i >= numReactions)
				break;
			MatlabParams::AssignCellElement(*reactionRefs, 0, i, GetReactionReference(reaction));
			i++;
		}
		for (auto reaction : eventReactions)
		{
			if (i >= numReactions)
				break;
			MatlabParams::AssignCellElement(*reactionRefs, 0, i, GetReactionReference(reaction));
			i++;
		}
		params.Set(0, reactionRefs.release());
	}
	else if (methodName == "GetReaction")
	{
		std::string reactionName = params.Get<std::string>(0);
		auto propensityReaction = GetPropensityReaction(reactionName);
		if (propensityReaction)
		{
			params.Set(0, GetReactionReference(propensityReaction));
			return;
		}
		auto eventReaction = GetEventReaction(reactionName);
		if (eventReaction)
		{
			params.Set(0, GetReactionReference(eventReaction));
			return;
		}
		std::stringstream errorMessage;
		errorMessage << "Reaction with name " << reactionName << " not defined in simulation.";
		throw std::exception(errorMessage.str().c_str());		
	}
	else if (methodName == "Run")
	{
		resultLogger_->SetShouldLog(params.NumReturns() > 0);
		double runtime = params.Get<double>(0);
		Run(runtime);
		if (params.NumReturns() >= 2)
		{
			params.Set(0, resultLogger_->GetHeaders().release());
			params.Set(1, resultLogger_->GetResults().release());
		}
		else if (params.NumReturns() == 1)
		{
			params.Set(0, resultLogger_->GetResults().release());
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
void SimulationWrapper::ParseStateCommand(std::shared_ptr<stochsim::State>& state, const std::string & methodName, MatlabParams & params)
{
	if (methodName == "GetInitialCondition")
	{
		params.Set(0, state->GetInitialCondition());
	}
	else if (methodName == "SetInitialCondition")
	{
		unsigned long initialCondition = params.Get<unsigned long>(0);
		auto composedState = std::dynamic_pointer_cast<stochsim::ComposedState>(state);
		if (composedState)
		{
			composedState->SetInitialCondition(static_cast<size_t>(initialCondition));
			return;
		}
		auto simpleState = std::dynamic_pointer_cast<stochsim::State>(state);
		if (simpleState)
		{
			simpleState->SetInitialCondition(static_cast<size_t>(initialCondition));
			return;
		}
		std::stringstream errorMessage;
		errorMessage << "State " << state->GetName() << " is not a State nor a ComposedState.";
		throw std::exception(errorMessage.str().c_str());
	}
	else if (methodName == "GetName")
	{
		params.Set(0, state->GetName());
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class " << statePrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}
void SimulationWrapper::ParseComposedStateCommand(std::shared_ptr<stochsim::ComposedState>& state, const std::string & methodName, MatlabParams & params)
{
	if (methodName == "GetInitialCondition")
	{
		params.Set(0, state->GetInitialCondition());
	}
	else if(methodName == "SetInitialCondition")
	{
		unsigned long initialCondition = params.Get<unsigned long>(0);
		auto composedState = std::dynamic_pointer_cast<stochsim::ComposedState>(state);
		if (composedState)
		{
			composedState->SetInitialCondition(static_cast<size_t>(initialCondition));
			return;
		}
		auto simpleState = std::dynamic_pointer_cast<stochsim::State>(state);
		if (simpleState)
		{
			simpleState->SetInitialCondition(static_cast<size_t>(initialCondition));
			return;
		}
		std::stringstream errorMessage;
		errorMessage << "State " << state->GetName() << " is not a State nor a ComposedState.";
		throw std::exception(errorMessage.str().c_str());
	}
	else if (methodName == "GetName")
	{
		params.Set(0, state->GetName());
	}
	else if (methodName == "SaveFinalNumModificationsToFile")
	{
		auto composedState = std::dynamic_pointer_cast<stochsim::ComposedState>(state);
		if (!composedState)
		{
			std::stringstream errorMessage;
			errorMessage << "State " << state->GetName() << " is not a ComposedState.";
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
		errorMessage << "Method " << methodName << " not known for class " << composedStatePrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}

void SimulationWrapper::ParsePropensityReactionCommand(std::shared_ptr<stochsim::PropensityReaction>& reaction, const std::string & methodName, MatlabParams & params)
{
	if (methodName == "GetName")
	{
		params.Set(0, reaction->GetName());
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
	else if (methodName == "GetReactants")
	{
		auto elements = reaction->GetReactants();
		auto pair = toMatlab(elements);
		params.Set(0, pair.first.release());
		params.Set(1, pair.second.release());
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
	else if (methodName == "GetModifiers")
	{
		auto elements = reaction->GetModifiers();
		auto pair = toMatlab(elements);
		params.Set(0, pair.first.release());
		params.Set(1, pair.second.release());
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
	else if (methodName == "GetTransformees")
	{
		auto elements = reaction->GetTransformees();
		auto pair = toMatlab(elements);
		params.Set(0, pair.first.release());
		params.Set(1, pair.second.release());
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
	else if (methodName == "GetProducts")
	{
		auto elements = reaction->GetProducts();
		auto pair = toMatlab(elements);
		params.Set(0, pair.first.release());
		params.Set(1, pair.second.release());
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
	else if (methodName == "SetRateEquation")
	{
		auto rateEquation = params.Get<std::string>(0);
		reaction->SetRateEquation(rateEquation);
	}
	else if (methodName == "GetRateEquation")
	{
		auto rateEquation = reaction->GetRateEquation();
		params.Set(0, rateEquation->ToCmdl());
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class " << propensityReactionPrefix_ << ".";
		throw std::exception(errorMessage.str().c_str());
	}
}

void SimulationWrapper::ParseDelayReactionCommand(std::shared_ptr<stochsim::DelayReaction>& reaction, const std::string & methodName, MatlabParams & params)
{
	if (methodName == "GetName")
	{
		params.Set(0, reaction->GetName());
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
	else if (methodName == "GetProducts")
	{
		auto elements = reaction->GetProducts();
		auto pair = toMatlab(elements);
		params.Set(0, pair.first.release());
		params.Set(1, pair.second.release());
	}
	else if (methodName == "GetReactants")
	{
		auto reactant = reaction->GetReactant();
		// create collection to offer a common interface to Matlab.
		stochsim::Collection<stochsim::ReactionElement> elements = { stochsim::ReactionElement (reactant, 1)};
		auto pair = toMatlab(elements);
		params.Set(0, pair.first.release());
		params.Set(1, pair.second.release());
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
	if (methodName == "GetName")
	{
		params.Set(0, reaction->GetName());
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
	else if (methodName == "GetProducts")
	{
		auto elements = reaction->GetProducts();
		auto pair = toMatlab(elements);
		params.Set(0, pair.first.release());
		params.Set(1, pair.second.release());
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
void SimulationWrapper::ParseChoiceCommand(std::shared_ptr<stochsim::Choice>& choice, const std::string & methodName, MatlabParams & params)
{
	if (methodName == "GetName")
	{
		params.Set(0, choice->GetName());
	}
	else if (methodName == "AddProductIfTrue")
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

		choice->AddProductIfTrue(state, stochiometry);
	}
	else if (methodName == "AddProductIfFalse")
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

		choice->AddProductIfFalse(state, stochiometry);
	}
	else if (methodName == "GetProductsIfTrue")
	{
		auto elements = choice->GetProductsIfTrue();
		auto pair = toMatlab(elements);
		params.Set(0, pair.first.release());
		params.Set(1, pair.second.release());
	}
	else if (methodName == "GetProductsIfFalse")
	{
		auto elements = choice->GetProductsIfFalse();
		auto pair = toMatlab(elements);
		params.Set(0, pair.first.release());
		params.Set(1, pair.second.release());
	}
	else if (methodName == "SetChoiceEquation")
	{
		auto choiceEquation = params.Get<std::string>(0);
		choice->SetChoiceEquation(choiceEquation);
	}
	else if (methodName == "GetChoiceEquation")
	{
		auto choiceEquation = choice->GetChoiceEquation();
		params.Set(0, choiceEquation->ToCmdl());
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Method " << methodName << " not known for class " << choicePrefix_ << ".";
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
		auto stateObj = std::dynamic_pointer_cast<stochsim::State>(state);
		if (!stateObj)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name "<< stateName << " is not a simple state.";
			throw std::exception(errorMessage.str().c_str());
		}
		ParseStateCommand(stateObj, methodName, params.ShiftInputs(1));
	}
	else if (className == composedStatePrefix_)
	{
		std::string stateName = params.Get<std::string>(0);
		std::shared_ptr<stochsim::IState> state = GetState(stateName);
		if (!state)
		{
			std::stringstream errorMessage;
			errorMessage << "Composed state (or any state) with name " << stateName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		auto stateObj = std::dynamic_pointer_cast<stochsim::ComposedState>(state);
		if (!stateObj)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " is not a composed state.";
			throw std::exception(errorMessage.str().c_str());
		}
		ParseComposedStateCommand(stateObj, methodName, params.ShiftInputs(1));
	}
	else if (className == choicePrefix_)
	{
		std::string stateName = params.Get<std::string>(0);
		std::shared_ptr<stochsim::IState> state = GetState(stateName);
		if (!state)
		{
			std::stringstream errorMessage;
			errorMessage << "Choice (or any state) with name " << stateName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		auto stateObj = std::dynamic_pointer_cast<stochsim::Choice>(state);
		if (!stateObj)
		{
			std::stringstream errorMessage;
			errorMessage << "State with name " << stateName << " is not a choice.";
			throw std::exception(errorMessage.str().c_str());
		}
		ParseChoiceCommand(stateObj, methodName, params.ShiftInputs(1));
	}
	else if (className == propensityReactionPrefix_)
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
		std::shared_ptr<stochsim::IEventReaction> reaction = GetEventReaction(reactionName);
		if (!reaction)
		{
			std::stringstream errorMessage;
			errorMessage << "Delay reaction with name " << reactionName << " not defined in simulation.";
			throw std::exception(errorMessage.str().c_str());
		}
		auto composedReaction = std::dynamic_pointer_cast<stochsim::DelayReaction>(reaction);
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
		std::shared_ptr<stochsim::IEventReaction> reaction = GetEventReaction(reactionName);
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

void SimulationWrapper::AddState(std::shared_ptr<stochsim::IState> state)
{
	Simulation::AddState(state);
	stateLogger_->AddState(state);
	resultLogger_->AddState(state);
}
