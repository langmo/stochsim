#pragma once
#include "ComplexState.h"
#include <memory>
#include "types.h"

namespace stochsim
{
/// <summary>
/// A reaction which fires at a specific time (instead of having a propensity), with the time when the reaction fires next being determined by the properties of the first molecule of a ComplexState.
/// Since the first molecule of a complex state is also the oldest molecule, this type of reaction typically represents a reaction firing a fixed delay after a molecule of a given species was created.
/// </summary>
template<class T> class ComplexDelayedReaction : public DelayedReaction
{
public:
	/// <summary>
	/// Type definition of the function used to calculat the next firing time based on the properties of the first element represented by a ComplexState.
	/// </summary>
	typedef std::function<double(T& molecule)> FireTime;
	/// <summary>
	/// Type definition of the function which determines what should happen if the reaction fires.
	/// </summary>
	typedef std::function<void(T& molecule, SimInfo& simInfo)> FireAction;

	ComplexDelayedReaction(std::shared_ptr<ComplexState<T>> state, FireTime fireTime, FireAction fireAction);
	
	virtual double NextReactionTime() const override;
	virtual void Fire(SimInfo& simInfo) override;
private:
	FireTime fireTime_;
	FireAction fireAction_;
	std::shared_ptr<ComplexState<T>> state_;
};

template<class T>
inline ComplexDelayedReaction<T>::ComplexDelayedReaction(std::shared_ptr<ComplexState<T>> state, FireTime fireTime, FireAction fireAction) : state_(state), fireTime_(fireTime), fireAction_(fireAction)
{
}

template<class T>
inline double ComplexDelayedReaction<T>::NextReactionTime() const
{
	return state_->Num() > 0 ? fireTime_(state_->Get(0)) : stochsim::inf;
}
template<class T>
inline void ComplexDelayedReaction<T>::Fire(SimInfo & simInfo)
{
	fireAction_(state_->Get(0), simInfo);
}
}