#pragma once
#include <unordered_map>
#include <memory>
#include <sstream>
#include "expression_common.h"
#include "NumberExpression.h"
namespace cmdlparser
{
	class CmdlParseTree
	{
	public:
		typedef std::unordered_map<expression::identifier, std::unique_ptr<expression::IExpression>> variable_collection;
		typedef std::unordered_map<expression::identifier, std::unique_ptr<expression::IFunctionHolder>> function_collection;
		typedef std::unordered_map<expression::identifier, std::unique_ptr<ReactionDefinition>> reaction_collection;
		typedef std::unordered_map<expression::identifier, std::unique_ptr<ChoiceDefinition>> choice_collection;
		typedef std::function<void(expression::identifier)> include_file_callback;
	public:
		CmdlParseTree() : defaultFunctions_(expression::makeDefaultFunctions())
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

		void SetIncludeFileCallback(include_file_callback callback)
		{
			callback_ = callback;
		}

		/// <summary>
		/// Creates a variable with the given name and value. Variables defined via this methods have precedence over variables defined via CreateVariable(...).
		/// Useful to dynamically overwrite variable definitions in the CMDL file.
		/// </summary>
		/// <param name="name">Name of variable</param>
		/// <param name="value">Value of variable</param>
		void CreateFinalVariable(expression::identifier name, expression::number value)
		{
			finalVariables_[name] = std::make_unique<expression::NumberExpression>(value);
		}
		void CreateReaction(std::unique_ptr<ReactionLeftSide> reactants, std::unique_ptr<ReactionRightSide> products, std::unique_ptr<ReactionSpecifiers> specifiers)
		{ 
			std::stringstream name;
			name << "reaction_" << (reactions_.size() + 1); 
			auto nameC = name.str();
			auto nameI = expression::identifier(nameC.begin(), nameC.end());
			CreateReaction(nameI, std::move(reactants), std::move(products), std::move(specifiers));
		}
		void CreateReaction(expression::identifier name, std::unique_ptr<ReactionLeftSide> reactants, std::unique_ptr<ReactionRightSide> products, std::unique_ptr<ReactionSpecifiers> specifiers)
		{
			reactions_[name] = std::make_unique<ReactionDefinition>(std::move(reactants), std::move(products), std::move(specifiers));
		}
		expression::identifier CreateChoice(std::unique_ptr<expression::IExpression> condition, std::unique_ptr<ReactionRightSide> componentsIfTrue, std::unique_ptr<ReactionRightSide> componentsIfFalse)
		{
			std::stringstream name;
			name << "choice_" << (choices_.size() + 1);
			auto nameC = name.str();
			auto nameI = expression::identifier(nameC.begin(), nameC.end());
			choices_[nameI] = std::make_unique<ChoiceDefinition>(std::move(condition), std::move(componentsIfTrue), std::move(componentsIfFalse));
			return nameI;
		}

		void IncludeFile(expression::identifier file)
		{
			callback_(file);
		}
		/// <summary>
		/// Finds the variable with the given name and returns its expression.
		/// If no variable with the given name exists, a nullptr is returned.
		/// </summary>
		/// <param name="name">Name of the variable to evaluate.</param>
		/// <returns>Expression of the variable.</returns>
		std::unique_ptr<expression::IExpression> FindVariableExpression(const expression::identifier name) const
		{
			auto final_search = finalVariables_.find(name);
			if (final_search != finalVariables_.end())
			{
				return final_search->second->Clone();
			}
			auto search = variables_.find(name);
			if (search != variables_.end())
			{
				return search->second->Clone();
			}
			auto default_search = defaultVariables_.find(name);
			if (default_search != defaultVariables_.end())
				return default_search->second->Clone();

			return nullptr;
		}

		std::unique_ptr<expression::IFunctionHolder> FindFunctionHandler(const expression::identifier& name) const
		{
			auto search = functions_.find(name);
			if (search != functions_.end())
			{
				return search->second->Clone();
			}
			auto default_search = defaultFunctions_.find(name);
			if (default_search != defaultFunctions_.end())
				return default_search->second->Clone();

			return nullptr;
		}

		/// <summary>
		/// Takes an expression and evaluates it. If the expression could not be evaluated, throws a std::exception.
		/// </summary>
		/// <param name="expression">Expression to evaluate.</param>
		/// <returns>Value of expression.</returns>
		expression::number GetExpressionValue(const expression::IExpression* expression) const
		{
			auto clone = expression->Clone();
			auto bindings = GetBindingRegister();
			clone->Bind(bindings);
			return clone->Eval();
		}

		/// <summary>
		/// Finds the variable with the given name, evaluates its expression and returns the result.
		/// If no variable with the given name exists, or if the expression could not be evaluated, throws a std::exception.
		/// </summary>
		/// <param name="name">Name of the variable to evaluate.</param>
		/// <returns>Value of the variable.</returns>
		expression::number FindVariableValue(const expression::identifier& name) const
		{
			auto final_search = finalVariables_.find(name);
			if (final_search != finalVariables_.end())
			{
				auto clone = final_search->second->Clone();
				auto bindings = GetBindingRegister();
				clone->Bind(bindings);
				return clone->Eval();
			}
			auto search = variables_.find(name);
			if (search != variables_.end())
			{
				auto clone = search->second->Clone();
				auto bindings = GetBindingRegister();
				clone->Bind(bindings);
				return clone->Eval();
			}
			auto default_search = defaultVariables_.find(name);
			if (default_search != defaultVariables_.end())
			{
				auto clone = default_search->second->Clone();
				auto bindings = GetBindingRegister();
				clone->Bind(bindings);
				return clone->Eval();
			}

			std::stringstream errorMessage;
			errorMessage << "Variable with name \"" << name << "\" not defined";
			throw std::runtime_error(errorMessage.str().c_str());
		}
		
	private:
		/// <summary>
		/// Returns a binding for all defined variable.
		/// </summary>
		/// <returns>Function to lookup variable values.</returns>
		expression::BindingRegister GetBindingRegister() const noexcept
		{
			return [this](const expression::identifier name)->std::unique_ptr<expression::IFunctionHolder>
			{
				if (name[name.size() - 1] == ')' && name[name.size() - 2] == '(')
				{
					return this->FindFunctionHandler(name.substr(0, name.size()-2));
				}
				else
				{
					expression::number value;
					try
					{
						value = this->FindVariableValue(name);
					}
					catch (...)
					{
						return nullptr;
					}
					std::function<expression::number()> binding = [value]()->expression::number {return value; };
					return expression::makeFunctionHolder(binding, false);
				}
			};
		}
	public:
		const reaction_collection& GetReactions()
		{
			return reactions_;
		}
		const choice_collection& GetChoices()
		{
			return choices_;
		}
	private:
		variable_collection finalVariables_;
		variable_collection variables_;
		variable_collection defaultVariables_;
		function_collection functions_;
		function_collection defaultFunctions_;
		reaction_collection reactions_;
		choice_collection choices_;
		include_file_callback callback_;
	};
}