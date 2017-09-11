#pragma once
#include "stochsim_interfaces.h"
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include "ReactionRate.h"
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
		struct ReactionElement
		{
		public:
			Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			const bool modifier_;
			ReactionElement(std::shared_ptr<IState> state, Stochiometry stochiometry, bool modifier) : stochiometry_(stochiometry), state_(std::move(state)), modifier_(modifier)
			{
			}
		};
	public:
		PropensityReaction(std::string name, double rateConstant) : name_(std::move(name)), rateConstant_(rateConstant), rateEquation_("")
		{
		}
		PropensityReaction(std::string name, std::string rateEquation) : name_(std::move(name)), rateConstant_(0), rateEquation_(rateEquation)
		{
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
						errorMessage << "State "<<state->Name()<<" cannot take part in reaction "<<Name()<<" both as a modifier/transformee and as a normal reactant.";
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
						errorMessage << "State " << state->Name() << " cannot take part in reaction " << Name() << " both as a modifier/transformee and as a normal reactant.";
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
			for (auto& reactant : reactants_)
			{
				if (state == reactant.state_)
				{
					if (!reactant.modifier_)
					{
						std::stringstream errorMessage;
						errorMessage << "State " << state->Name() << " cannot take part in reaction " << Name() << " both as a modifier/transformee and as a normal reactant.";
						throw std::exception(errorMessage.str().c_str());
					}
					else
					{
						reactant.stochiometry_ += stochiometry;
						return;
					}
				}
			}
			for (auto& product : products_)
			{
				if (state == product.state_)
				{
					if (!product.modifier_)
					{
						std::stringstream errorMessage;
						errorMessage << "State " << state->Name() << " cannot take part in reaction " << Name() << " both as a transformee and as a normal product.";
						throw std::exception(errorMessage.str().c_str());
					}
					else
					{
						product.stochiometry_ += stochiometry;
						return;
					}
				}
			}
			reactants_.emplace_back(state, stochiometry, true);
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
						errorMessage << "State " << state->Name() << " cannot take part in reaction " << Name() << " both as a transformee and as a normal product.";
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
			for (const auto& reactant : reactants_)
			{
				if (!reactant.modifier_)
				{
					reactant.state_->Remove(simInfo, reactant.stochiometry_);
				}
			}
		}
		virtual double ComputeRate(ISimInfo& simInfo) const override
		{
			if (rateEquation_.empty())
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
			else
				return reactionRate_.CalculateRate(simInfo);
		}
		virtual std::string Name() const override
		{
			return name_;
		}
		virtual void Initialize(ISimInfo& simInfo) override
		{
			if (rateEquation_.empty())
				return;
			std::vector<std::shared_ptr<IState>> reactantStates;
			std::transform(reactants_.begin(), reactants_.end(), std::back_inserter(reactantStates), [](ReactionElement& element) -> const std::shared_ptr<IState>& {return element.state_; });
			reactionRate_.Initialize(rateEquation_, reactantStates);
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			// do nothing.
		}
		/// <summary>
		/// Returns the rate constant of this reaction. If this reaction depends on a custom rate equation instead of a rate constant, returns -1.
		/// </summary>
		/// <returns>Rate constant of reaction. Unit of rate constant is assumed to fit number of reactants.</returns>
		double GetRateConstant() const
		{
			if (rateEquation_.empty())
				return rateConstant_;
			else
				return -1;
		}
		/// <summary>
		/// Sets the rate constant of this reaction. Resets any custom rate equation if defined.
		/// </summary>
		/// <param name="rateConstant">Rate constant of reaction. Unit of rate constant is assumed to fit number of reactants</param>
		void SetRateConstant(double rateConstant)
		{
			rateEquation_ = "";
			rateConstant_ = rateConstant;
		}

		/// <summary>
		/// Returns the rate equation of this reaction. If this reaction does not have a custom rate equation but instead follows standard mass action kinetics, returns an empty string.
		/// </summary>
		/// <returns>Custom rate equation of reaction. Unit is one divided by simulation time.</returns>
		std::string GetRateEquation() const
		{
			return rateEquation_;
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
			rateConstant_ = 0;
			rateEquation_ = std::move(rateEquation);
		}
	private:
		ReactionRate reactionRate_;
		double rateConstant_;
		const std::string name_;
		std::string rateEquation_;
		std::vector<ReactionElement> reactants_;
		std::vector<ReactionElement> products_;
	};
}