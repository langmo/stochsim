#pragma once
#include <random>
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
	public:
		CmdlParseTree() : defaultVariables_(MakeDefaultVariables()), defaultFunctions_(MakeDefaultFunctions())
		{
		}
		/// <summary>
		/// Creates a variable with the given name, and the expression as the value. This corresponds to variables following early evaluation.
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
		void CreateReaction(std::unique_ptr<ReactionSide> reactants, std::unique_ptr<ReactionSide> products, std::unique_ptr<ReactionSpecifiers> specifiers)
		{ 
			std::stringstream name;
			name << "reaction_" << (reactions_.size() + 1); 
			auto nameC = name.str();
			auto nameI = expression::identifier(nameC.begin(), nameC.end());
			CreateReaction(nameI, std::move(reactants), std::move(products), std::move(specifiers));
		}
		void CreateReaction(expression::identifier name, std::unique_ptr<ReactionSide> reactants, std::unique_ptr<ReactionSide> products, std::unique_ptr<ReactionSpecifiers> specifiers)
		{
			reactions_[name] = std::make_unique<ReactionDefinition>(std::move(reactants), std::move(products), std::move(specifiers));
		}
		expression::identifier CreateChoice(std::unique_ptr<expression::IExpression> condition, std::unique_ptr<ReactionSide> componentsIfTrue, std::unique_ptr<ReactionSide> componentsIfFalse)
		{
			std::stringstream name;
			name << "choice_" << (choices_.size() + 1);
			auto nameC = name.str();
			auto nameI = expression::identifier(nameC.begin(), nameC.end());
			choices_[nameI] = std::make_unique<ChoiceDefinition>(std::move(condition), std::move(componentsIfTrue), std::move(componentsIfFalse));
			return nameI;
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
		static std::unordered_map<expression::identifier, std::unique_ptr<expression::IExpression>> MakeDefaultVariables() noexcept
		{
			std::unordered_map<expression::identifier, std::unique_ptr<expression::IExpression>> defaultVariables;
			defaultVariables.emplace(expression::identifier("true"), std::unique_ptr<expression::IExpression>(new expression::NumberExpression(expression::number_true)));
			defaultVariables.emplace(expression::identifier("false"), std::unique_ptr<expression::IExpression>(new expression::NumberExpression(expression::number_false)));
			defaultVariables.emplace(expression::identifier("pi"), std::unique_ptr<expression::IExpression>(new expression::NumberExpression(expression::number(3.141592653589793))));
			defaultVariables.emplace(expression::identifier("e"), std::unique_ptr<expression::IExpression>(new expression::NumberExpression(expression::number(2.718281828459046))));
			return std::move(defaultVariables);
		}

		static std::unordered_map<expression::identifier, std::unique_ptr<expression::IFunctionHolder>> MakeDefaultFunctions() noexcept
		{
			std::unordered_map<expression::identifier, std::unique_ptr<expression::IFunctionHolder>> defaultFunctions;
			
			defaultFunctions.emplace("min", expression::makeFunctionHolder(
				(expression::number(*)(expression::number, expression::number))(&fmin), false));
			defaultFunctions.emplace("max", expression::makeFunctionHolder(
				(expression::number(*)(expression::number, expression::number))(&fmax), false));
			defaultFunctions.emplace("mod", expression::makeFunctionHolder(
				(expression::number(*)(expression::number, expression::number))(&fmod), false));
			
			defaultFunctions.emplace("sin", expression::makeFunctionHolder(
				(expression::number (*)(expression::number))(&sin), false));
			defaultFunctions.emplace("cos", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&cos, false));
			defaultFunctions.emplace("tan", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&tan, false));

			defaultFunctions.emplace("asin", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))(&asin), false));
			defaultFunctions.emplace("acos", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&acos, false));
			defaultFunctions.emplace("atan", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&atan, false));

			defaultFunctions.emplace("sinh", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))(&sinh), false));
			defaultFunctions.emplace("cosh", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&cosh, false));
			defaultFunctions.emplace("tanh", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&tanh, false));

			defaultFunctions.emplace("asinh", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))(&asinh), false));
			defaultFunctions.emplace("acosh", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&acosh, false));
			defaultFunctions.emplace("atanh", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&atanh, false));

			defaultFunctions.emplace("abs", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&abs, false));
			
			defaultFunctions.emplace("ceil", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&ceil, false));
			defaultFunctions.emplace("floor", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&floor, false));
			defaultFunctions.emplace("round", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&round, false));
			
			defaultFunctions.emplace("erf", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&erf, false));
			defaultFunctions.emplace("exp", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&exp, false));
			defaultFunctions.emplace("exp2", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&exp2, false));
			defaultFunctions.emplace("log", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&log, false));
			defaultFunctions.emplace("log10", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&log10, false));
			defaultFunctions.emplace("log2", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&log2, false));
			
			defaultFunctions.emplace("pow", expression::makeFunctionHolder(
				(expression::number(*)(expression::number, expression::number))&pow, false));
			defaultFunctions.emplace("sqrt", expression::makeFunctionHolder(
				(expression::number(*)(expression::number))&sqrt, false));

			defaultFunctions.emplace("rand", expression::makeFunctionHolder(
				static_cast<std::function<expression::number()>>(
					[]() -> expression::number
			{
				static std::default_random_engine randomEngine(std::random_device{}());
				static std::uniform_real<expression::number> randomUniform;
				return randomUniform(randomEngine);
			}
			), true));
			return std::move(defaultFunctions);
		}
		
	private:
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
		const reaction_collection& GetReactions()
		{
			return reactions_;
		}
		const choice_collection& GetChoices()
		{
			return choices_;
		}
	private:
		variable_collection variables_;
		variable_collection defaultVariables_;
		function_collection functions_;
		function_collection defaultFunctions_;
		reaction_collection reactions_;
		choice_collection choices_;
	};
}