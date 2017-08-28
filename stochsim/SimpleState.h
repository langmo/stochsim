#pragma once
#include "types.h"
namespace stochsim
{
	/// <summary>
	/// The simplest and most common implementation of a state. The number of elements in a SimpleState are simply represented by an integer, realizing the case when molecules represented by this state
	/// cannot be distinguished. As a consequence, these molecules cannot be modified, neither (SimpleState::Modify does nothing).
	/// </summary>
	class SimpleState :
		public State
	{
	public:
		SimpleState(std::string name, long initialCondition) : num_(0), name_(name), initialCondition_(initialCondition)
		{
		}
		virtual unsigned long Num() const override
		{
			return num_;
		}
		virtual void Add(SimInfo& simInfo, unsigned long num = 1) override
		{
			num_ += num;
		}
		virtual void Remove(SimInfo& simInfo, unsigned long num = 1) override
		{
			num_ -= num;
		}
		virtual void Modify(SimInfo& simInfo) override
		{
			// do nothing, simple states are not counting...
		}
		virtual void Initialize(SimInfo& simInfo) override
		{
			num_ = InitialCondition();
		}
		virtual void Uninitialize(SimInfo& simInfo) override
		{
			num_ = 0;
		}
		virtual std::string Name() const override
		{
			return name_;
		}
		virtual unsigned long InitialCondition() const override
		{
			return initialCondition_;
		}
	private:
		unsigned long num_;
		const std::string name_;
		const unsigned long initialCondition_;
	};
}
