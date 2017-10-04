#pragma once
#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include "stochsim_interfaces.h"
#include "expression.h"
namespace stochsim
{
	namespace cmdl
	{
		/// <summary>
		/// Terminal symbol. Either a string, a double, or a terminal symbol without value (e.g. the terminal symbols '+', '-', '+', '/', ',', '->', ...).
		/// </summary>
		class terminal_symbol
		{
		public:
			enum terminal_type
			{
				/// <summary>
				/// Type for terminal symbols being identifiers, i.e. strings.
				/// </summary>
				type_identifier,
				/// <summary>
				/// Type for numbers, i.e. doubles.
				/// </summary>
				type_number,
				/// <summary>
				/// Type for terminal symbols, like '+', '-', '+', '/', ',', '->', etc., holding no specific value.
				/// </summary>
				type_empty
			};

		public:
			operator expression::number() const
			{
				if (type_ != type_number)
					throw std::exception("Terminal symbol is not a number.");
				return numberValue_;
			}

			operator const expression::identifier() const
			{
				if (type_ != type_identifier)
					throw std::exception("Terminal symbol is not an identifier.");
				return identifierValue_;
			}

			terminal_symbol(expression::number numberValue) : numberValue_(std::move(numberValue)), type_(type_number)
			{
			}
			terminal_symbol(expression::identifier identifierValue) : identifierValue_(std::move(identifierValue)), type_(type_identifier)
			{
			}
			terminal_symbol() : type_(type_empty)
			{
			}
			terminal_type type()
			{
				return type_;
			}
		private:
			expression::number numberValue_;
			expression::identifier identifierValue_;
			terminal_type type_;
		};

