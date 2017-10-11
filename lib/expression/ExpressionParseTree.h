#pragma once
#include <unordered_map>
#include <memory>
#include <sstream>
#include "expression_common.h"
#include "NumberExpression.h"
namespace expression
{
	class ExpressionParseTree
	{
	public:
		typedef std::unordered_map<expression::identifier, std::unique_ptr<expression::IExpression>> variable_collection;
		typedef std::unordered_map<expression::identifier, std::unique_ptr<expression::IFunctionHolder>> function_collection;
	public:
		ExpressionParseTree() : defaultFunctions_(expression::makeDefaultFunctions())
		{
			auto defaultVarValues = expression::makeDefaultVariables();
			for (auto& var : defaultVarValues)
			{
				defaultVariables_.emplace(var.first, std::unique_ptr<expression::IExpression>(new expression::NumberExpression(var.second)));
			}
		}
		/// <summary>
		/// Creates a variable with the given name, and the expression as the value. This corresponds to variables following late evaluation.
		/// </summary>
		/// <param name="name">Name of variable.</param>
		/// <param name="expression">Expression of variable.</param>
		void CreateVariable(expression::identifier name, std::unique_ptr<expression::IExpression> expression)
		{
			variables_[name] = std::move(expression);
		}
		/// <summary>
		/// Creates a variable with the given name and value. This corresponds to variables following early evaluation.
		/// </summary>
		/// <param name="name">Name of variable</param>
		/// <param name="value">Value of variable</param>
		void CreateVariable(expression::identifier name, expression::number value)
		{
			variables_[name] = std::make_unique<expression::NumberExpression>(value);
		}
		
		/// <summary>
		/// Finds the variable with the given name and returns its expression.
		/// If no variable with the given name exists, throws a std::exception.
		/// </summary>
		/// <param name="name">Name of the variable to evaluate.</param>
		/// <returns>Expression of the variable.</returns>
		std::unique_ptr<expression::IExpression> GetVariableExpression(const expression::identifier name) const
		{
			auto search = variables_.find(name);
			if (search != variables_.end())
			{
				return search->second->Clone();
			}
			auto default_search = defaultVariables_.find(name);
			if (default_search != defaultVariables_.end())
				return default_search->second->Clone();

			std::stringstream errorMessage;
			errorMessage << "Variable with name \"" << name << "\" not defined";
			throw std::exception(errorMessage.str().c_str());
		}

		std::unique_ptr<expression::IFunctionHolder> GetFunctionHandler(const expression::identifier& name) const
		{
			auto search = functions_.find(name);
			if (search != functions_.end())
			{
				return search->second->Clone();
			}
			auto default_search = defaultFunctions_.find(name);
			if (default_search != defaultFunctions_.end())
				return default_search->second->Clone();

			std::stringstream errorMessage;
			errorMessage << "Function with name \"" << name << "\" not defined";
			throw std::exception(errorMessage.str().c_str());
		}

		/// <summary>
		/// Takes an expression and evaluates it. If the expression could not be evaluated, throws a std::exception.
		/// </summary>
		/// <param name="expression">Expression to evaluate.</param>
		/// <returns>Value of expression.</returns>
		expression::number GetExpressionValue(const expression::IExpression* expression) const
		{
			auto clone = expression->Clone();
			auto bindings = GetBindingLookup();
			clone->Bind(bindings);
			return clone->Eval();
		}

		/// <summary>
		/// Finds the variable with the given name, evaluates its expression and returns the result.
		/// If no variable with the given name exists, or if the expression could not be evaluated, throws a std::exception.
		/// </summary>
		/// <param name="name">Name of the variable to evaluate.</param>
		/// <returns>Value of the variable.</returns>
		expression::number GetVariableValue(const expression::identifier& name) const
		{
			auto search = variables_.find(name);
			if (search != variables_.end())
			{
				auto clone = search->second->Clone();
				auto bindings = GetBindingLookup();
				clone->Bind(bindings);
				return clone->Eval();
			}
			auto default_search = defaultVariables_.find(name);
			if (default_search != defaultVariables_.end())
			{
				auto clone = default_search->second->Clone();
				auto bindings = GetBindingLookup();
				clone->Bind(bindings);
				return clone->Eval();
			}

			std::stringstream errorMessage;
			errorMessage << "Variable with name \"" << name << "\" not defined";
			throw std::exception(errorMessage.str().c_str());
		}
		
	public:
		/// <summary>
		/// Returns a binding for all defined variable.
		/// </summary>
		/// <returns>Function to lookup variable values.</returns>
		expression::BindingLookup GetBindingLookup() const noexcept
		{
			return [this](const expression::identifier name)->std::unique_ptr<expression::IFunctionHolder>
			{
				if (name[name.size() - 1] == ')' && name[name.size() - 2] == '(')
				{
					return this->GetFunctionHandler(name.substr(0, name.size()-2));
				}
				else
				{
					auto value = this->GetVariableValue(name);
					std::function<expression::number()> binding = [value]()->expression::number {return value; };
					return expression::makeFunctionHolder(binding, false);
				}
			};
		}
	public:
		const variable_collection& GetVariables()
		{
			return variables_;
		}
		const function_collection& GetFunctions()
		{
			return functions_;
		}
		void SetResult(std::unique_ptr<IExpression> result) noexcept
		{
			result_ = std::move(result);
		}
		const IExpression* GetResult() const noexcept
		{
			return result_.get();
		}
	private:
		variable_collection variables_;
		variable_collection defaultVariables_;
		function_collection functions_;
		function_collection defaultFunctions_;

		std::unique_ptr<IExpression> result_;
	};
}