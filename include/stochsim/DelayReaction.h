#pragma once
#include "ComposedState.h"
#include <memory>
#include "stochsim_common.h"
#include <vector>
#include <sstream>
namespace stochsim
{
	/// <summary>
	/// A reaction which fires at a specific time (instead of having a propensity), with the time when the reaction fires next being determined by the properties of the first molecule of a ComplexState.
	/// Since the first molecule of a complex state is also the oldest molecule, this type of reaction typically represents a reaction firing a fixed delay after a molecule of a given species was created.
	/// </summary>
	class DelayReaction : public IEventReaction
	{
	private:
		/// <summary>
		/// Structure to store the information about the products of a DelayedReaction, as well as their stochiometries.
		/// </summary>
		struct ReactionElementWithModifiers
		{
		public:
			Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			const MoleculeProperties properties_;
			ReactionElementWithModifiers(std::shared_ptr<IState> state, Stochiometry stochiometry, MoleculeProperties properties) : stochiometry_(stochiometry), state_(std::move(state)), properties_(std::move(properties))
			{
			}
		};
	public:
		DelayReaction(std::string name, std::shared_ptr<ComposedState> state, double delay) : state_(std::move(state)), delay_(delay), name_(std::move(name))
		{
			for (int i = 0; i < numMoleculeProperties; i++)
			{
				std::stringstream name;
				name << "N"<<i;
				variables_.emplace_back(name.str(), 0);
			}
		}
		virtual double NextReactionTime(ISimInfo& simInfo) const override
		{
			return state_->Num(simInfo) > 0 ? state_->PeakFirst().creationTime + delay_ : stochsim::inf;
		}
		virtual void Fire(ISimInfo& simInfo) override
		{
			auto& molecule = state_->PeakFirst();
			for (int i = 0; i < numMoleculeProperties; i++)
			{
				variables_[i].second=molecule.properties[i];
			}
			state_->RemoveFirst(simInfo);
			for (const auto& product : products_)
			{
				product.state_->Add(simInfo, product.stochiometry_, product.properties_, variables_);
			}
		}
		virtual std::string GetName() const override
		{
			return name_;
		}
		virtual void Initialize(ISimInfo& simInfo) override
		{
			// do nothing.
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			// do nothing.
		}

		/// <summary>
		/// Returns the current delay of the reaction
		/// </summary>
		/// <returns>Current delay in simulation time units.</returns>
		double GetDelay() const
		{
			return delay_;
		}
		/// <summary>
		/// Sets the delay of the reaction.
		/// </summary>
		/// <param name="delay">Delay in simulation time units.</param>
		void SetDelay(double delay)
		{
			delay_ = delay;
		}


		/// <summary>
		/// Adds a species as a product of the reaction. When the reaction fires, its concentration is increased according to its stochiometry.
		/// </summary>
		/// <param name="state">Species to add as a product.</param>
		/// <param name="stochiometry">Number of molecules produced when the reaction fires.</param>
		void AddProduct(std::shared_ptr<IState> state, Stochiometry stochiometry = 1, MoleculeProperties moleculeProperties = defaultMoleculeProperties)
		{
			for (auto& product : products_)
			{
				if (state == product.state_)
				{
					if (product.properties_ != moleculeProperties)
					{
						std::stringstream errorMessage;
						errorMessage << "State " << state->GetName() << " cannot take part in reaction " << GetName() << " as a product with different properties being initialized.";
						throw std::exception(errorMessage.str().c_str());
					}
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			products_.emplace_back(state, stochiometry, moleculeProperties);
		}
		/// <summary>
		/// Returns all products of the reaction.
		/// </summary>
		/// <returns>Products of the reaction.</returns>
		stochsim::Collection<stochsim::ReactionElement> GetProducts() const
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& product : products_)
			{
				returnVal.emplace_back(product.state_, product.stochiometry_);
			}
			return std::move(returnVal);
		}
		/// <summary>
		/// Returns the reactant of the reaction. A delayed reaction always has exactly one reactant with stochiometry one.
		/// </summary>
		/// <returns>Reactant of the reaction.</returns>
		std::shared_ptr<ComposedState> GetReactant() const
		{
			return state_;
		}
	private:
		std::vector<Variable> variables_;
		double delay_;
		std::shared_ptr<ComposedState> state_;
		const std::string name_;
		std::vector<ReactionElementWithModifiers> products_;
	};
}

