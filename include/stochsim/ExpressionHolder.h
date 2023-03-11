#pragma once
#include "stochsim_common.h"
#include <string>
#include <codecvt>
#include <memory>
#include "expression_common.h"
#include <map>
#include <unordered_map>
namespace stochsim
{
	/// <summary>
	/// Holds a mathematical expression, binds free variables upon initialization, and allows to evaluate the expression.
	/// </summary>
	class ExpressionHolder
	{
	private:
		typedef std::unordered_map<expression::identifier, std::unique_ptr<expression::number>> TemporaryVariables;
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		ExpressionHolder() noexcept : temporaryVariables_(10)
		{
		}

		void SetExpression(std::unique_ptr<expression::IExpression> expression) noexcept
		{
			expression_ = std::move(expression);
		}
		const expression::IExpression* GetExpression() const noexcept
		{
			if (expression_)
				return expression_.get();
			else
				return nullptr;
		}

		/// <summary>
		/// Returns true if a expression was set (is not NULL), false otherwise.
		/// </summary>
		/// <returns>True if expression is set.</returns>
		operator bool() const noexcept
		{
			return expression_.operator bool();
		}

		/// <summary>
		/// Calculates the current rate of the reaction by solving the rate equation with the current species concentrations.
		/// Throws a std::exception if rate could not be calculated.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		/// <returns>The current rate of the reaction.</returns>
		double operator()(ISimInfo& simInfo, const std::vector<Variable>& variables = {}) const
		{
			if (!operator bool())
				throw std::runtime_error("Expression not set.");

			// Set temporary variables
			bool rebind = false;
			for (auto& variable : variables)
			{
				auto& valuePtr = temporaryVariables_[static_cast<expression::identifier>(variable.first)];
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
				rebindTemporaryVariables();


			return boundExpession_->Eval();
		}

		void Initialize(ISimInfo& simInfo)
		{
			if (!operator bool())
				throw std::runtime_error("Expression not set.");
			temporaryVariables_.clear();
			boundExpession_ = expression_->Clone();
			bindVariables(simInfo);
			boundExpession_ = boundExpession_->Simplify();
		}
		void Uninitialize(ISimInfo& simInfo)
		{
			boundExpession_ = nullptr;
			temporaryVariables_.clear();
		}
	private:
		std::unique_ptr<expression::IExpression> boundExpession_;
		std::unique_ptr<expression::IExpression> expression_;
		mutable TemporaryVariables temporaryVariables_;

		void bindVariables(ISimInfo& simInfo)
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
					auto search = temporaryVariables_.find(name);
					if (search != temporaryVariables_.end())
					{
						auto valuePointer = search->second.get();
						std::function<expression::number()> holder = [valuePointer]() -> expression::number
						{
							return *valuePointer;
						};
						return expression::makeFunctionHolder(holder, true);
					}
					for (auto& state : simInfo.GetStates())
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
			boundExpession_->Bind(bindings);
		}
		void rebindTemporaryVariables() const
		{
			expression::BindingRegister bindings = [this](const expression::identifier name) -> std::unique_ptr<expression::IFunctionHolder>
			{
				auto search = temporaryVariables_.find(name);
				if (search != temporaryVariables_.end())
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
			boundExpession_->Bind(bindings);
		}
	};
}

