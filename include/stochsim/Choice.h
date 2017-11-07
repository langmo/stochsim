#pragma once
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include "stochsim_common.h"
#include "expression_common.h"
#include "ExpressionParser.h"
namespace stochsim
{
	/// <summary>
	/// A choice is a special kind of state. A call to Add does not increase the concentration of the choice. Instead, the boolean formula
	/// associated with this choice is evaluated. Depending on the outcome of this evaluation, either the concentrations of one or the other set of element states is increased according to their
	/// stochiometry. The same holds for Remove and Transform. 
	/// The typical use case is to use a Choice to implement conditionals for reactions. For example, a reaction might result in a product B with a certain probability, and in a product C with another, i.e.
	///      { B     if rand()>0.2
	/// A -> {
	///      { C     otherwise.
	/// This would then be implemented as a propensity reaction with A as the reactant, and a choice as the product. The boolean formula associated to the choice would be "rand()>0.2", the first set of
	/// products of the choice would contain only B with stochiometry 1, and the second set only C with stochiometry 1.
	/// The usage of a choice becomes specifically interesting when combined with reactions providing additional information (variables) which can be used in the boolean expression. For example, a delay
	/// reaction passes the number of how often its (sole) reactant was transformed.
	/// To be compatible with mass action kinetics, the concentration of a choice (Num()) is calculated as follows: First, the boolean expression is evaluated. Then,
	/// either only the elementsIfTrue, or the elementsIfFalse are considered based on the outcome of the evaluation of the condition. The return value is then the product
	/// of these elements, according to their stochiometry. That is, if the choice is between 2*A+C and 3*D, we return A*(A-1)*C if the choice condition evaluates to true, and D*(D-1)*(D-2) if not.
	/// </summary>
	class Choice :
		public IState
	{
	private:
		/// <summary>
		/// Structure to store the information about the products of a Choice, as well as their stochiometries.
		/// </summary>
		struct ReactionElementWithModifiers
		{
		public:
			Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			const MoleculeProperties properties_;
			ReactionElementWithModifiers(std::shared_ptr<IState> state, Stochiometry stochiometry, MoleculeProperties properties) : stochiometry_(stochiometry), state_(std::move(state)), properties_(std::move(properties))
			{
			}
		};

		typedef std::unordered_map<expression::identifier, std::unique_ptr<expression::number>> VariablesMap;
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of this choice. Since a choice is conveniently treated as a normal state to be able to make a choice whenever being able to make no choice (i.e. just increase the
		/// concentration of a given product), a choice also has to have a name which is not clashing with other state names.</param>
		/// <param name="condition">The boolean equation for this choice. The expression
		/// can contain any number of arbitrary variable names. These variables are automatically initialized to be zero. This value is only changed before an evaluation of the expression if the reaction responsible
		/// for triggering the choice (the reaction invoking the Add method) passes a varibale with the same name.</param>
		Choice(std::string name, std::unique_ptr<expression::IExpression> condition) : name_(std::move(name)), choiceEquation_(std::move(condition)), variables_(10)
		{
		}
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of this choice. Since a choice is conveniently treated as a normal state to be able to make a choice whenever being able to make no choice (i.e. just increase the
		/// concentration of a given product), a choice also has to have a name which is not clashing with other state names.</param>
		/// <param name="condition">The boolean equation for this choice as a string. The string is parsed immediately. If parsing fails e.g. due to a syntax error, an std::exception is thrown. The expression
		/// can contain any number of arbitrary variable names. These variables are automatically initialized to be zero. This value is only changed before an evaluation of the expression if the reaction responsible
		/// for triggering the choice (the reaction invoking the Add method) passes a varibale with the same name.</param>
		Choice(std::string name, std::string condition) : name_(std::move(name)), variables_(10)
		{
			SetCondition(condition);
		}
		/// <summary>
		/// A choice is not really a state, but only implements the state interface such that it can be added as a reactant or product of any reaction.
		/// Instead of the molecular number of the choice, we here evaluate the choice condition, and return a value which is compatible with mass action kinetics.
		/// That is, if the choice is between 2*A+C and 3*D, we return A*(A-1)*C if the choice condition evaluates to true, and D*(D-1)*(D-2) if not.
		/// </summary>
		/// <returns>Returns value compatible to mass action kinetics.</returns>
		virtual size_t Num(ISimInfo& simInfo) const override
		{
			return 0;
		}
		virtual void Add(ISimInfo& simInfo, size_t num = 1, const MoleculeProperties& moleculeProperties = defaultMoleculeProperties, const std::vector<Variable>& variables = {}) override
		{
			// Set parser variables
			bool rebind = false;
			for (auto& variable : variables)
			{
				auto& valuePtr = variables_[static_cast<expression::identifier>(variable.first)];
				if (!valuePtr)
				{
					valuePtr = std::make_unique<expression::number>(static_cast<expression::number>(variable.second));
					rebind = true;
				}
				else
				{
					*valuePtr = static_cast<expression::number>(variable.second);
				}
			}
			if (rebind)
				rebind_variables(simInfo, false);
			for (size_t i = 0; i < num; i++)
			{
				// Find out which choice was made by evaluating the formula with the current variable values.
				expression::number choice;
				try
				{
					choice = boundChoiceEquation_->Eval();
				}
				catch (const std::exception& e)
				{
					std::stringstream errorMessage;
					errorMessage << "Could not evaluate expression \"";
					choiceEquation_->PrintCmdl(errorMessage, false);
					errorMessage << "\" for choice " << name_ << ": " << e.what();
					throw std::exception(errorMessage.str().c_str());
				}
				// Depending of the choice, either increase one or the other sets of products.
				if (choice != 0)
				{
					for (const auto& product : elementsIfTrue_)
					{
						product.state_->Add(simInfo, product.stochiometry_, product.properties_, variables);
					}
				}
				else
				{
					for (const auto& product : elementsIfFalse_)
					{
						product.state_->Add(simInfo, product.stochiometry_, product.properties_, variables);
					}
				}
			}
		}
		virtual void Remove(ISimInfo& simInfo, size_t num = 1, const std::vector<Variable>& variables = {}) override
		{
			throw std::exception("Choices must only be used as products of a reaction, not as reactants (i.e. Remove must not be called).");
		}
		virtual void Transform(ISimInfo& simInfo, size_t num = 1, const MoleculeProperties& moleculeProperties = defaultMoleculeTransformation, const std::vector<Variable>& variables = {}) override
		{
			throw std::exception("Choices must only be used as products of a reaction, not as transformees (i.e. Transform must not be called).");
		}
		virtual void Initialize(ISimInfo& simInfo) override
		{
			variables_.clear();
			boundChoiceEquation_ = choiceEquation_->Clone();
			rebind_variables(simInfo, true);
			boundChoiceEquation_ = boundChoiceEquation_->Simplify();
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			variables_.clear();
			boundChoiceEquation_ = nullptr;
		}
		virtual std::string GetName() const noexcept override
		{
			return name_;
		}

		/// <summary>
		/// Adds a species as a product of the Choice, which gets increased according to its stochiometry when the Add method of the Choice is invoked, and if the boolean expression associated to this Choice evaluates to true.
		/// </summary>
		/// <param name="state">Species to add as a product when the boolean expression evaluates to true.</param>
		/// <param name="stochiometry">Number of molecules produced when the boolean expression evaluates to true.</param>
		void AddElementIfTrue(std::shared_ptr<IState> state, Stochiometry stochiometry = 1, MoleculeProperties moleculeProperties = defaultMoleculeProperties) noexcept
		{
			for (auto& product : elementsIfTrue_)
			{
				if (state == product.state_)
				{
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			elementsIfTrue_.emplace_back(state, stochiometry, moleculeProperties);
		}

		/// <summary>
		/// Adds a species as a product of the Choice, which gets increased according to its stochiometry when the Add method of the Choice is invoked, and if the boolean expression associated to this Choice evaluates to false.
		/// </summary>
		/// <param name="state">Species to add as a product when the boolean expression evaluates to false.</param>
		/// <param name="stochiometry">Number of molecules produced when the boolean expression evaluates to false.</param>
		void AddElementIfFalse(std::shared_ptr<IState> state, Stochiometry stochiometry = 1, MoleculeProperties moleculeProperties = defaultMoleculeProperties) noexcept
		{
			for (auto& product : elementsIfFalse_)
			{
				if (state == product.state_)
				{
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			elementsIfFalse_.emplace_back(state, stochiometry, moleculeProperties);
		}

		/// <summary>
		/// Adds a species which can be used in the expression determining the choice.
		/// </summary>
		/// <param name="state">Species to add as a modifier.</param>
		void AddModifier(std::shared_ptr<IState> state) noexcept
		{
			for (auto& modifier : modifiers_)
			{
				if (state == modifier)
				{
					return;
				}
			}
			modifiers_.emplace_back(std::move(state));
		}

		/// <summary>
		/// Returns all products and their stochiometries in the case the expression associated to this choice evaluates to true.
		/// </summary>
		/// <returns>Products of the reaction if expression evaluates to true.</returns>
		stochsim::Collection<stochsim::ReactionElement> GetElementsIfTrue() const noexcept
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& product : elementsIfTrue_)
			{
				returnVal.emplace_back(product.state_, product.stochiometry_);
			}
			return std::move(returnVal);
		}

		/// <summary>
		/// Returns all modifiers.
		/// </summary>
		/// <returns>Collection of all modifiers.</returns>
		stochsim::Collection<std::shared_ptr<IState>> GetModifiers() const noexcept
		{
			stochsim::Collection<std::shared_ptr<IState>> returnVal;
			for (auto& modifier : modifiers_)
			{
				returnVal.emplace_back(modifier);
			}
			return std::move(returnVal);
		}

		/// <summary>
		/// Returns all products and their stochiometries in the case the expression associated to this choice evaluates to false.
		/// </summary>
		/// <returns>Products of the reaction if expression evaluates to false.</returns>
		stochsim::Collection<stochsim::ReactionElement> GetElementsIfFalse() const noexcept
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& product : elementsIfFalse_)
			{
				returnVal.emplace_back(product.state_, product.stochiometry_);
			}
			return std::move(returnVal);
		}

		/// <summary>
		/// Returns the equation determining for which set of products the concentration is increased according to the stochiometry.
		/// </summary>
		/// <returns>Choice equation.</returns>
		const expression::IExpression* GetCondition() const noexcept
		{
			return choiceEquation_.get();
		}
		/// <summary>
		/// Sets the equation determining for which set of products the concentration is increased according to the stochiometry.
		/// </summary>
		/// <param name="choiceEquation">Choice equation.</param>
		void SetCondition(std::unique_ptr<expression::IExpression> choiceEquation) noexcept
		{
			choiceEquation_ = std::move(choiceEquation);
		}
		/// <summary>
		/// Sets the equation determining for which set of products the concentration is increased according to the stochiometry.
		/// The provided string is parsed to an IExpression, i.e. an internal representation of the formula. If this parsing fails, e.g. due to
		/// a syntax error in the equation, an std::exception is thrown.
		/// </summary>
		/// <param name="choiceEquation">Choice equation as a string.</param>
		void SetCondition(std::string choiceEquation) noexcept
		{
			expression::ExpressionParser parser;
			SetCondition(parser.Parse(choiceEquation, false, false));
		}

	private:
		void rebind_variables(ISimInfo& simInfo, bool all)
		{
			if (all)
			{
				auto defaultFunctions = expression::makeDefaultFunctions();
				auto defaultVariables = expression::makeDefaultVariables();
				expression::BindingRegister bindings = [this, &defaultFunctions, &defaultVariables, &simInfo](const expression::identifier name)->std::unique_ptr<expression::IFunctionHolder>
				{
					std::string stdName(name);
					if (name[name.size() - 1] == ')' && name[name.size() - 2] == '(')
					{
						if (stdName == "rand()")
						{
							std::function<expression::number()> holder = [&simInfo]() -> expression::number
							{
								return static_cast<expression::number>(simInfo.Rand());
							};
							return expression::makeFunctionHolder(holder, true);
						}
						auto default_search = defaultFunctions.find(name);
						if (default_search != defaultFunctions.end())
							return default_search->second->Clone();

					}
					else
					{
						auto search = variables_.find(name);
						if (search != variables_.end())
						{
							auto valuePointer = search->second.get();
							std::function<expression::number()> holder = [valuePointer]() -> expression::number
							{
								return *valuePointer;
							};
							return expression::makeFunctionHolder(holder, true);
						}

						for (auto& component : elementsIfTrue_)
						{
							auto& state = component.state_;
							if (state->GetName() == name)
							{
								std::function<expression::number()> holder = [state, &simInfo]() -> expression::number
								{
									return static_cast<expression::number>(state->Num(simInfo));
								};
								return expression::makeFunctionHolder(holder, true);
							}
						}
						for (auto& component : elementsIfFalse_)
						{
							auto& state = component.state_;
							if (state->GetName() == name)
							{
								std::function<expression::number()> holder = [state, &simInfo]() -> expression::number
								{
									return static_cast<expression::number>(state->Num(simInfo));
								};
								return expression::makeFunctionHolder(holder, true);
							}
						}
						for (auto& state : modifiers_)
						{
							if (state->GetName() == name)
							{
								std::function<expression::number()> holder = [state, &simInfo]() -> expression::number
								{
									return static_cast<expression::number>(state->Num(simInfo));
								};
								return expression::makeFunctionHolder(holder, true);
							}
						}
						if (stdName == "time")
						{
							std::function<expression::number()> holder = [&simInfo]() -> expression::number
							{
								return static_cast<expression::number>(simInfo.GetSimTime());
							};
							return expression::makeFunctionHolder(holder, true);
						}
						auto default_search = defaultVariables.find(name);
						if (default_search != defaultVariables.end())
						{
							auto value = default_search->second;
							std::function<expression::number()> binding = [value]()->expression::number {return value; };
							return expression::makeFunctionHolder(binding, false);
						}
					}
					return nullptr;
				};
				boundChoiceEquation_->Bind(bindings);
			}
			else
			{
				expression::BindingRegister bindings = [this](const expression::identifier name) -> std::unique_ptr<expression::IFunctionHolder>
				{
					auto search = variables_.find(name);
					if (search != variables_.end())
					{
						auto valuePointer = search->second.get();
						std::function<expression::number()> holder = [valuePointer]() -> expression::number
						{
							return *valuePointer;
						};
						return expression::makeFunctionHolder(holder, true);
					}
					return nullptr;
				};
				boundChoiceEquation_->Bind(bindings);
			}
		}

	private:
		const std::string name_;
		std::unique_ptr<expression::IExpression> choiceEquation_;
		std::unique_ptr<expression::IExpression> boundChoiceEquation_;
		std::vector<ReactionElementWithModifiers> elementsIfTrue_;
		std::vector<ReactionElementWithModifiers> elementsIfFalse_;
		std::vector<std::shared_ptr<IState>> modifiers_;
		VariablesMap variables_;
	};
}
