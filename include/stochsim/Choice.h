#pragma once
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include "stochsim_common.h"
#include "expression_common.h"
namespace stochsim
{
	/// <summary>
	/// A choice is a special kind of state. Different to most or all other states, its concentration is always zero. The concentration of a choice can thus not be decreased, and an
	/// error is thrown if trying to call Remove. In contrary, Add can (and should) be called, but a call to Add does not increase the concentration of the choice. Instead, the boolean formula
	/// associated with this choice is evaluated. Depending on the outcome of this evaluation, either the concentrations of one or the other set of product states is increased according to their
	/// stochiometry.
	/// The typical use case is to use a Choice to implement conditionals for reactions. For example, a reaction might result in a product B with a certain probability, and in a product C with another, i.e.
	///      { B     if rand()>0.2
	/// A -> {
	///      { C     otherwise.
	/// This would then be implemented as a propensity reaction with A as the reactant, and a choice as the product. The boolean formula associated to the choice would be "rand()>0.2", the first set of
	/// products of the choice would contain only B with stochiometry 1, and the second set only C with stochiometry 1.
	/// The usage of a choice becomes specifically interesting when combined with reactions providing additional information (variables) which can be used in the boolean expression. For example, a delay
	/// reaction passes the number of how often its (sole) reactant was transformed.
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
			ReactionElementWithModifiers(std::shared_ptr<IState> state, Stochiometry stochiometry) : stochiometry_(stochiometry), state_(std::move(state))
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
		/// <param name="choiceEquation">The boolean equation for this choice. To evaluate this equation we use muparser. Please see the documentation of muparser for the syntax of the expression. The expression
		/// can contain any number of arbitrary variable names. These variables are automatically initialized to be zero. This value is only changed before an evaluation of the expression if the reaction responsible
		/// for triggering the choice (the reaction invoking the Add method) passes a varibale with the same name.</param>
		Choice(std::string name, std::unique_ptr<expression::IExpression> choiceEquation) : name_(std::move(name)), choiceEquation_(std::move(choiceEquation)), variables_(10)
		{
		}
		/// <summary>
		/// A choice is not really a state, but only implements the state interface such that it can be added as a product of any reaction which allows to add an arbitrary state as a product.
		/// Since, however, a choice does not really have a concentration, this function always returns zero. This prevents at least some possible exceptions, e.g. when (invalidly) adding a Choice as a
		/// reactant to a propensity reaction following mass action kinetics, this reaction will never fire since a zero concentration of the choice then implies zero propensity of the reaction.
		/// </summary>
		/// <returns>Returns always zero.</returns>
		virtual size_t Num() const override
		{
			// Special kind of state, which only forwards addition of molecules to one of two choices. Thus, its concentration is always zero.
			return 0;
		}
		virtual void Add(ISimInfo& simInfo, size_t num = 1, std::initializer_list<Variable> variables = {}) override
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
					for (const auto& product : productsIfTrue_)
					{
						product.state_->Add(simInfo, product.stochiometry_, variables);
					}
				}
				else
				{
					for (const auto& product : productsIfFalse_)
					{
						product.state_->Add(simInfo, product.stochiometry_, variables);
					}
				}
			}
		}
		virtual void Remove(ISimInfo& simInfo, size_t num = 1, std::initializer_list<Variable> variables = {}) override
		{
			throw std::exception("Trying to decrease the concentration of a Choice state. Choices must only be used on the RHS of a reaction, and their concentration must only be increased.");
		}
		virtual void Transform(ISimInfo& simInfo, size_t num = 1, std::initializer_list<Variable> variables = {}) override
		{
			// do nothing.
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
		virtual std::string GetName() const override
		{
			return name_;
		}

		/// <summary>
		/// Adds a species as a product of the Choice, which gets increased according to its stochiometry when the Add method of the Choice is invoked, and if the boolean expression associated to this Choice evaluates to true.
		/// </summary>
		/// <param name="state">Species to add as a product when the boolean expression evaluates to true.</param>
		/// <param name="stochiometry">Number of molecules produced when the boolean expression evaluates to true.</param>
		void AddProductIfTrue(std::shared_ptr<IState> state, Stochiometry stochiometry = 1)
		{
			for (auto& product : productsIfTrue_)
			{
				if (state == product.state_)
				{
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			productsIfTrue_.emplace_back(state, stochiometry);
		}

		/// <summary>
		/// Adds a species as a product of the Choice, which gets increased according to its stochiometry when the Add method of the Choice is invoked, and if the boolean expression associated to this Choice evaluates to false.
		/// </summary>
		/// <param name="state">Species to add as a product when the boolean expression evaluates to false.</param>
		/// <param name="stochiometry">Number of molecules produced when the boolean expression evaluates to false.</param>
		void AddProductIfFalse(std::shared_ptr<IState> state, Stochiometry stochiometry = 1)
		{
			for (auto& product : productsIfFalse_)
			{
				if (state == product.state_)
				{
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			productsIfFalse_.emplace_back(state, stochiometry);
		}

		/// <summary>
		/// Returns all products and their stochiometries in the case the expression associated to this choice evaluates to true.
		/// </summary>
		/// <returns>Products of the reaction if expression evaluates to true.</returns>
		stochsim::Collection<stochsim::ReactionElement> GetProductsIfTrue() const
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& product : productsIfTrue_)
			{
				returnVal.emplace_back(product.state_, product.stochiometry_);
			}
			return std::move(returnVal);
		}

		/// <summary>
		/// Returns all products and their stochiometries in the case the expression associated to this choice evaluates to false.
		/// </summary>
		/// <returns>Products of the reaction if expression evaluates to false.</returns>
		stochsim::Collection<stochsim::ReactionElement> GetProductsIfFalse() const
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& product : productsIfFalse_)
			{
				returnVal.emplace_back(product.state_, product.stochiometry_);
			}
			return std::move(returnVal);
		}

		/// <summary>
		/// Returns the equation determining for which set of products the concentration is increased according to the stochiometry.
		/// </summary>
		/// <returns>Choice equation. For the syntax, see the documentation of muparser.</returns>
		const expression::IExpression* GetChoiceEquation() const
		{
			return choiceEquation_.get();
		}
		/// <summary>
		/// Sets the equation determining for which set of products the concentration is increased according to the stochiometry.
		/// </summary>
		/// <param name="choiceEquation">Choice equation. For the syntax, see the documentation of muparser.</param>
		void SetChoiceEquation(std::unique_ptr<expression::IExpression> choiceEquation)
		{
			choiceEquation_ = std::move(choiceEquation);
		}

	private:
		void rebind_variables(ISimInfo& simInfo, bool all)
		{
			if (all)
			{
				auto defaultFunctions = expression::makeDefaultFunctions();
				auto defaultVariables = expression::makeDefaultVariables();
				expression::BindingLookup bindings = [this, &defaultFunctions, &defaultVariables, &simInfo](const expression::identifier name)->std::unique_ptr<expression::IFunctionHolder>
				{
					if (name[name.size() - 1] == ')' && name[name.size() - 2] == '(')
					{
						if (name == "rand()")
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

						for (auto& component : productsIfTrue_)
						{
							auto& state = component.state_;
							if (state->GetName() == name)
							{
								std::function<expression::number()> holder = [state]() -> expression::number
								{
									return static_cast<expression::number>(state->Num());
								};
								return expression::makeFunctionHolder(holder, true);
							}
						}
						for (auto& component : productsIfFalse_)
						{
							auto& state = component.state_;
							if (state->GetName() == name)
							{
								std::function<expression::number()> holder = [state]() -> expression::number
								{
									return static_cast<expression::number>(state->Num());
								};
								return expression::makeFunctionHolder(holder, true);
							}
						}

						auto default_search = defaultVariables.find(name);
						if (default_search != defaultVariables.end())
						{
							auto value = default_search->second;
							std::function<expression::number()> binding = [value]()->expression::number {return value; };
							return expression::makeFunctionHolder(binding, false);
						}
					}
					std::stringstream errorMessage;
					errorMessage << "State or function with name \"" << name << "\" is not defined.";
					throw std::exception(errorMessage.str().c_str());
				};
				boundChoiceEquation_->Bind(bindings);
			}
			else
			{
				expression::BindingLookup bindings = [this](const expression::identifier name) -> std::unique_ptr<expression::IFunctionHolder>
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

					std::stringstream errorMessage;
					errorMessage << "State or function with name \"" << name << "\" is not defined.";
					throw std::exception(errorMessage.str().c_str());
				};
				boundChoiceEquation_->Bind(bindings);
			}
		}

	private:
		const std::string name_;
		std::unique_ptr<expression::IExpression> choiceEquation_;
		std::unique_ptr<expression::IExpression> boundChoiceEquation_;
		std::vector<ReactionElementWithModifiers> productsIfTrue_;
		std::vector<ReactionElementWithModifiers> productsIfFalse_;
		VariablesMap variables_;
	};
}
