#pragma once
#include "stochsim_common.h"
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
		virtual size_t Num(ISimInfo& simInfo) const override
		{
			return num_;
		}
		virtual void Add(ISimInfo& simInfo, size_t num = 1, std::initializer_list<Variable> variables = {}) override
		{
			num_ += num;
		}
		virtual void Remove(ISimInfo& simInfo, size_t num = 1, std::initializer_list<Variable> variables = {}) override
		{
			num_ -= num;
		}
		virtual void Transform(ISimInfo& simInfo, size_t num = 1, std::initializer_list<Variable> variables = {}) override
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
		virtual std::string GetName() const noexcept override
		{
			return name_;
		}
		/// <summary>
		///  Returns the initial condition of the state. It holds that at t=0, Num()==GetInitialCondition().
		/// </summary>
		/// <returns>Initial condition of the state.</returns>
		size_t GetInitialCondition() const
		{
			return initialCondition_;
		}
		/// <summary>
		/// Sets the initial condition of the state. It holds that at t=0, Num()==GetInitialCondition().
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
