#pragma once
namespace cmdl
{
	class parse_tree
	{
	public:
		typedef std::unordered_map<expression::identifier, std::unique_ptr<reaction_definition>>::value_type value_type;
		typedef std::unordered_map<expression::identifier, std::unique_ptr<reaction_definition>>::size_type size_type;
		typedef std::unordered_map<expression::identifier, std::unique_ptr<reaction_definition>>::iterator iterator;
		typedef std::unordered_map<expression::identifier, std::unique_ptr<reaction_definition>>::const_iterator const_iterator;
		iterator begin() noexcept
		{
			return reactions_.begin();
		}
		const_iterator begin() const noexcept
		{
			return reactions_.begin();
		}
		const_iterator cbegin() const noexcept
		{
			return reactions_.cbegin();
		}
		iterator end() noexcept
		{
			return reactions_.end();
		}
		const_iterator end() const noexcept
		{
			return reactions_.end();
		}
		const_iterator cend() const noexcept
		{
			return reactions_.cend();
		}
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

		const expression::function_holder_base* get_function_handler(const expression::identifier& name) const
		{
			auto search = functions_.find(name);
			if (search != functions_.end())
			{
				return search->second.get();
			}
			auto default_search = defaultFunctions_.find(name);
			if (default_search != defaultFunctions_.end())
				return default_search->second.get();

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
			defaultFunctions.emplace("max", expression::make_function_holder(
				static_cast<std::function<expression::number(expression::number, expression::number)>>(
					[](expression::number n1, const expression::number n2) -> expression::number
			{
				return n1 > n2 ? n1 : n2;
			}
			), false));
			return std::move(defaultFunctions);
		}
	private:
		/// <summary>
		/// Returns a binding for all defined variable.
		/// </summary>
		/// <returns>Function to lookup variable values.</returns>
		const expression::binding_lookup get_binding_lookup() const noexcept
		{
			return [this](const expression::identifier name)->std::unique_ptr<expression::function_holder_base>
			{
				std::function<expression::number()> binding = [this, name]()->expression::number {return this->get_variable_value(name); };
				return expression::make_function_holder(binding, false);
			};
		}

	private:
		std::unordered_map<expression::identifier, std::unique_ptr<expression::expression_base>> variables_;
		std::unordered_map<expression::identifier, std::unique_ptr<expression::expression_base>> defaultVariables_;
		std::unordered_map<expression::identifier, std::unique_ptr<expression::function_holder_base>> functions_;
		std::unordered_map<expression::identifier, std::unique_ptr<expression::function_holder_base>> defaultFunctions_;
		std::unordered_map<expression::identifier, std::unique_ptr<reaction_definition>> reactions_;
	};
}