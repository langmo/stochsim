#pragma once
#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include "../stochsim.h"
#include "../expression/expression.h"
#include "../expression/sum_expression.h"
#include "../expression/variable_expression.h"
#include "../expression/product_expression.h"

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
		reaction_component(expression::identifier state, expression::number stochiometry, bool modifier) : state_(std::move(state)), modifier_(modifier)
		{
			if (stochiometry + 0.5 < 0)
			{
				std::stringstream errorMessage;
				errorMessage << "State " << state_ << " has a negative stochiometry.";
				throw std::exception(errorMessage.str().c_str());
			}
			stochiometry_ = static_cast<stochsim::Stochiometry>(stochiometry + 0.5);
		}
		stochsim::Stochiometry get_stochiometry() const noexcept
		{
			return stochiometry_;
		}
		void set_stochiometry(stochsim::Stochiometry stochiometry) noexcept
		{
			stochiometry_ = stochiometry;
		}
		bool is_modifier() const noexcept
		{
			return modifier_;
		}
		expression::identifier get_state() const noexcept
		{
			return state_;
		}
	private:
		expression::identifier state_;
		stochsim::Stochiometry stochiometry_;
		bool modifier_;
	};

	class reaction_side
	{
	public:
		typedef std::unordered_map<expression::identifier, std::unique_ptr<reaction_component>> collection_type;
		typedef collection_type::value_type value_type;
		typedef collection_type::key_type key_type;
		typedef collection_type::size_type size_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef collection_type::iterator iterator;
		typedef collection_type::const_iterator const_iterator;

		bool empty() const noexcept
		{
			return components_.empty();
		}
		size_type size() const noexcept
		{
			return components_.size();
		}
		iterator begin() noexcept
		{
			return components_.begin();
		}
		const_iterator begin() const noexcept
		{
			return components_.begin();
		}
		const_iterator cbegin() const noexcept
		{
			return components_.cbegin();
		}
		iterator end() noexcept
		{
			return components_.end();
		}
		const_iterator end() const noexcept
		{
			return components_.end();
		}
		const_iterator cend() const noexcept
		{
			return components_.cend();
		}
		iterator find(const key_type& k)
		{
			return components_.find(k);
		}
		const_iterator find(const key_type& k) const
		{
			return components_.find(k);
		}
	public:
		reaction_side()
		{
		}
		void push_back(std::unique_ptr<reaction_component> component)
		{
			auto search = components_.find(component->get_state());
			if (search == components_.end())
			{
				components_.emplace(component->get_state(), std::move(component));
				return;
			}
			auto& existingComponent = search->second;
			if (existingComponent->is_modifier() != component->is_modifier())
			{
				std::stringstream errorMessage;
				errorMessage << "State " << component->get_state() << " cannot participate in a reaction both as a modifier and a reactant, or both as a product and a transformee.";
				throw std::exception(errorMessage.str().c_str());
			}
			existingComponent->set_stochiometry(existingComponent->get_stochiometry() + component->get_stochiometry());
		}
		void remove_zero_components()
		{
			for (auto it = components_.begin(); it != components_.end(); /* no increment */)
			{
				if (it->second->get_stochiometry() == 0)
				{
					it = components_.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
	private:
		collection_type components_;
	};

	class reaction_definition
	{
	public:
		reaction_definition(std::unique_ptr<reaction_side> reactants, std::unique_ptr<reaction_side> products, std::unique_ptr<expression::expression_base> rate) :
			reactants_(std::move(reactants)),
			products_(std::move(products)),
			rate_(std::move(rate))
		{
			// transformees must be specified as LHS and RHS modifiers. Reduce stochiometry of LHS modifiers according to RHS modifiers stochiometry.
			for (auto& product : *products_)
			{
				if (product.second->is_modifier())
				{
					auto search = reactants_->find(product.first);
					if (search == reactants_->end() || !search->second->is_modifier() || search->second->get_stochiometry() < product.second->get_stochiometry())
					{
						std::stringstream errorMessage;
						errorMessage << "State " << product.first << " is specified as a transformee of the reactions. Transformees must appear on both sides of the reaction arrow ('->'), be marked on both sides by a opening and closing square brackets ('[]'), and have a stochiometry on the LHS at least as big as on the RHS.";
						throw std::exception(errorMessage.str().c_str());
					}
					search->second->set_stochiometry(search->second->get_stochiometry() - product.second->get_stochiometry());
				}
			}
			products_->remove_zero_components();
			reactants_->remove_zero_components();
		}
		const reaction_side* get_reactants() const noexcept
		{
			return reactants_.get();
		}

		const reaction_side* get_products() const noexcept
		{
			return products_.get();
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
		std::unique_ptr<reaction_side> reactants_;
		std::unique_ptr<reaction_side> products_;
		std::unique_ptr<expression::expression_base> rate_;
	};

	
}
