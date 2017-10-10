#pragma once
#include <random>
#include <unordered_map>
#include <memory>
#include <sstream>
#include "../expression/expression.h"
namespace cmdl
{
	class parse_tree
	{
	public:
		typedef std::unordered_map<expression::identifier, std::unique_ptr<expression::expression_base>> variable_collection;
		typedef std::unordered_map<expression::identifier, std::unique_ptr<expression::function_holder_base>> function_collection;
		typedef std::unordered_map<expression::identifier, std::unique_ptr<reaction_definition>> reaction_collection;
		typedef std::unordered_map<expression::identifier, std::unique_ptr<choice_definition>> choice_collection;
	public:
		parse_tree() : defaultVariables_(create_default_variables()), defaultFunctions_(create_default_functions())
		{
		}
		/// <summary>
		/// Creates a variable with the given name, and the expression as the value. This corresponds to variables following early evaluation.
		/// </summary>
		/// <param name="name">Name of variable.</param>
		/// <param name="expression">Expression of variable.</param>
		void create_variable(expression::identifier name, std::unique_ptr<expression::expression_base> expression)
		{
			variables_[name] = std::move(expression);
		}
		/// <summary>
		/// Creates a variable with the given name and value. This corresponds to variables following early evaluation.
		/// </summary>
		/// <param name="name">Name of variable</param>
		/// <param name="value">Value of variable</param>
		void create_variable(expression::identifier name, expression::number value)
		{
			variables_[name] = std::make_unique<expression::number_expression>(value);
		}
		void create_reaction(std::unique_ptr<reaction_side> reactants, std::unique_ptr<reaction_side> products, expression::number rateConstant)
		{
			create_reaction(std::move(reactants), std::move(products), std::make_unique<expression::number_expression>(rateConstant));
		}
		void create_reaction(std::unique_ptr<reaction_side> reactants, std::unique_ptr<reaction_side> products, std::unique_ptr<expression::expression_base> rate)
		{
			std::stringstream name;
			name << "reaction_" << (reactions_.size() + 1); 
			auto nameC = name.str();
			auto nameI = expression::identifier(nameC.begin(), nameC.end());
			create_reaction(nameI, std::move(reactants), std::move(products), std::move(rate));
		}
		void create_reaction(expression::identifier name, std::unique_ptr<reaction_side> reactants, std::unique_ptr<reaction_side> products, expression::number rateConstant)
		{
			create_reaction(std::move(name), std::move(reactants), std::move(products), std::make_unique<expression::number_expression>(rateConstant));
		}
		void create_reaction(expression::identifier name, std::unique_ptr<reaction_side> reactants, std::unique_ptr<reaction_side> products, std::unique_ptr<expression::expression_base> rate)
		{
			reactions_[name] = std::make_unique<reaction_definition>(std::move(reactants), std::move(products), std::move(rate));
		}
		expression::identifier create_choice(std::unique_ptr<expression::expression_base> condition, std::unique_ptr<reaction_side> componentsIfTrue, std::unique_ptr<reaction_side> componentsIfFalse)
		{
			std::stringstream name;
			name << "choice_" << (choices_.size() + 1);
			auto nameC = name.str();
			auto nameI = expression::identifier(nameC.begin(), nameC.end());
			choices_[nameI] = std::make_unique<choice_definition>(std::move(condition), std::move(componentsIfTrue), std::move(componentsIfFalse));
			return nameI;
		}

		/// <summary>
		/// Finds the variable with the given name and returns its expression.
		/// If no variable with the given name exists, throws a std::exception.
		/// </summary>
		/// <param name="name">Name of the variable to evaluate.</param>
		/// <returns>Expression of the variable.</returns>
		std::unique_ptr<expression::expression_base> get_variable_expression(const expression::identifier name) const
		{
			auto search = variables_.find(name);
			if (search != variables_.end())
			{
				return search->second->clone();
			}
			auto default_search = defaultVariables_.find(name);
			if (default_search != defaultVariables_.end())
				return default_search->second->clone();

			std::stringstream errorMessage;
			errorMessage << "Variable with name \"" << name << "\" not defined";
			throw std::exception(errorMessage.str().c_str());
		}

		std::unique_ptr<expression::function_holder_base> get_function_handler(const expression::identifier& name) const
		{
			auto search = functions_.find(name);
			if (search != functions_.end())
			{
				return search->second->clone();
			}
			auto default_search = defaultFunctions_.find(name);
			if (default_search != defaultFunctions_.end())
				return default_search->second->clone();

			std::stringstream errorMessage;
			errorMessage << "Function with name \"" << name << "\" not defined";
			throw std::exception(errorMessage.str().c_str());
		}

		/// <summary>
		/// Takes an expression and evaluates it. If the expression could not be evaluated, throws a std::exception.
		/// </summary>
		/// <param name="expression">Expression to evaluate.</param>
		/// <returns>Value of expression.</returns>
		expression::number get_expression_value(const expression::expression_base* expression) const
		{
			auto clone = expression->clone();
			auto bindings = get_binding_lookup();
			clone->bind(bindings);
			return clone->eval();
		}

