#pragma once
#include <memory>
#include "stochsim_interfaces.h"
#include <vector>
#include <string>
namespace stochsim
{
	/// <summary>
	/// A reaction which fires once at a specific time (instead of having a propensity).
	/// Good to implement events like adding some substrate at a given time.
	/// </summary>
	class TimerReaction : public IDelayedReaction
	{
	private:
		/// <summary>
		/// Structure to store the information about the products released when the reaction fires, as well as their stochiometries.
		/// </summary>
		struct ReactionElement
		{
		public:
			const Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			ReactionElement(std::shared_ptr<IState> state, Stochiometry stochiometry) : stochiometry_(stochiometry), state_(std::move(state))
			{
			}
		};
	public:
		TimerReaction(std::string name, double fireTime_) : fireTime_(fireTime_), name_(std::move(name))
		{
		}
		virtual double NextReactionTime(ISimInfo& simInfo) const override
		{
			return simInfo.SimTime() < fireTime_ ? fireTime_ : stochsim::inf;
		}
		virtual void Fire(ISimInfo& simInfo) override
		{
			for (const auto& product : products_)
			{
				product.state_->Add(simInfo, product.stochiometry_);
			}
		}
		virtual std::string Name() const override
		{
			return name_;
		}

		/// <summary>
		/// Returns the time when the reaction fires.
		/// </summary>
		/// <returns>Time when reaction fires in simulation time units.</returns>
		double GetFireTime() const
		{
			return fireTime_;
		}
		/// <summary>
		/// Sets the time when the reaction fires.
		/// </summary>
		/// <param name="delay">ime when reaction fires in simulation time units.</param>
		void SetFireTime(double fireTime)
		{
			fireTime_ = fireTime;
		}

		/// <summary>
		/// Adds a species as a product of the reaction. When the reaction fires, its concentration is increased according to its stochiometry.
		/// </summary>
		/// <param name="state">Species to add as a product.</param>
		/// <param name="stochiometry">Number of molecules produced when the reaction fires.</param>
		void AddProduct(std::shared_ptr<IState> state, Stochiometry stochiometry = 1)
		{
			products_.emplace_back(state, stochiometry);
		}
	private:
		double fireTime_;
		const std::string name_;
		std::vector<ReactionElement> products_;
	};
}
