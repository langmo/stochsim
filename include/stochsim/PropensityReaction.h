#pragma once
#include "stochsim_common.h"
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <sstream>
#include <algorithm>
#include "ReactionRate.h"
#include "expression_common.h"
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
		/// <summary>
		/// Structure to store the information about the reactants and products of a SimpleReaction, as well as their stochiometries.
		/// </summary>
		struct ReactionElementWithModifiers
		{
		public:
			Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			const bool modifier_;
			ReactionElementWithModifiers(std::shared_ptr<IState> state, Stochiometry stochiometry, bool modifier) noexcept : stochiometry_(stochiometry), state_(std::move(state)), modifier_(modifier)
			{
			}
		};
	public:
		PropensityReaction(std::string name, double rateConstant) noexcept : name_(std::move(name)), rateConstant_(rateConstant)
		{
		}
		PropensityReaction(std::string name, std::unique_ptr<expression::IExpression> rateEquation) noexcept : name_(std::move(name)), rateConstant_(0), rateEquation_(std::move(rateEquation))
		{
		}
		/// <summary>
		/// Returns all reactants of the reaction. Modifiers and Transformees are not considered to be reactants.
		/// </summary>
		/// <returns>Reactants of reaction.</returns>
		stochsim::Collection<stochsim::ReactionElement> GetReactants() const noexcept
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& reactant : reactants_)
			{
				if (!reactant.modifier_)
				{
					returnVal.emplace_back(reactant.state_, reactant.stochiometry_);
				}
			}
			return std::move(returnVal);
		}
		/// <summary>
		/// Returns all products of the reaction. Modifiers and Transformees are not considered to be products.
		/// </summary>
		/// <returns>Products of the reaction.</returns>
		stochsim::Collection<stochsim::ReactionElement> GetProducts() const noexcept
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& product : products_)
			{
				if (!product.modifier_)
				{
					returnVal.emplace_back(product.state_, product.stochiometry_);
				}
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
		stochsim::Collection<stochsim::ReactionElement> GetTransformees() const noexcept
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& product : products_)
			{
				if (product.modifier_)
				{
					returnVal.emplace_back(product.state_, product.stochiometry_);
				}
			}
			return std::move(returnVal);
		}
		/// <summary>
		/// Returns all modifiers of the reaction. Modifiers are similar to reactants in that they modify the reaction rate according to their stochiometry, but their concentration is
		/// neither increased or decreased when the reaction fires.
		/// </summary>
		/// <returns>Modifiers of the reaction</returns>
		stochsim::Collection<stochsim::ReactionElement> GetModifiers() const noexcept
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& reactant : reactants_)
			{
				if (reactant.modifier_)
				{
					Stochiometry stoch = reactant.stochiometry_;
					for (auto& product : products_)
					{
						if (product.modifier_ && product.state_ == reactant.state_)
						{
							stoch -= product.stochiometry_;
						}
					}
					if(stoch > 0)
						returnVal.emplace_back(reactant.state_, stoch);
				}
			}
			return std::move(returnVal);
		}

		/// <summary>
		/// Adds a species as a reactant of the reaction. When the reaction fires, its concentration is decreased according to its stochiometry.
		/// </summary>
		/// <param name="state">Species to add as a reactant.</param>
		/// <param name="stochiometry">Number of molecules of the reactant taking part in a reaction.</param>
		void AddReactant(std::shared_ptr<IState> state, Stochiometry stochiometry = 1)
		{
			for (auto& reactant : reactants_)
			{
				if (state == reactant.state_)
				{
					if (reactant.modifier_)
					{
						std::stringstream errorMessage;
						errorMessage << "State "<<state->GetName()<<" cannot take part in reaction "<< GetName()<<" both as a modifier/transformee and as a normal reactant.";
						throw std::exception(errorMessage.str().c_str());
					}
					else
					{
						reactant.stochiometry_ += stochiometry;
						return;
					}
				}
			}
			reactants_.emplace_back(state, stochiometry, false);
		}
		/// <summary>
		/// Adds a species as a modifier of the reaction. Different to a reactant, when the reaction fires, its concentration does not decreased. However, a modifier still changes the rate at which a reaction takes place (e.g. enzymes catalyzing the reaction).
		/// </summary>
		/// <param name="state">Species to add as a modifier.</param>
		/// <param name="stochiometry">Number of molecules of the modifier taking part in a reaction.</param>
		void AddModifier(std::shared_ptr<IState> state, Stochiometry stochiometry = 1)
		{
			for (auto& reactant : reactants_)
			{
				if (state == reactant.state_)
				{
					if (!reactant.modifier_)
					{
						std::stringstream errorMessage;
						errorMessage << "State " << state->GetName() << " cannot take part in reaction " << GetName() << " both as a modifier/transformee and as a normal reactant.";
						throw std::exception(errorMessage.str().c_str());
					}
					else
					{
						reactant.stochiometry_ += stochiometry;
						return;
					}
				}
			}
			reactants_.emplace_back(state, stochiometry, true);
		}
		/// <summary>
		/// Adds a species as a transformee of the reaction. Similar to a modifier, the concentration of a transformee is not changed when the reaction fires, but it still changes the propensity of the reaction.
		/// However, a transformee is additionally transformed by the rection (its method IState.Transform is called).
		/// </summary>
		/// <param name="state">Species to add as a transformee.</param>
		/// <param name="stochiometry">Number of molecules of the transformee taking part in a reaction.</param>
		void AddTransformee(std::shared_ptr<IState> state, Stochiometry stochiometry = 1)
		{
			bool addedReactant = false;
			for (auto& reactant : reactants_)
			{
				if (state == reactant.state_)
				{
					if (!reactant.modifier_)
					{
						std::stringstream errorMessage;
						errorMessage << "State " << state->GetName() << " cannot take part in reaction " << GetName() << " both as a modifier/transformee and as a normal reactant.";
						throw std::exception(errorMessage.str().c_str());
					}
					else
					{
						reactant.stochiometry_ += stochiometry;
						addedReactant = true;
						break;
					}
				}
			}
			if (!addedReactant)
			{
				reactants_.emplace_back(state, stochiometry, true);
			}

			bool addedProduct = false;
			for (auto& product : products_)
			{
				if (state == product.state_)
				{
					if (!product.modifier_)
					{
						std::stringstream errorMessage;
						errorMessage << "State " << state->GetName() << " cannot take part in reaction " << GetName() << " both as a transformee and as a normal product.";
						throw std::exception(errorMessage.str().c_str());
					}
					else
					{
						product.stochiometry_ += stochiometry;
						addedProduct = true;
					}
				}
			}
			if(!addedProduct)
				products_.emplace_back(state, stochiometry, true);
		}
		/// <summary>
		/// Adds a species as a product of the reaction. When the reaction fires, its concentration is increased according to its stochiometry.
		/// </summary>
		/// <param name="state">Species to add as a product.</param>
		/// <param name="stochiometry">Number of molecules produced when the reaction fires.</param>
		void AddProduct(std::shared_ptr<IState> state, Stochiometry stochiometry = 1)
		{
			for (auto& product : products_)
			{
				if (state == product.state_)
				{
					if (product.modifier_)
					{
						std::stringstream errorMessage;
						errorMessage << "State " << state->GetName() << " cannot take part in reaction " << GetName() << " both as a transformee and as a normal product.";
						throw std::exception(errorMessage.str().c_str());
					}
					else
					{
						product.stochiometry_ += stochiometry;
						return;
					}
				}
			}
			products_.emplace_back(state, stochiometry, false);
		}
		virtual void Fire(ISimInfo& simInfo) override
		{
			for (const auto& reactant : reactants_)
			{
				if (!reactant.modifier_)
				{
					reactant.state_->Remove(simInfo, reactant.stochiometry_);
				}
			}
			for (const auto& product : products_)
			{
				if (product.modifier_)
				{
					product.state_->Transform(simInfo, product.stochiometry_);
				}
				else
				{
					product.state_->Add(simInfo, product.stochiometry_);
				}
			}
		}
		virtual double ComputeRate(ISimInfo& simInfo) const override
		{
			if (customRate_)
			{
				try
				{
					return customRate_(simInfo);
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
					const size_t num = reactant.state_->Num();
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
			if (rateEquation_)
			{
				std::vector<std::shared_ptr<IState>> reactantsPtrs;
				std::transform(reactants_.begin(), reactants_.end(), std::back_inserter(reactantsPtrs),
					[](const ReactionElementWithModifiers& reactant) -> std::shared_ptr<IState> {return reactant.state_; });

				customRate_ = ReactionRate(rateEquation_.get(), reactantsPtrs);
			}
			else
				customRate_ = ReactionRate();
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			customRate_ = ReactionRate();
		}
		/// <summary>
		/// Returns the rate constant of this reaction. If this reaction depends on a custom rate equation instead of a rate constant, returns -1.
		/// </summary>
		/// <returns>Rate constant of reaction. Unit of rate constant is assumed to fit number of reactants.</returns>
		double GetRateConstant() const noexcept
		{
			if (!rateEquation_)
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
			rateEquation_ = nullptr;
		}

		/// <summary>
		/// Returns the rate equation of this reaction. If this reaction does not have a custom rate equation but instead follows standard mass action kinetics, returns a nullptr.
		/// </summary>
		/// <returns>Custom rate equation of reaction.</returns>
		const expression::IExpression* GetRateEquation() const noexcept
		{
			return rateEquation_.get();
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
			rateEquation_ = std::move(rateEquation);
		}
	private:
		ReactionRate customRate_;
		double rateConstant_;
		const std::string name_;
		std::unique_ptr<expression::IExpression> rateEquation_;
		std::vector<ReactionElementWithModifiers> reactants_;
		std::vector<ReactionElementWithModifiers> products_;
	};
}