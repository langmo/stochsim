#pragma once
#include "CustomState.h"
#include <memory>
#include "stochsim_common.h"
#include <vector>
namespace stochsim
{
	/// <summary>
	/// A reaction which fires at a specific time (instead of having a propensity), with the time when the reaction fires next being determined by the properties of the first molecule of a ComplexState.
	/// Since the first molecule of a complex state is also the oldest molecule, this type of reaction typically represents a reaction firing a fixed delay after a molecule of a given species was created.
	/// </summary>
	template<class T> class CustomDelayReaction : public IEventReaction
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

		CustomDelayReaction(std::string name, std::shared_ptr<CustomState<T>> state, FireTime fireTime, FireAction fireAction) : state_(std::move(state)), fireTime_(fireTime), fireAction_(fireAction), name_(std::move(name))
		{

		}
		virtual double NextReactionTime(ISimInfo& simInfo) const override
		{
			return state_->Num(simInfo) > 0 ? fireTime_((*state_)[0]) : stochsim::inf;
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
		std::shared_ptr<CustomState<T>> state_;
		const std::string name_;
	};
}

