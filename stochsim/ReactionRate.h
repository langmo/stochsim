#pragma once
#include "stochsim.h"
#include "muParser.h"
#include <string>
#include <codecvt>
#include <memory>
#include "expression/expression.h"
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
		/// Default constructor.
		/// Sets the reaction rate to zero.
		/// </summary>
		ReactionRate() noexcept
		{
		}
		/// <summary>
		/// Constructor.
		/// Sets the reaction rate expression and all reactants of the reaction. The expression may contain variables with the names of the reactants of the reaction.
		/// </summary>
		/// <param name="rateExpression">Custom reaction rate expression.</param>
		/// <param name="reactants">Reactants of the reaction.</param>
		ReactionRate(const expression::expression_base* rateExpression, const std::vector<std::shared_ptr<IState>>& reactants)
		{
			expression::binding_lookup bindings = [&reactants](const expression::identifier name) -> std::unique_ptr<expression::function_holder_base>
			{
				for (auto reactant : reactants)
				{
					if (reactant->GetName() == name)
					{
						std::function<expression::number()> holder = [reactant]() -> expression::number
						{
							return static_cast<expression::number>(reactant->Num());
						};
						return expression::make_function_holder(holder, true);
					}
				}
				std::stringstream errorMessage;
				errorMessage << "State or function with name \"" << name << "\" is not defined.";
				throw std::exception(errorMessage.str().c_str());
			};
			boundRateExpession_ = rateExpression->clone();
			boundRateExpession_->bind(bindings);
			boundRateExpession_ = boundRateExpession_->simplify();

		}
		/// <summary>
		/// Copy constructor.
		/// </summary>
		/// <param name="other">Reation rate to copy.</param>
		ReactionRate(const ReactionRate& other)
		{
			boundRateExpession_ = other.boundRateExpession_->clone();
		}
		/// <summary>
		/// Move constructor.
		/// </summary>
		/// <param name="other">Reaction rate to swap contents with.</param>
		ReactionRate(ReactionRate&& other) noexcept
		{
			boundRateExpession_.swap(other.boundRateExpession_);
		}
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		/// <param name="other">Reation rate to copy.</param>
		/// <returns>Reference to this object.</returns>
		ReactionRate& operator=(const ReactionRate& other)
		{
			boundRateExpession_ = other.boundRateExpession_->clone();
			return *this;
		}
		/// <summary>
		/// Move assignement operator.
		/// </summary>
		/// <param name="other">Reaction rate to swap contents with.</param>
		/// <returns>Reference to this object.</returns>
		ReactionRate& operator=(ReactionRate&& other) noexcept
		{
			boundRateExpession_.swap(other.boundRateExpession_);
			return *this;
		}
		/// <summary>
		/// Returns true if a custom reaction rate expression was set via an appropriate constructor, false otherwise.
		/// </summary>
		/// <returns>True if custom reaction rate.</returns>
		operator bool() const noexcept
		{
			return boundRateExpession_.operator bool();
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
			return boundRateExpession_->eval();
		}
	private:
		std::unique_ptr<expression::expression_base> boundRateExpession_;
	};
}

