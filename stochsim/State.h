#pragma once
#include "stochsim_interfaces.h"
namespace stochsim
{
	/// <summary>
	/// The simplest and most common implementation of a state. The number of elements in a SimpleState are simply represented by an integer, realizing the case when molecules represented by this state
	/// cannot be distinguished. As a consequence, these molecules cannot be modified, neither (SimpleState::Modify does nothing).
	/// </summary>
	class State :
		public IState
	{
	public:
		State(std::string name, size_t initialCondition) : num_(0), name_(name), initialCondition_(initialCondition)
		{
		}
		virtual size_t Num() const override
		{
			return num_;
		}
		virtual void Add(ISimInfo& simInfo, size_t num = 1) override
		{
			num_ += num;
		}
		virtual void Remove(ISimInfo& simInfo, size_t num = 1) override
		{
			num_ -= num;
		}
		virtual void Transform(ISimInfo& simInfo, size_t num = 1) override
		{
			// do nothing, simple states are not counting...
		}
		virtual void Initialize(ISimInfo& simInfo) override
		{
			num_ = GetInitialCondition();
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			num_ = 0;
		}
		virtual std::string GetName() const override
		{
			return name_;
		}
		virtual size_t GetInitialCondition() const override
		{
			return initialCondition_;
		}
		/// <summary>
		/// Sets the initial condition of the state. It must be guaranteed that at t=0, Num()==GetInitialCondition().
		/// </summary>
		/// <param name="initialCondition">initial condition</param>
		void SetInitialCondition(size_t initialCondition)
		{
			initialCondition_ = initialCondition;
		}
	private:
		size_t num_;
		const std::string name_;
		size_t initialCondition_;
	};
}
