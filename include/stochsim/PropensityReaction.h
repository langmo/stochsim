#pragma once
#include "stochsim_common.h"
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <sstream>
#include <algorithm>
#include "ExpressionHolder.h"
#include "expression_common.h"
#include "ExpressionParser.h"
namespace stochsim
{
	/// <summary>
	/// Simple and most typical type of a reaction. The propensity of the reaction is proportional to its rate, and the product of the absolut reactant numbers (taking their stochiometry into account).
	/// When the reaction fires, for most reactant/products the absolute numbers are increased/decreased accoding to their stochiometries.
	/// However, when a reactant is flagged (its modifier is true), its concentration is not decreased when the reaction fires, which allows to implement e.g. enzymes catalyzing a reaction.
	/// In contrary, when a product is flagged (its modifier is true), its concentration is also not increased when the reaction is fired, but instead the modify function is called on the respective state.
	/// </summary>
	class PropensityReaction :
		public IPropensityReaction
	{
	private:
		class Reactant
		{
		public:
			Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			const Molecule::PropertyNames propertyNames_;
			Reactant(std::shared_ptr<IState> state, Stochiometry stochiometry, Molecule::PropertyNames propertyNames) noexcept : stochiometry_(stochiometry), state_(std::move(state)), propertyNames_(std::move(propertyNames))
			{
			}
			inline void Initialize(ISimInfo& simInfo)
			{
				// do nothing.
			}
			inline void Uninitialize(ISimInfo& simInfo)
			{
				// do nothing.
			}
		};
		class Modifier
		{
		public:
			Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			const Molecule::PropertyNames propertyNames_;
			Modifier(std::shared_ptr<IState> state, Stochiometry stochiometry, Molecule::PropertyNames propertyNames) noexcept : stochiometry_(stochiometry), state_(std::move(state)), propertyNames_(std::move(propertyNames))
			{
			}
			inline void Initialize(ISimInfo& simInfo)
			{
				// do nothing.
			}
			inline void Uninitialize(ISimInfo& simInfo)
			{
				// do nothing.
			}
		};
		class Product
		{
		public:
			Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			std::array<ExpressionHolder, Molecule::size_> propertyExpressions_;
			Product(std::shared_ptr<IState> state, Stochiometry stochiometry, Molecule::PropertyExpressions propertyExpressions) noexcept : stochiometry_(stochiometry), state_(std::move(state))
			{
				for (size_t i = 0; i < Molecule::size_; i++)
				{
					propertyExpressions_[i].SetExpression(std::move(propertyExpressions[i]));
				}
			}
			inline void Initialize(ISimInfo& simInfo)
			{
				for (auto& propertyExpression : propertyExpressions_)
				{
					if (propertyExpression)
						propertyExpression.Initialize(simInfo);
				}
			}
			inline void Uninitialize(ISimInfo& simInfo)
			{
				for (auto& propertyExpression : propertyExpressions_)
				{
					if (propertyExpression)
						propertyExpression.Uninitialize(simInfo);
				}
			}
			inline Molecule operator() (ISimInfo& simInfo, const std::vector<Variable>& variables = {}) const
			{
				Molecule molecule;
				for (size_t i = 0; i < Molecule::size_; i++)
				{
					if (propertyExpressions_[i])
						molecule[i] = propertyExpressions_[i](simInfo, variables);
				}
				return molecule;
			}
		};
		class Transformee
		{
		public:
			Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			std::array<ExpressionHolder, Molecule::size_> propertyExpressions_;
			const Molecule::PropertyNames propertyNames_;
			Transformee(std::shared_ptr<IState> state, Stochiometry stochiometry, Molecule::PropertyExpressions propertyExpressions, Molecule::PropertyNames propertyNames) noexcept : stochiometry_(stochiometry), state_(std::move(state)), propertyNames_(std::move(propertyNames))
			{
				for (size_t i = 0; i < Molecule::size_; i++)
				{
					propertyExpressions_[i].SetExpression(std::move(propertyExpressions[i]));
				}
			}
			inline void Initialize(ISimInfo& simInfo)
			{
				for (auto& propertyExpression : propertyExpressions_)
				{
					if (propertyExpression)
						propertyExpression.Initialize(simInfo);
				}
			}
			inline void Uninitialize(ISimInfo& simInfo)
			{
				for (auto& propertyExpression : propertyExpressions_)
				{
					if (propertyExpression)
						propertyExpression.Uninitialize(simInfo);
				}
			}
			inline Molecule& operator() (Molecule& molecule, ISimInfo& simInfo, const std::vector<Variable>& variables = {}) const
			{
				for (size_t i = 0; i < Molecule::size_; i++)
				{
					if (propertyExpressions_[i])
						molecule[i] = propertyExpressions_[i](simInfo, variables);
				}
				return molecule;
			}
		};
	public:
		PropensityReaction(std::string name, double rateConstant) noexcept : name_(std::move(name)), rateConstant_(rateConstant)
		{
		}
		PropensityReaction(std::string name, std::unique_ptr<expression::IExpression> rateEquation) : name_(std::move(name)), rateConstant_(0)
		{
			SetRateEquation(std::move(rateEquation));
		}
		PropensityReaction(std::string name, std::string rateEquation) : name_(std::move(name)), rateConstant_(0)
		{
			SetRateEquation(std::move(rateEquation));
		}
		/// <summary>
		/// Returns all reactants of the reaction. Modifiers and Transformees are not considered to be reactants.
		/// </summary>
		/// <returns>Reactants of reaction.</returns>
		stochsim::Collection<stochsim::ReactionLeftElement> GetReactants() const noexcept
		{
			stochsim::Collection<stochsim::ReactionLeftElement> returnVal;
			for (auto& reactant : reactants_)
			{
				Molecule::PropertyNames names;
				for (size_t i = 0; i < reactant.propertyNames_.size(); i++)
				{
					names[i] = reactant.propertyNames_[i];
				}
				returnVal.emplace_back(reactant.state_, reactant.stochiometry_, std::move(names));
			}
			return std::move(returnVal);
		}
		/// <summary>
		/// Returns all products of the reaction. Modifiers and Transformees are not considered to be products.
		/// </summary>
		/// <returns>Products of the reaction.</returns>
		stochsim::Collection<stochsim::ReactionRightElement> GetProducts() const noexcept
		{
			stochsim::Collection<stochsim::ReactionRightElement> returnVal;
			for (auto& product : products_)
			{
				Molecule::PropertyExpressions expressions;
				for (size_t i = 0; i < product.propertyExpressions_.size(); i++)
				{
					expressions[i] = product.propertyExpressions_[i] ? product.propertyExpressions_[i].GetExpression()->Clone() : nullptr;
				}
				returnVal.emplace_back(product.state_, product.stochiometry_, std::move(expressions));
			}
			return std::move(returnVal);
		}
		/// <summary>
		/// Returns all transformees of the reaction.
		/// Transformees are similar to modifiers of the reaction in that they modify the reaction rate according to their stochiometry, and in that their concentration is neither increased nor decreased
		/// when the reaction fires. However, a transformee is transformed (IState.transform() is called) according to the stochiometry. What a transformation of a state exactly implies is implementation
		/// dependent.
		/// </summary>
		/// <returns>Transformees of the reaction.</returns>
		stochsim::Collection<stochsim::ReactionLeftRightElement> GetTransformees() const noexcept
		{
			stochsim::Collection<stochsim::ReactionLeftRightElement> returnVal;
			for (auto& transformee : transformees_)
			{
				Molecule::PropertyNames names;
				for (size_t i = 0; i < transformee.propertyNames_.size(); i++)
				{
					names[i] = transformee.propertyNames_[i];
				}
				Molecule::PropertyExpressions expressions;
				for (size_t i = 0; i < transformee.propertyExpressions_.size(); i++)
				{
					expressions[i] = transformee.propertyExpressions_[i] ? transformee.propertyExpressions_[i].GetExpression()->Clone() : nullptr;
				}
				returnVal.emplace_back(transformee.state_, transformee.stochiometry_, std::move(expressions), std::move(names));
			}
			return std::move(returnVal);
		}
		/// <summary>
		/// Returns all modifiers of the reaction. Modifiers are similar to reactants in that they modify the reaction rate according to their stochiometry, but their concentration is
		/// neither increased or decreased when the reaction fires.
		/// </summary>
		/// <returns>Modifiers of the reaction</returns>
		stochsim::Collection<stochsim::ReactionLeftElement> GetModifiers() const noexcept
		{
			stochsim::Collection<stochsim::ReactionLeftElement> returnVal;
			for (auto& modifier : modifiers_)
			{
				Molecule::PropertyNames names;
				for (size_t i = 0; i < modifier.propertyNames_.size(); i++)
				{
					names[i] = modifier.propertyNames_[i];
				}
				returnVal.emplace_back(modifier.state_, modifier.stochiometry_, std::move(names));
			}
			return std::move(returnVal);
		}

		/// <summary>
		/// Adds a species as a reactant of the reaction. When the reaction fires, its concentration is decreased according to its stochiometry.
		/// </summary>
		/// <param name="state">Species to add as a reactant.</param>
		/// <param name="stochiometry">Number of molecules of the reactant taking part in a reaction.</param>
		void AddReactant(std::shared_ptr<IState> state, Stochiometry stochiometry = 1, Molecule::PropertyNames propertyNames = Molecule::PropertyNames())
		{
			for (auto& reactant : reactants_)
			{
				if (state == reactant.state_)
				{
					for (auto i = 0; i < propertyNames.size(); i++)
					{
						if (propertyNames[i] != reactant.propertyNames_[i])
						{
							std::stringstream errorMessage;
							errorMessage << "Property " << std::to_string(i) <<" of reactant " << state->GetName() << " in reaction " << GetName() << " was already assigned to the name " << reactant.propertyNames_[i] << ". Cannot re-assign it to the name " << propertyNames[i] <<".";
							throw std::exception(errorMessage.str().c_str());
						}
					}
					reactant.stochiometry_ += stochiometry;
					return;
				}
			}
			reactants_.emplace_back(state, stochiometry, std::move(propertyNames));
		}
		/// <summary>
		/// Adds a species as a modifier of the reaction. Different to a reactant, when the reaction fires, its concentration does not decreased. However, a modifier still changes the rate at which a reaction takes place (e.g. enzymes catalyzing the reaction).
		/// </summary>
		/// <param name="state">Species to add as a modifier.</param>
		/// <param name="stochiometry">Number of molecules of the modifier taking part in a reaction.</param>
		void AddModifier(std::shared_ptr<IState> state, Stochiometry stochiometry = 1, Molecule::PropertyNames propertyNames = Molecule::PropertyNames())
		{
			for (auto& modifier : modifiers_)
			{
				if (state == modifier.state_)
				{
					for (auto i = 0; i < propertyNames.size(); i++)
					{
						if (propertyNames[i] != modifier.propertyNames_[i])
						{
							std::stringstream errorMessage;
							errorMessage << "Property " << std::to_string(i) << " of modifier " << state->GetName() << " in reaction " << GetName() << " was already assigned to the name " << modifier.propertyNames_[i] << ". Cannot re-assign it to the name " << propertyNames[i] << ".";
							throw std::exception(errorMessage.str().c_str());
						}
					}
					modifier.stochiometry_ += stochiometry;
					return;
				}
			}
			modifiers_.emplace_back(state, stochiometry, std::move(propertyNames));
		}
		/// <summary>
		/// Adds a species as a transformee of the reaction. Similar to a modifier, the concentration of a transformee is not changed when the reaction fires, but it still changes the propensity of the reaction.
		/// However, a transformee is additionally transformed by the rection (its method IState.Transform is called).
		/// </summary>
		/// <param name="state">Species to add as a transformee.</param>
		/// <param name="stochiometry">Number of molecules of the transformee taking part in a reaction.</param>
		void AddTransformee(std::shared_ptr<IState> state, Stochiometry stochiometry = 1, Molecule::PropertyExpressions propertyExpressions = Molecule::PropertyExpressions(), Molecule::PropertyNames propertyNames = Molecule::PropertyNames())
		{
			for (auto& transformee : transformees_)
			{
				if (state == transformee.state_)
				{
					for (auto i = 0; i < propertyNames.size(); i++)
					{
						if (propertyNames[i] != transformee.propertyNames_[i])
						{
							std::stringstream errorMessage;
							errorMessage << "Property " << std::to_string(i) << " of transformee " << state->GetName() << " in reaction " << GetName() << " was already assigned to the name " << transformee.propertyNames_[i] << ". Cannot re-assign it to the name " << propertyNames[i] << ".";
							throw std::exception(errorMessage.str().c_str());
						}
					}
					for (auto i = 0; i < propertyExpressions.size(); i++)
					{
						std::string expressionOld("<none>");
						if (transformee.propertyExpressions_[i])
							expressionOld = transformee.propertyExpressions_[i].GetExpression()->ToCmdl();
						std::string expressionNew("<none>");
						if (propertyExpressions[i])
							expressionNew = propertyExpressions[i]->ToCmdl();
						if (expressionOld != expressionNew)
						{
							std::stringstream errorMessage;
							errorMessage << "Property " << std::to_string(i) << " of transformee " << state->GetName() << " in reaction " << GetName() << " was already assigned to the expression " << expressionOld << ". Cannot re-assign it to the expression " << expressionNew << ".";
							throw std::exception(errorMessage.str().c_str());
						}
					}
					transformee.stochiometry_ += stochiometry;
					return;
				}
			}
			transformees_.emplace_back(state, stochiometry, std::move(propertyExpressions), std::move(propertyNames));
		}
		/// <summary>
		/// Adds a species as a product of the reaction. When the reaction fires, its concentration is increased according to its stochiometry.
		/// </summary>
		/// <param name="state">Species to add as a product.</param>
		/// <param name="stochiometry">Number of molecules produced when the reaction fires.</param>
		void AddProduct(std::shared_ptr<IState> state, Stochiometry stochiometry = 1, Molecule::PropertyExpressions propertyExpressions = Molecule::PropertyExpressions())
		{
			for (auto& product : products_)
			{
				if (state == product.state_)
				{
					for (auto i = 0; i < propertyExpressions.size(); i++)
					{
						std::string expressionOld("<none>");
						if (product.propertyExpressions_[i])
							expressionOld = product.propertyExpressions_[i].GetExpression()->ToCmdl();
						std::string expressionNew("<none>");
						if (propertyExpressions[i])
							expressionNew = propertyExpressions[i]->ToCmdl();
						if (expressionOld != expressionNew)
						{
							std::stringstream errorMessage;
							errorMessage << "Property " << std::to_string(i) << " of product " << state->GetName() << " in reaction " << GetName() << " was already assigned to the expression " << expressionOld << ". Cannot re-assign it to the expression " << expressionNew << ".";
							throw std::exception(errorMessage.str().c_str());
						}
					}
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			products_.emplace_back(state, stochiometry, std::move(propertyExpressions));
		}
		virtual void Fire(ISimInfo& simInfo) override
		{
			Variables variables;
			for (const auto& reactant : reactants_)
			{
				for (size_t i = 0; i < reactant.stochiometry_; i++)
				{
					Molecule molecule = reactant.state_->Remove(simInfo);
					for (size_t p = 0; p < molecule.Size(); p++)
					{
						if (!reactant.propertyNames_[p].empty())
						{
							if(reactant.stochiometry_ > 1)
								variables.push_back(Variable(reactant.propertyNames_[p] + "[" + std::to_string(i) + "]", molecule[p]));
							else
								variables.push_back(Variable(reactant.propertyNames_[p], molecule[p]));
							
						}
					}
				}
			}
			for (const auto& modifier : modifiers_)
			{
				for (size_t i = 0; i < modifier.stochiometry_; i++)
				{
					const Molecule& molecule = modifier.state_->Peak(simInfo);
					for (size_t p = 0; p < molecule.Size(); p++)
					{
						if (!modifier.propertyNames_[p].empty())
						{
							if(modifier.stochiometry_ > 1)
								variables.push_back(Variable(modifier.propertyNames_[p] + "[" + std::to_string(i) + "]", molecule[p]));
							else
								variables.push_back(Variable(modifier.propertyNames_[p], molecule[p]));
						}
					}
				}
			}
			for (auto& transformee : transformees_)
			{
				std::vector<Molecule*> molecules;
				for (size_t i = 0; i < transformee.stochiometry_; i++)
				{
					molecules.push_back(&transformee.state_->Transform(simInfo));
					for (size_t p = 0; p < molecules[i]->Size(); p++)
					{
						if (!transformee.propertyNames_[p].empty())
						{
							if (transformee.stochiometry_ > 1)
								variables.push_back(Variable(transformee.propertyNames_[p] + "[" + std::to_string(i) + "]", (*molecules[i])[p]));
							else
								variables.push_back(Variable(transformee.propertyNames_[p], (*molecules[i])[p]));
						}
					}
				}

				for (auto molecule : molecules)
				{
					if (transformee.stochiometry_ > 1)
					{
						// For simplicity, if the stochiometry is bigger than one, the property name without array notation temporarily denotes the respective property of the
						// current molecule.
						for (size_t p = 0; p < molecule->Size(); p++)
						{
							if (!transformee.propertyNames_[p].empty())
							{
								variables.push_back(Variable(transformee.propertyNames_[p], (*molecule)[p]));
							}
						}
					}
					transformee(*molecule, simInfo, variables);

					if (transformee.stochiometry_ > 1)
					{
						// remove all temporarily added variables
						for (size_t p = 0; p < molecule->Size(); p++)
						{
							if (!transformee.propertyNames_[p].empty())
							{
								variables.pop_back();
							}
						}
					}
				}
			}
			for (auto& product : products_)
			{
				Molecule molecule = product(simInfo, variables);
				for (size_t i = 0; i < product.stochiometry_; i++)
				{
					product.state_->Add(simInfo, molecule, variables);
				}
			}
		}
		virtual double ComputeRate(ISimInfo& simInfo) const override
		{
			if (customRate_)
			{
				try
				{
					return customRate_.operator()(simInfo);
				}
				catch (const std::exception& ex)
				{
					std::stringstream errorMessage;
					errorMessage << "Error while computing custom reaction rate of reaction " << name_ << ": " << ex.what();
					throw std::exception(errorMessage.str().c_str());
				}
				catch (...)
				{
					std::stringstream errorMessage;
					errorMessage << "Error while computing custom reaction rate of reaction " << name_ << ": Unexpected error.";
					throw std::exception(errorMessage.str().c_str());
				}
			}
			else
			{
				double rate = rateConstant_;
				for (const auto& reactant : reactants_)
				{
					const long stoch = reactant.stochiometry_;
					const size_t num = reactant.state_->Num(simInfo);
					for (size_t s = 0; s < stoch; s++)
					{
						rate *= num - s;
					}
				}
				for (const auto& modifier : modifiers_)
				{
					const long stoch = modifier.stochiometry_;
					const size_t num = modifier.state_->Num(simInfo);
					for (size_t s = 0; s < stoch; s++)
					{
						rate *= num - s;
					}
				}
				for (const auto& transformee : transformees_)
				{
					const long stoch = transformee.stochiometry_;
					const size_t num = transformee.state_->Num(simInfo);
					for (size_t s = 0; s < stoch; s++)
					{
						rate *= num - s;
					}
				}
				return rate;
			}
		}
		virtual std::string GetName() const override
		{
			return name_;
		}
		virtual void Initialize(ISimInfo& simInfo) override
		{
			if (customRate_)
			{
				customRate_.Initialize(simInfo);
			}

			for (auto& reactant : reactants_)
			{
				reactant.Initialize(simInfo);
			}
			for (auto& product : products_)
			{
				product.Initialize(simInfo);
			}
			for (auto& modifier : modifiers_)
			{
				modifier.Initialize(simInfo);
			}
			for (auto& transformee : transformees_)
			{
				transformee.Initialize(simInfo);
			}
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			for (auto& reactant : reactants_)
			{
				reactant.Uninitialize(simInfo);
			}
			for (auto& product : products_)
			{
				product.Uninitialize(simInfo);
			}
			for (auto& modifier : modifiers_)
			{
				modifier.Uninitialize(simInfo);
			}
			for (auto& transformee : transformees_)
			{
				transformee.Uninitialize(simInfo);
			}

			customRate_.Uninitialize(simInfo);
		}
		/// <summary>
		/// Returns the rate constant of this reaction. If this reaction depends on a custom rate equation instead of a rate constant, returns -1.
		/// </summary>
		/// <returns>Rate constant of reaction. Unit of rate constant is assumed to fit number of reactants.</returns>
		double GetRateConstant() const noexcept
		{
			if (!customRate_)
				return rateConstant_;
			else
				return -1;
		}
		/// <summary>
		/// Sets the rate constant of this reaction. Resets any custom rate equation if defined.
		/// </summary>
		/// <param name="rateConstant">Rate constant of reaction. Unit of rate constant is assumed to fit number of reactants</param>
		void SetRateConstant(double rateConstant) noexcept
		{
			rateConstant_ = rateConstant;
			customRate_.SetExpression(nullptr);
		}

		/// <summary>
		/// Returns the rate equation of this reaction. If this reaction does not have a custom rate equation but instead follows standard mass action kinetics, returns a nullptr.
		/// </summary>
		/// <returns>Custom rate equation of reaction.</returns>
		const expression::IExpression* GetRateEquation() const noexcept
		{
			return customRate_.GetExpression();
		}
		/// <summary>
		/// Sets a custom rate equation for this reaction. If a custom rate equation is defined, the rate of the equation is not determined by standard mass action kinetics.
		/// Instead, the rate is dynamically calculated by solving the mathematical formula provided as an argument. This formula can contain standard math functions like
		/// min, sin and similar, as well as variables having the name of the reactants of this reaction, which are dynamically replaced by the molcular numbers of these reactants
		/// during evaluation. To deactivate the usage of a custom rate equation again, simply define a rate constant for this reaction (i.e. call SetRateConstant(...)).
		/// </summary>
		/// <param name="rateEquation">Custom reaction rate equation.</param>
		void SetRateEquation(std::unique_ptr<expression::IExpression> rateEquation) noexcept
		{
			rateConstant_ = 0;
			customRate_.SetExpression(std::move(rateEquation));
		}
		/// <summary>
		/// Sets a custom rate equation for this reaction. If a custom rate equation is defined, the rate of the equation is not determined by standard mass action kinetics.
		/// Instead, the rate is dynamically calculated by solving the mathematical formula provided as an argument. This formula can contain standard math functions like
		/// min, sin and similar, as well as variables having the name of the reactants of this reaction, which are dynamically replaced by the molcular numbers of these reactants
		/// during evaluation. To deactivate the usage of a custom rate equation again, simply define a rate constant for this reaction (i.e. call SetRateConstant(...)).
		/// </summary>
		/// <param name="rateEquation">Custom reaction rate equation.</param>
		void SetRateEquation(std::string rateEquation)
		{
			expression::ExpressionParser parser;
			SetRateEquation(parser.Parse(rateEquation, false, false));
		}
	private:
		ExpressionHolder customRate_;
		double rateConstant_;
		const std::string name_;
		std::vector<Reactant> reactants_;
		std::vector<Modifier> modifiers_;
		std::vector<Product> products_;
		std::vector<Transformee> transformees_;
	};
}