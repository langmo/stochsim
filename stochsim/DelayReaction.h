#pragma once
#include "ComposedState.h"
#include <memory>
#include "stochsim_interfaces.h"
#include <vector>
namespace stochsim
{
	/// <summary>
	/// A reaction which fires at a specific time (instead of having a propensity), with the time when the reaction fires next being determined by the properties of the first molecule of a ComplexState.
	/// Since the first molecule of a complex state is also the oldest molecule, this type of reaction typically represents a reaction firing a fixed delay after a molecule of a given species was created.
	/// </summary>
	template<class T> class DelayReaction : public IDelayedReaction
	{
	public:
		/// <summary>
		/// Type definition of the function used to calculat the next firing time based on the properties of the first element represented by a ComplexState.
		/// </summary>
		typedef std::function<double(T& molecule)> FireTime;
		/// <summary>
		/// Type definition of the function which determines what should happen if the reaction fires.
		/// </summary>
		typedef std::function<void(T& molecule, ISimInfo& simInfo)> FireAction;

		DelayReaction(std::string name, std::shared_ptr<ComposedState<T>> state, FireTime fireTime, FireAction fireAction) : state_(std::move(state)), fireTime_(fireTime), fireAction_(fireAction), name_(std::move(name))
		{

		}
		virtual double NextReactionTime() const override
		{
			return state_->Num() > 0 ? fireTime_((*state_)[0]) : stochsim::inf;
		}
		virtual void Fire(ISimInfo& simInfo) override
		{
			fireAction_((*state_)[0], simInfo);
		}
		virtual std::string Name() const override
		{
			return name_;
		}
	private:
		FireTime fireTime_;
		FireAction fireAction_;
		std::shared_ptr<ComposedState<T>> state_;
		const std::string name_;
	};

	template<> class DelayReaction<Molecule> : public IDelayedReaction
	{
	private:
		/// <summary>
		/// Structure to store the information about the products of a DelayedReaction, as well as their stochiometries.
		/// </summary>
		struct ReactionElement
		{
		public:
			const Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			const bool modifier_;
			ReactionElement(std::shared_ptr<IState> state, Stochiometry stochiometry, bool modifier) : stochiometry_(stochiometry), state_(std::move(state)), modifier_(modifier)
			{
			}
		};
	public:
		DelayReaction(std::string name, std::shared_ptr<ComposedState<Molecule>> state, double delay) : state_(std::move(state)), delay_(delay), name_(std::move(name))
		{

		}
		virtual double NextReactionTime() const override
		{
			return state_->Num() > 0 ? (*state_)[0].creationTime + delay_ : stochsim::inf;
		}
		virtual void Fire(ISimInfo& simInfo) override
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
			state_->Remove(simInfo);
		}
		virtual std::string Name() const override
		{
			return name_;
		}

		/// <summary>
		/// Adds a species as a product of the reaction. When the reaction fires, its concentration is increased according to its stochiometry, except when the modifier is true.
		/// In this case, its concentration is neither increased nor decreased, but instead the State::Modify function is called on the respective state. Useful to e.g. count how often a given molecule takes
		/// part in a reaction where the molecule acts as a modifier.
		/// </summary>
		/// <param name="state">Species to add as a product.</param>
		/// <param name="stochiometry">Number of molecules produced when the reaction fires.</param>
		/// <param name="modifier">If false, the concentration of the species is increased when the reaction fires according to the stochiometry. If true, the concentration is not modified, but instead State::Modify() is called.</param>
		void AddProduct(std::shared_ptr<IState> state, Stochiometry stochiometry = 1, bool modifier = false)
		{
			products_.emplace_back(state, stochiometry, modifier);
		}
	private:
		const double delay_;
		std::shared_ptr<ComposedState<Molecule>> state_;
		const std::string name_;
		std::vector<ReactionElement> products_;
	};
}

