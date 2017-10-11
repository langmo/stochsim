#pragma once
#include "stochsim_common.h"
#include <string>
#include <codecvt>
#include <memory>
#include "expression_common.h"
#include <map>
namespace stochsim
{
	/// <summary>
	/// A custom reaction rate is a formula stored as a mathematical expression, which can be evaluated to obtain the reaction rate as a double value.
	/// The expression may contain variables with the names of the reactants of the reaction.
	/// </summary>
	class ReactionRate
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		ReactionRate() noexcept
		{
		}
		
		void SetRateExpression(std::unique_ptr<expression::IExpression> rateExpression) noexcept
		{
			rateExpression_ = std::move(rateExpression);
		}
		const expression::IExpression* GetRateExpression() const noexcept
		{
			if (rateExpression_)
				return rateExpression_.get();
			else
				return nullptr;
		}

		/// <summary>
		/// Returns true if a custom reaction rate expression was set via an appropriate constructor, false otherwise.
		/// </summary>
		/// <returns>True if custom reaction rate.</returns>
		operator bool() const noexcept
		{
			return rateExpression_.operator bool();
		}

		/// <summary>
		/// Calculates the current rate of the reaction by solving the rate equation with the current species concentrations.
		/// Throws a std::exception if rate could not be calculated.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		/// <returns>The current rate of the reaction.</returns>
		double operator()(ISimInfo& simInfo) const
		{
			if (!operator bool())
				throw std::exception("Custom reaction rate not set.");
			return boundRateExpession_->Eval();
		}

		void Initialize(ISimInfo& simInfo, std::vector<std::shared_ptr<IState>>& reactants)
		{
			boundRateExpession_ = rateExpression_->Clone();
			bindVariables(simInfo, reactants);
			boundRateExpession_ = boundRateExpession_->Simplify();
		}
		void Uninitialize(ISimInfo& simInfo)
		{
			boundRateExpession_ = nullptr;
		}
	private:
		std::unique_ptr<expression::IExpression> boundRateExpession_;
		std::unique_ptr<expression::IExpression> rateExpression_;

		void bindVariables(ISimInfo& simInfo, std::vector<std::shared_ptr<IState>>& reactants)
		{
			
			auto defaultFunctions = expression::makeDefaultFunctions();
			auto defaultVariables = expression::makeDefaultVariables();
			expression::BindingLookup bindings = [this, &reactants, &defaultFunctions, &defaultVariables, &simInfo](const expression::identifier name)->std::unique_ptr<expression::IFunctionHolder>
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
					for (auto reactant : reactants)
					{
						if (reactant->GetName() == name)
						{
							std::function<expression::number()> holder = [reactant]() -> expression::number
							{
								return static_cast<expression::number>(reactant->Num());
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
			boundRateExpession_->Bind(bindings);
		}
	};
}