		class reaction_component
		{
		public:
			reaction_component(Stochiometry stochiometry, bool modifier) : stochiometry_(stochiometry), modifier_(modifier)
			{
			}
			Stochiometry get_stochiometry() const noexcept
			{
				return stochiometry_;
			}
			Stochiometry& get_stochiometry() noexcept
			{
				return stochiometry_;
			}
			bool is_modifier() const noexcept
			{
				return modifier_;
			}
			bool& is_modifier() noexcept
			{
				return modifier_;
			}
		private:
			expression::identifier state_;
			Stochiometry stochiometry_;
			bool modifier_;
		};
		class reaction_definition
		{
		public:
			typedef std::unordered_map<expression::identifier, reaction_component> reaction_components;
		private:
			static reaction_components parse_components(std::unique_ptr<expression::sum_expression>& componentRaws, const expression::variable_lookup& lookup)
			{
				reaction_components components;
				for (auto& componentRaw : *componentRaws)
				{
					if (componentRaw.is_inverse())
					{
						throw std::exception("Reaction components must be separated by plus signs ('+'), and may or may not be proceeded by a factor determining their stochiometry, which is separated by a multiplication sign ('*'). This factor can either be a number, or an arbitrary formula which must, however, not include outer plus or minus signs. To include sums or differences in the calculation of the stochiometry, put the formula into brackets ('(' and ')').");
					}
					// A reaction component can either be a product, or an identifier. We simply try one after the other...
					auto identifierExpression = dynamic_cast<expression::variable_expression*>(componentRaw.get_expression());
					if (identifierExpression)
					{
						auto state = identifierExpression->get_identifier();
						//TODO: Handle modifiers.
						auto search = components.find(state);
						if (search != components.end())
						{
							auto& component = search->second;
							if (component.is_modifier())
							{
								std::stringstream errorMessage;
								errorMessage << "State " << state << " cannot participate in a reaction both as a modifier and a reactant, or both as a product and a transformee.";
								throw std::exception(errorMessage.str().c_str());
							}
							component.get_stochiometry()++;
						}
						else
							components.insert({ state, reaction_component(1, false) });
						continue;
					}

					auto product = dynamic_cast<expression::product_expression*>(componentRaw.get_expression());
					if (product)
					{

						// Get name and stochiometry.
						// All products must be non-empty, so we don't have to check...
						auto identifierElement = product->pop_back();
						if (identifierElement.is_inverse())
						{
							throw std::exception("Reaction components must be separated by plus signs ('+'), and may or may not be proceeded by a factor determining their stochiometry, which is separated by a multiplication sign ('*'). This factor can either be a number, or an arbitrary formula which must, however, not include outer plus or minus signs. To include sums or differences in the calculation of the stochiometry, put the formula into brackets ('(' and ')').");
						}
						auto identifierExpression = dynamic_cast<expression::variable_expression*>(identifierElement.get_expression());
						if (!identifierExpression)
						{
							throw std::exception("Reaction components must be separated by plus signs ('+'), and may or may not be proceeded by a factor determining their stochiometry, which is separated by a multiplication sign ('*'). This factor can either be a number, or an arbitrary formula which must, however, not include outer plus or minus signs. To include sums or differences in the calculation of the stochiometry, put the formula into brackets ('(' and ')').");
						}
						expression::identifier state = identifierExpression->get_identifier();
						Stochiometry stochiometry;
						try
						{
							auto number = product->eval(lookup);
							if (number + 0.5 < 0)
							{
								std::stringstream errorMessage;
								errorMessage << "Stochiometry of state \"" << state << "\" is negative.";
								throw std::exception(errorMessage.str().c_str());
							}
							stochiometry = static_cast<Stochiometry>(number + 0.5);
						}
						catch (const std::exception& e)
						{
							std::stringstream errorMessage;
							errorMessage << "Could not evaluate expression for stochiometry of state \"" << state << "\": " << e.what();
							throw std::exception(errorMessage.str().c_str());
						}
						if (stochiometry == 0)
							continue;
						auto search = components.find(state);
						if (search != components.end())
						{
							auto& component = search->second;
							if (component.is_modifier())
							{
								std::stringstream errorMessage;
								errorMessage << "State " << state << " cannot participate in a reaction both as a modifier and a reactant, or both as a product and a transformee.";
								throw std::exception(errorMessage.str().c_str());
							}
							component.get_stochiometry() += stochiometry;
						}
						else
							components.insert({ state, reaction_component(stochiometry, false) });
						continue;
					}
					//TODO: Handle modifiers.
					// if we are here, the element is invalid!
					throw std::exception("Reaction components must be separated by plus signs ('+'), and may or may not be proceeded by a factor determining their stochiometry, which is separated by a multiplication sign ('*'). This factor can either be a number, or an arbitrary formula which must, however, not include outer plus or minus signs. To include sums or differences in the calculation of the stochiometry, put the formula into brackets ('(' and ')').");
				}
				return std::move(components);
			}
			static void remove_zero_stochiometries(reaction_components& components)
			{
				for (auto it = components.begin(); it != components.end(); )
				{
					if (it->second.get_stochiometry() == 0)
						it = components.erase(it);
					else
						++it;
				}
			}
		public:
			reaction_definition(std::unique_ptr<expression::sum_expression> reactants, std::unique_ptr<expression::sum_expression> products, std::unique_ptr<expression::expression_base> rate, const expression::variable_lookup& lookup) :
				reactants_(parse_components(reactants, lookup)),
				products_(parse_components(products, lookup)),
				rate_(std::move(rate))
			{
				// transformees must be specified as LHS and RHS modifiers. Reduce stochiometry of LHS modifiers according to RHS modifiers stochiometry.
				for (auto& product : products_)
				{
					if (product.second.is_modifier())
					{
						auto search = reactants_.find(product.first);
						if (search == reactants_.end() || search->second.is_modifier() || search->second.get_stochiometry() < product.second.get_stochiometry())
						{
							std::stringstream errorMessage;
							errorMessage << "State " << product.first << " is specified as a transformee of the reactions. Transformees must appear on both sides of the reaction arrow ('->'), be marked on both sides by a opening and closing square brackets ('[]'), and have a stochiometry on the LHS at least as big as on the RHS.";
							throw std::exception(errorMessage.str().c_str());
						}
						search->second.get_stochiometry() -= product.second.get_stochiometry();
					}
				}
				// remove elements with zero stochiometry
				remove_zero_stochiometries(reactants_);
				remove_zero_stochiometries(products_);
			}
			reaction_components& get_reactants() noexcept
			{
				return reactants_;
			}
			const reaction_components& get_reactants() const noexcept
			{
				return reactants_;
			}

