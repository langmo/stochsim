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
	template<class T> class BasicDelayReaction : public IDelayedReaction
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

		BasicDelayReaction(std::string name, std::shared_ptr<BasicComposedState<T>> state, FireTime fireTime, FireAction fireAction) : state_(std::move(state)), fireTime_(fireTime), fireAction_(fireAction), name_(std::move(name))
		{

		}
		virtual double NextReactionTime(ISimInfo& simInfo) const override
		{
			return state_->Num() > 0 ? fireTime_((*state_)[0]) : stochsim::inf;
		}
		virtual void Fire(ISimInfo& simInfo) override
		{
			fireAction_((*state_)[0], simInfo);
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
	private:
		FireTime fireTime_;
		FireAction fireAction_;
		std::shared_ptr<BasicComposedState<T>> state_;
		const std::string name_;
	};

	template<> class BasicDelayReaction<Molecule> : public IDelayedReaction
	{
	private:
		/// <summary>
		/// Structure to store the information about the products of a DelayedReaction, as well as their stochiometries.
		/// </summary>
		struct ReactionElement
		{
		public:
			Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			ReactionElement(std::shared_ptr<IState> state, Stochiometry stochiometry) : stochiometry_(stochiometry), state_(std::move(state))
			{
			}
		};
	public:
		BasicDelayReaction(std::string name, std::shared_ptr<ComposedState> state, double delay) : state_(std::move(state)), delay_(delay), name_(std::move(name))
		{
		}
		virtual double NextReactionTime(ISimInfo& simInfo) const override
		{
			return state_->Num() > 0 ? (*state_)[0].creationTime + delay_ : stochsim::inf;
		}
		virtual void Fire(ISimInfo& simInfo) override
		{
			for (const auto& product : products_)
			{
				product.state_->Add(simInfo, product.stochiometry_);
			}
			state_->Remove(simInfo);
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
		void AddProduct(std::shared_ptr<IState> state, Stochiometry stochiometry = 1)
		{
			for (auto& product : products_)
			{
				if (state == product.state_)
				{
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			products_.emplace_back(state, stochiometry);
		}
	private:
		double delay_;
		std::shared_ptr<ComposedState> state_;
		const std::string name_;
		std::vector<ReactionElement> products_;
	};

	typedef BasicDelayReaction<Molecule> DelayReaction;
}