		/// <summary>
		/// Finds the variable with the given name, evaluates its expression and returns the result.
		/// If no variable with the given name exists, or if the expression could not be evaluated, throws a std::exception.
		/// </summary>
		/// <param name="name">Name of the variable to evaluate.</param>
		/// <returns>Value of the variable.</returns>
		expression::number get_variable_value(const expression::identifier& name) const
		{
			auto search = variables_.find(name);
			if (search != variables_.end())
			{
				auto clone = search->second->clone();
				auto bindings = get_binding_lookup();
				clone->bind(bindings);
				return clone->eval();
			}
			auto default_search = defaultVariables_.find(name);
			if (default_search != defaultVariables_.end())
			{
				auto clone = default_search->second->clone();
				auto bindings = get_binding_lookup();
				clone->bind(bindings);
				return clone->eval();
			}

			std::stringstream errorMessage;
			errorMessage << "Variable with name \"" << name << "\" not defined";
			throw std::exception(errorMessage.str().c_str());
		}
		static std::unordered_map<expression::identifier, std::unique_ptr<expression::expression_base>> create_default_variables() noexcept
		{
			std::unordered_map<expression::identifier, std::unique_ptr<expression::expression_base>> defaultVariables;
			defaultVariables.emplace(expression::identifier("true"), std::unique_ptr<expression::expression_base>(new expression::number_expression(expression::number_true)));
			defaultVariables.emplace(expression::identifier("false"), std::unique_ptr<expression::expression_base>(new expression::number_expression(expression::number_false)));
			defaultVariables.emplace(expression::identifier("pi"), std::unique_ptr<expression::expression_base>(new expression::number_expression(expression::number(3.141592653589793))));
			defaultVariables.emplace(expression::identifier("e"), std::unique_ptr<expression::expression_base>(new expression::number_expression(expression::number(2.718281828459046))));
			return std::move(defaultVariables);
		}

		static std::unordered_map<expression::identifier, std::unique_ptr<expression::function_holder_base>> create_default_functions() noexcept
		{
			std::unordered_map<expression::identifier, std::unique_ptr<expression::function_holder_base>> defaultFunctions;
			
			defaultFunctions.emplace("min", expression::make_function_holder(
				(expression::number(*)(expression::number, expression::number))(&fmin), false));
			defaultFunctions.emplace("max", expression::make_function_holder(
				(expression::number(*)(expression::number, expression::number))(&fmax), false));
			defaultFunctions.emplace("mod", expression::make_function_holder(
				(expression::number(*)(expression::number, expression::number))(&fmod), false));
			
			defaultFunctions.emplace("sin", expression::make_function_holder(
				(expression::number (*)(expression::number))(&sin), false));
			defaultFunctions.emplace("cos", expression::make_function_holder(
				(expression::number(*)(expression::number))&cos, false));
			defaultFunctions.emplace("tan", expression::make_function_holder(
				(expression::number(*)(expression::number))&tan, false));

			defaultFunctions.emplace("asin", expression::make_function_holder(
				(expression::number(*)(expression::number))(&asin), false));
			defaultFunctions.emplace("acos", expression::make_function_holder(
				(expression::number(*)(expression::number))&acos, false));
			defaultFunctions.emplace("atan", expression::make_function_holder(
				(expression::number(*)(expression::number))&atan, false));

			defaultFunctions.emplace("sinh", expression::make_function_holder(
				(expression::number(*)(expression::number))(&sinh), false));
			defaultFunctions.emplace("cosh", expression::make_function_holder(
				(expression::number(*)(expression::number))&cosh, false));
			defaultFunctions.emplace("tanh", expression::make_function_holder(
				(expression::number(*)(expression::number))&tanh, false));

			defaultFunctions.emplace("asinh", expression::make_function_holder(
				(expression::number(*)(expression::number))(&asinh), false));
			defaultFunctions.emplace("acosh", expression::make_function_holder(
				(expression::number(*)(expression::number))&acosh, false));
			defaultFunctions.emplace("atanh", expression::make_function_holder(
				(expression::number(*)(expression::number))&atanh, false));

			defaultFunctions.emplace("abs", expression::make_function_holder(
				(expression::number(*)(expression::number))&abs, false));
			
			defaultFunctions.emplace("ceil", expression::make_function_holder(
				(expression::number(*)(expression::number))&ceil, false));
			defaultFunctions.emplace("floor", expression::make_function_holder(
				(expression::number(*)(expression::number))&floor, false));
			defaultFunctions.emplace("round", expression::make_function_holder(
				(expression::number(*)(expression::number))&round, false));
			
			defaultFunctions.emplace("erf", expression::make_function_holder(
				(expression::number(*)(expression::number))&erf, false));
			defaultFunctions.emplace("exp", expression::make_function_holder(
				(expression::number(*)(expression::number))&exp, false));
			defaultFunctions.emplace("exp2", expression::make_function_holder(
				(expression::number(*)(expression::number))&exp2, false));
			defaultFunctions.emplace("log", expression::make_function_holder(
				(expression::number(*)(expression::number))&log, false));
			defaultFunctions.emplace("log10", expression::make_function_holder(
				(expression::number(*)(expression::number))&log10, false));
			defaultFunctions.emplace("log2", expression::make_function_holder(
				(expression::number(*)(expression::number))&log2, false));
			
			defaultFunctions.emplace("pow", expression::make_function_holder(
				(expression::number(*)(expression::number, expression::number))&pow, false));
			defaultFunctions.emplace("sqrt", expression::make_function_holder(
				(expression::number(*)(expression::number))&sqrt, false));

			defaultFunctions.emplace("rand", expression::make_function_holder(
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
		expression::binding_lookup get_binding_lookup() const noexcept
		{
			return [this](const expression::identifier name)->std::unique_ptr<expression::function_holder_base>
			{
				if (name[name.size() - 1] == ')' && name[name.size() - 2] == '(')
				{
					return this->get_function_handler(name.substr(0, name.size()-2));
				}
				else
				{
					auto value = this->get_variable_value(name);
					std::function<expression::number()> binding = [value]()->expression::number {return value; };
					return expression::make_function_holder(binding, false);
				}
			};
		}
	public:
		const variable_collection& get_variables()
		{
			return variables_;
		}
		const function_collection& get_functions()
		{
			return functions_;
		}
		const reaction_collection& get_reactions()
		{
			return reactions_;
		}
		const choice_collection& get_choices()
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