			reaction_components& get_products() noexcept
			{
				return products_;
			}
			const reaction_components& get_products() const noexcept
			{
				return products_;
			}
			const expression::expression_base* get_rate() const noexcept
			{
				return rate_.get();
			}
			expression::expression_base* get_rate() noexcept
			{
				return rate_.get();
			}
		private:
			reaction_components reactants_;
			reaction_components products_;
			std::unique_ptr<expression::expression_base> rate_;
		};

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
			parse_tree() :lookup_(std::bind(&parse_tree::variable_value, this, std::placeholders::_1))
			{
			}
			/// <summary>
			/// Creates a variable with the given name, and the expression as the value. This corresponds to variables following early evaluation.
			/// </summary>
			/// <param name="name">Name of variable.</param>
			/// <param name="expression">Expression of variable.</param>
			void create_variable(std::unique_ptr<terminal_symbol> name, std::unique_ptr<expression::expression_base> expression)
			{
				variables_[*name] = std::move(expression);
			}
			/// <summary>
			/// Creates a variable with the given name and value. This corresponds to variables following early evaluation.
			/// </summary>
			/// <param name="name">Name of variable</param>
			/// <param name="value">Value of variable</param>
			void create_variable(std::unique_ptr<terminal_symbol> name, expression::number value)
			{
				variables_[*name] = std::make_unique<expression::number_expression>(value);
			}
			void create_reaction(std::unique_ptr<expression::sum_expression> reactants, std::unique_ptr<expression::sum_expression> products, expression::number rateConstant)
			{
				create_reaction(std::move(reactants), std::move(products), std::make_unique<expression::number_expression>(rateConstant));
			}
			void create_reaction(std::unique_ptr<expression::sum_expression> reactants, std::unique_ptr<expression::sum_expression> products, std::unique_ptr<expression::expression_base> rate)
			{
				std::stringstream name;
				name << "reaction_" << (reactions_.size() + 1);
				create_reaction(std::make_unique<terminal_symbol>(name.str()), std::move(reactants), std::move(products), std::move(rate));
			}
			void create_reaction(std::unique_ptr<terminal_symbol> name, std::unique_ptr<expression::sum_expression> reactants, std::unique_ptr<expression::sum_expression> products, expression::number rateConstant)
			{
				create_reaction(std::move(name), std::move(reactants), std::move(products), std::make_unique<expression::number_expression>(rateConstant));
			}
			void create_reaction(std::unique_ptr<terminal_symbol> name, std::unique_ptr<expression::sum_expression> reactants, std::unique_ptr<expression::sum_expression> products, std::unique_ptr<expression::expression_base> rate)
			{
				reactions_[*name] = std::make_unique<reaction_definition>(std::move(reactants), std::move(products), std::move(rate), this->get_lookup());
			}
			/// <summary>
			/// Finds the variable with the given name, evaluates its expression and returns the result.
			/// If no variable with the given name exists, or if the expression could not be evaluated, throws a std::exception.
			/// </summary>
			/// <param name="name">Name of the variable to evaluate.</param>
			/// <returns>Value of the variable.</returns>
			expression::number variable_value(const expression::identifier& name) const
			{
				return variable_expression(name)->eval(lookup_);
			}
			static expression::expression_base* get_default_variable(const expression::identifier& name) noexcept
			{
				static std::map<expression::identifier, expression::number_expression> defaultVariables(
				{
					std::make_pair(expression::identifier("true"), expression::number_true),
					std::make_pair(expression::identifier("false"), expression::number_false),
					std::make_pair(expression::identifier("pi"), expression::number(3.141592653589793)),
					std::make_pair(expression::identifier("e"), expression::number(2.718281828459046))
				});
				auto search = defaultVariables.find(name);
				if (search != defaultVariables.end())
				{
					return &search->second;
				}
				else
					return nullptr;
			}
			/// <summary>
			/// Finds the variable with the given name and returns its expression.
			/// If no variable with the given name exists, throws a std::exception.
			/// </summary>
			/// <param name="name">Name of the variable to evaluate.</param>
			/// <returns>Expression of the variable.</returns>
			const expression::expression_base* variable_expression(const expression::identifier& name) const
			{
				auto search = variables_.find(name);
				if (search != variables_.end())
				{
					return search->second.get();
				}
				auto default_var = get_default_variable(name);
				if (default_var)
					return default_var;
				
				std::stringstream errorMessage;
				errorMessage << "Variable with name \"" << name << "\" not defined";
				throw std::exception(errorMessage.str().c_str());
			}
			/// <summary>
			/// Returns a function to lookup the value of a variable.
			/// Calling this->get_lookup()(variableName) is equivalent to calling
			/// this->variable_value(variableName).
			/// </summary>
			/// <returns>Function to lookup variable values.</returns>
			const expression::variable_lookup& get_lookup() const noexcept
			{
				return lookup_;
			}
		private:
			std::unordered_map<expression::identifier, std::unique_ptr<expression::expression_base>> variables_;
			std::unordered_map<expression::identifier, std::unique_ptr<reaction_definition>> reactions_;
			expression::variable_lookup lookup_;
		};

		/// <summary>
		/// The cmdl parser. We define the class members dynamically as part of the grammar definition,
		/// and only expose this c++ style class handling everything.
		/// </summary>
		class parser
		{
		public:
			/// <summary>
			/// Constructor.
			/// Throws an std::exception uppon errors.
			/// </summary>
			/// <param name="errorFileName">If not empty, the parse log is written to this file.</param>
			parser(std::string parseLogFilePath);
			~parser();
			/// <summary>
			/// Parses the token.
			/// Important: After calling this function, the token is owned (i.e. eventually destructed) by the parser.
			/// Throws an std::exception uppon errors.
			/// </summary>
			/// <param name="tokenID">ID/type of the token.</param>
			/// <param name="token">Value of the token</param>
			/// <param name="parseTree">Current parse tree.</param>
			void operator()(int tokenID, cmdl::terminal_symbol* token, cmdl::parse_tree& parseTree);

		private:
			void* handle_;
			FILE* traceFile_;
		};
	}
}