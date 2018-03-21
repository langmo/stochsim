#pragma once
#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include "stochsim_common.h"
#include "expression_common.h"

namespace cmdlparser
{
	/// <summary>
	/// Terminal symbol. Either a string, a double, or a terminal symbol without value (e.g. the terminal symbols '+', '-', '+', '/', ',', '->', ...).
	/// </summary>
	class TerminalSymbol
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

		TerminalSymbol(expression::number numberValue) : numberValue_(std::move(numberValue)), type_(type_number)
		{
		}
		TerminalSymbol(expression::identifier identifierValue) : identifierValue_(std::move(identifierValue)), type_(type_identifier)
		{
		}
		TerminalSymbol() : type_(type_empty)
		{
		}
		terminal_type GetType()
		{
			return type_;
		}
	private:
		expression::number numberValue_;
		expression::identifier identifierValue_;
		terminal_type type_;
	};

	typedef std::vector<expression::identifier> MoleculePropertyNames;
	typedef std::vector<std::unique_ptr<expression::IExpression>> MoleculePropertyExpressions;

	class ReactionLeftComponent
	{
	public:
		ReactionLeftComponent(expression::identifier state, expression::number stochiometry, bool modifier, std::unique_ptr<MoleculePropertyNames> propertyNames = nullptr) : state_(std::move(state)), modifier_(modifier)
		{
			if (stochiometry + 0.5 < 0)
			{
				std::stringstream errorMessage;
				errorMessage << "State " << state_ << " has a negative stochiometry.";
				throw std::exception(errorMessage.str().c_str());
			}
			stochiometry_ = static_cast<stochsim::Stochiometry>(stochiometry + 0.5);
			if (propertyNames)
			{
				if (propertyNames->size() > stochsim::Molecule::size_)
				{
					std::stringstream errorMessage;
					errorMessage << "Number of properties for State " << state_ << " too high (found "<< propertyNames->size()<<", maximally allowed "<<stochsim::Molecule::size_ <<").";
					throw std::exception(errorMessage.str().c_str());
				}
				for (int i = 0; i < propertyNames->size(); i++)
				{
					propertyNames_[i] = std::move((*propertyNames)[i]);
				}
			}
		}
		stochsim::Stochiometry GetStochiometry() const noexcept
		{
			return stochiometry_;
		}
		void SetStochiometry(stochsim::Stochiometry stochiometry) noexcept
		{
			stochiometry_ = stochiometry;
		}
		bool IsModifier() const noexcept
		{
			return modifier_;
		}
		expression::identifier GetState() const noexcept
		{
			return state_;
		}
		std::array<expression::identifier, stochsim::Molecule::size_>& GetPropertyNames()
		{
			return propertyNames_;
		}
	private:
		expression::identifier state_;
		stochsim::Stochiometry stochiometry_;
		bool modifier_;
		std::array<expression::identifier, stochsim::Molecule::size_> propertyNames_;
	};
	class ReactionRightComponent
	{
	public:
		ReactionRightComponent(expression::identifier state, expression::number stochiometry, bool modifier, std::unique_ptr<MoleculePropertyExpressions> propertyExpressions = nullptr) : state_(std::move(state)), modifier_(modifier)
		{
			if (stochiometry + 0.5 < 0)
			{
				std::stringstream errorMessage;
				errorMessage << "State " << state_ << " has a negative stochiometry.";
				throw std::exception(errorMessage.str().c_str());
			}
			stochiometry_ = static_cast<stochsim::Stochiometry>(stochiometry + 0.5);
			if (propertyExpressions)
			{
				if (propertyExpressions->size() > stochsim::Molecule::size_)
				{
					std::stringstream errorMessage;
					errorMessage << "Number of properties for State " << state_ << " too high (found " << propertyExpressions->size() << ", maximally allowed " << stochsim::Molecule::size_ << ").";
					throw std::exception(errorMessage.str().c_str());
				}
				for (int i = 0; i < propertyExpressions->size(); i++)
				{
					propertyExpressions_[i] = std::move((*propertyExpressions)[i]);
				}
			}
		}
		stochsim::Stochiometry GetStochiometry() const noexcept
		{
			return stochiometry_;
		}
		void SetStochiometry(stochsim::Stochiometry stochiometry) noexcept
		{
			stochiometry_ = stochiometry;
		}
		bool IsModifier() const noexcept
		{
			return modifier_;
		}
		expression::identifier GetState() const noexcept
		{
			return state_;
		}
		std::array<std::unique_ptr<expression::IExpression>, stochsim::Molecule::size_>& GetPropertyExpressions()
		{
			return propertyExpressions_;
		}
		std::array<expression::identifier, stochsim::Molecule::size_>& GetPropertyNames()
		{
			return propertyNames_;
		}
		void SetPropertyNames(const std::array<expression::identifier, stochsim::Molecule::size_>& propertyNames)
		{
			propertyNames_ = propertyNames;
		}
	private:
		expression::identifier state_;
		stochsim::Stochiometry stochiometry_;
		bool modifier_;
		std::array<std::unique_ptr<expression::IExpression>, stochsim::Molecule::size_> propertyExpressions_;
		std::array<expression::identifier, stochsim::Molecule::size_> propertyNames_;
	};

	class ReactionLeftSide
	{
	public:
		typedef std::unordered_map<expression::identifier, std::unique_ptr<ReactionLeftComponent>> collection_type;
		typedef collection_type::value_type value_type;
		typedef collection_type::key_type key_type;
		typedef collection_type::size_type size_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef collection_type::iterator iterator;
		typedef collection_type::const_iterator const_iterator;

		bool IsEmpty() const noexcept
		{
			return components_.empty();
		}
		size_type GetNumComponents() const noexcept
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
		iterator FindComponent(const key_type& k)
		{
			return components_.find(k);
		}
		const_iterator FindComponent(const key_type& k) const
		{
			return components_.find(k);
		}
	public:
		ReactionLeftSide()
		{
		}
		void PushBack(std::unique_ptr<ReactionLeftComponent> component)
		{
			auto search = components_.find(component->GetState());
			if (search == components_.end())
			{
				components_.emplace(component->GetState(), std::move(component));
				return;
			}
			auto& existingComponent = search->second;
			if (existingComponent->IsModifier() != component->IsModifier())
			{
				std::stringstream errorMessage;
				errorMessage << "State " << component->GetState() << " cannot participate in reaction both as a modifier and a reactant.";
				throw std::exception(errorMessage.str().c_str());
			}
			if (existingComponent->GetPropertyNames() != component->GetPropertyNames())
			{
				std::stringstream errorMessage;
				errorMessage << "Property names of reactant/modifier " << component->GetState() << " cannot change.";
				throw std::exception(errorMessage.str().c_str());
			}

			existingComponent->SetStochiometry(existingComponent->GetStochiometry() + component->GetStochiometry());
		}
		void RemoveComponentsWithZeroStochiometry()
		{
			for (auto it = components_.begin(); it != components_.end(); /* no increment */)
			{
				if (it->second->GetStochiometry() == 0)
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

	class ReactionRightSide
	{
	public:
		typedef std::unordered_map<expression::identifier, std::unique_ptr<ReactionRightComponent>> collection_type;
		typedef collection_type::value_type value_type;
		typedef collection_type::key_type key_type;
		typedef collection_type::size_type size_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef collection_type::iterator iterator;
		typedef collection_type::const_iterator const_iterator;

		bool IsEmpty() const noexcept
		{
			return components_.empty();
		}
		size_type GetNumComponents() const noexcept
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
		iterator FindComponent(const key_type& k)
		{
			return components_.find(k);
		}
		const_iterator FindComponent(const key_type& k) const
		{
			return components_.find(k);
		}
	public:
		ReactionRightSide()
		{
		}
		void PushBack(std::unique_ptr<ReactionRightComponent> component)
		{
			auto search = components_.find(component->GetState());
			if (search == components_.end())
			{
				components_.emplace(component->GetState(), std::move(component));
				return;
			}
			auto& existingComponent = search->second;
			if (existingComponent->IsModifier() != component->IsModifier())
			{
				std::stringstream errorMessage;
				errorMessage << "State " << component->GetState() << " cannot participate in reaction both as a product and a transformee.";
				throw std::exception(errorMessage.str().c_str());
			}
			auto& existingExpressions = existingComponent->GetPropertyExpressions();
			auto& expressions = component->GetPropertyExpressions();
			for (size_t i = 0; i < expressions.size(); i++)
			{
				if(existingExpressions[i].operator bool() != expressions[i].operator bool()
					|| (existingExpressions[i].operator bool() && existingExpressions[i]->ToCmdl() != expressions[i]->ToCmdl()))
				{
					std::stringstream errorMessage;
					errorMessage << "Property " << std::to_string(i) << " of product/transformee " << component->GetState() << " cannot change.";
					throw std::exception(errorMessage.str().c_str());
				}
			}
			existingComponent->SetStochiometry(existingComponent->GetStochiometry() + component->GetStochiometry());
		}
		void RemoveComponentsWithZeroStochiometry()
		{
			for (auto it = components_.begin(); it != components_.end(); /* no increment */)
			{
				if (it->second->GetStochiometry() == 0)
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

	typedef std::vector<std::unique_ptr<expression::IExpression>> FunctionArguments;

	class ReactionSpecifier
	{
	public:
		static constexpr char rate_type[] = "rate";
		static constexpr char delay_type[] = "delay";
		ReactionSpecifier(expression::identifier type, std::unique_ptr<expression::IExpression> value) : type_(std::move(type)), value_(std::move(value))
		{
		}
		expression::identifier GetType() const noexcept
		{
			return type_;
		}
		const expression::IExpression* GetValue() const noexcept
		{
			return value_.get();
		}
	private:
		expression::identifier type_;
		std::unique_ptr<expression::IExpression> value_;
	};

	class ReactionSpecifiers
	{
	public:
		ReactionSpecifiers() :specifiers_(10)
		{
		} 
		void PushBack(std::unique_ptr<ReactionSpecifier> specifier)
		{
			auto search = specifiers_.find(specifier->GetType());
			if (search != specifiers_.end())
			{
				std::stringstream errorMessage;
				errorMessage << "Reaction specifier \""<< search->first <<"\" already defined.";
				throw std::exception(errorMessage.str().c_str());
			}
			specifiers_.emplace(specifier->GetType(), std::move(specifier));
		}
		bool HasType(expression::identifier type) const noexcept
		{
			return specifiers_.find(type) != specifiers_.end();
		}
		bool HasRate() const noexcept
		{
			return HasType(ReactionSpecifier::rate_type);
		}
		bool HasDelay() const noexcept
		{
			return HasType(ReactionSpecifier::delay_type);
		}
		const expression::IExpression* GetType(expression::identifier type) const noexcept
		{
			auto search = specifiers_.find(type);
			if (search == specifiers_.end())
				return nullptr;
			else
				return search->second->GetValue();
		}
		const expression::IExpression* GetRate() const noexcept
		{
			return GetType(ReactionSpecifier::rate_type);
		}
		const expression::IExpression* GetDelay() const noexcept
		{
			return GetType(ReactionSpecifier::delay_type);
		}
	private:
		std::unordered_map<expression::identifier, std::unique_ptr<ReactionSpecifier> > specifiers_;
		
	};
	
	class ChoiceDefinition
	{
	public:
		ChoiceDefinition(std::unique_ptr<expression::IExpression> condition, std::unique_ptr<ReactionRightSide> componentsIfTrue, std::unique_ptr<ReactionRightSide> componentsIfFalse) :
			componentsIfTrue_(std::move(componentsIfTrue)),
			componentsIfFalse_(std::move(componentsIfFalse)),
			condition_(std::move(condition))
		{
			componentsIfTrue_->RemoveComponentsWithZeroStochiometry();
			componentsIfFalse_->RemoveComponentsWithZeroStochiometry();
			// don't allow modifiers
			for (auto& component : *componentsIfTrue_)
			{
				if (component.second->IsModifier())
				{
					std::stringstream errorMessage;
					errorMessage << "Modifiers are not allowed for conditional reaction components. State " << component.first << " is defined as a modifier.";
					throw std::exception(errorMessage.str().c_str());
				}
			}
			for (auto& component : *componentsIfFalse_)
			{
				if (component.second->IsModifier())
				{
					std::stringstream errorMessage;
					errorMessage << "Modifiers are not allowed for conditional reaction components. State " << component.first << " is defined as a modifier.";
					throw std::exception(errorMessage.str().c_str());
				}
			}
		}
		const ReactionRightSide* GetComponentsIfTrue() const noexcept
		{
			return componentsIfTrue_.get();
		}

		const ReactionRightSide* GetComponentsIfFalse() const noexcept
		{
			return componentsIfFalse_.get();
		}

		const expression::IExpression* GetCondition() const noexcept
		{
			return condition_.get();
		}
	private:
		std::unique_ptr<ReactionRightSide> componentsIfTrue_;
		std::unique_ptr<ReactionRightSide> componentsIfFalse_;
		std::unique_ptr<expression::IExpression> condition_;
	};

	class ReactionDefinition
	{
	public:
		ReactionDefinition(std::unique_ptr<ReactionLeftSide> reactants, std::unique_ptr<ReactionRightSide> products, std::unique_ptr<ReactionSpecifiers> specifiers) :
			reactants_(std::move(reactants)),
			products_(std::move(products)),
			specifiers_(std::move(specifiers))
		{
			// transformees must be specified as LHS and RHS modifiers. Reduce stochiometry of LHS modifiers according to RHS modifiers stochiometry.
			for (auto& product : *products_)
			{
				if (product.second->IsModifier())
				{
					auto search = reactants_->FindComponent(product.first);
					if (search == reactants_->end() || !search->second->IsModifier() || search->second->GetStochiometry() < product.second->GetStochiometry())
					{
						std::stringstream errorMessage;
						errorMessage << "State " << product.first << " is specified as a transformee of the reactions. Transformees must appear on both sides of the reaction arrow ('->'), be marked on both sides by a opening and closing square brackets ('[]'), and have a stochiometry on the LHS at least as big as on the RHS.";
						throw std::exception(errorMessage.str().c_str());
					}
					search->second->SetStochiometry(search->second->GetStochiometry() - product.second->GetStochiometry());
					product.second->SetPropertyNames(search->second->GetPropertyNames());
				}
			}
			products_->RemoveComponentsWithZeroStochiometry();
			reactants_->RemoveComponentsWithZeroStochiometry();
		}
		const ReactionLeftSide* GetReactants() const noexcept
		{ 
			return reactants_.get();
		}

		const ReactionRightSide* GetProducts() const noexcept
		{
			return products_.get();
		}

		const ReactionSpecifiers* GetSpecifiers() const noexcept
		{
			return specifiers_.get();
		}
	private:
		std::unique_ptr<ReactionLeftSide> reactants_;
		std::unique_ptr<ReactionRightSide> products_;
		std::unique_ptr<ReactionSpecifiers> specifiers_;
	};

	
}
