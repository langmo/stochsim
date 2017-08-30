#pragma once
#include "types.h"
#include <vector>
#include <memory>
namespace stochsim
{
	/// <summary>
	/// Simple and most typical type of a reaction. The propensity of the reaction is proportional to its rate, and the product of the absolut reactant numbers (taking their stochiometry into account).
	/// When the reaction fires, for most reactant/products the absolute numbers are increased/decreased accoding to their stochiometries.
	/// However, when a reactant is flagged (its modifier is true), its concentration is not decreased when the reaction fires, which allows to implement e.g. enzymes catalyzing a reaction.
	/// In contrary, when a product is flagged (its modifier is true), its concentration is also not increased when the reaction is fired, but instead the modify function is called on the respective state.
	/// </summary>
	class SimpleReaction :
		public PropensityReaction
	{
	public:
		/// <summary>
		/// Structure to store the information about the reactants and products of a SimpleReaction, as well as their stochiometries.
		/// </summary>
		struct ReactionElement
		{
		public:
			const long stochiometry_;
			const std::shared_ptr<State> state_;
			const bool modifier_;
			ReactionElement(std::shared_ptr<State> state, long stochiometry, bool modifier) : stochiometry_(stochiometry), state_(std::move(state)), modifier_(modifier)
			{
			}
		};

		SimpleReaction(std::string name, double rateConstant) : name_(std::move(name)), rateConstant_(rateConstant)
		{
		}
		/// <summary>
		/// Adds a species as a reactant of the reaction. When the reaction fires, its concentration is decreased according to its stochiometry, except when the modifier is true, which
		/// indicates species who should modify the propensity of the reaction, but not change their concentration  (e.g. enzymes catalyzing the reaction).
		/// </summary>
		/// <param name="state">Species to add as a reactant.</param>
		/// <param name="stochiometry">Number of molecules of the reactant taking part in a reaction.</param>
		/// <param name="modifier">If false, the concentration of the species is decreased when the reaction fires according to the stochiometry. If true, the concentration is not modified (e.g. enzymes).</param>
		void AddReactant(std::shared_ptr<State> state, long stochiometry = 1, bool modifier = false)
		{
			reactants_.emplace_back(state, stochiometry, modifier);
		}
		/// <summary>
		/// Adds a species as a product of the reaction. When the reaction fires, its concentration is increased according to its stochiometry, except when the modifier is true.
		/// In this case, its concentration is neither increased nor decreased, but instead the State::Modify function is called on the respective state. Useful to e.g. count how often a given molecule takes
		/// part in a reaction where the molecule acts as a modifier.
		/// </summary>
		/// <param name="state">Species to add as a product.</param>
		/// <param name="stochiometry">Number of molecules produced when the reaction fires.</param>
		/// <param name="modifier">If false, the concentration of the species is increased when the reaction fires according to the stochiometry. If true, the concentration is not modified, but instead State::Modify() is called.</param>
		void AddProduct(std::shared_ptr<State> state, long stochiometry = 1, bool modifier = false)
		{
			products_.emplace_back(state, stochiometry, modifier);
		}
		virtual void Fire(SimInfo& simInfo) override
		{
			for (const auto& product : products_)
			{
				if (product.modifier_)
				{
					product.state_->Modify(simInfo);
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
		virtual double ComputeRate() const override
		{
			double rate = rateConstant_;
			for (const auto& reactant : reactants_)
			{
				const long stoch = reactant.stochiometry_;
				const unsigned long num = reactant.state_->Num();
				for (int s = 0; s < stoch; s++)
				{
					rate *= num - s;
				}

			}
			return rate;
		}
		virtual std::string Name() const override
		{
			return name_;
		}
	private:
		const double rateConstant_;
		const std::string name_;
		std::vector<ReactionElement> reactants_;
		std::vector<ReactionElement> products_;
	};
}