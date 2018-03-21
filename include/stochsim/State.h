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
		virtual void Add(ISimInfo& simInfo, const Molecule& molecule = defaultMolecule, const Variables& variables = {}) override
		{
			if (!addListeners_.empty())
			{
				double time = simInfo.GetSimTime();
				for (auto& addListener : addListeners_)
				{
					addListener(molecule, time);
				}
			}
			num_ ++;
		}
		virtual Molecule Remove(ISimInfo& simInfo, const Variables& variables = {}) override
		{
			if (!removeListeners_.empty())
			{
				double time = simInfo.GetSimTime();
				for (auto& removeListener : removeListeners_)
				{
					removeListener(defaultMolecule, time);
				}
			}
			num_ --;
			return defaultMolecule;
		}
		virtual Molecule& Transform(ISimInfo& simInfo, const Variables& variables = {}) override
		{
			static Molecule molecule;
			molecule.Reset();
			return molecule;
		}
		virtual const Molecule& Peak(ISimInfo& simInfo) const
		{
			return defaultMolecule;
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
		virtual inline void AddDecreaseListener(StateListener stateListener) override
		{
			removeListeners_.push_back(std::move(stateListener));
		}
		virtual inline void AddIncreaseListener(StateListener stateListener) override
		{
			addListeners_.push_back(std::move(stateListener));
		}
	private:
		size_t num_;
		const std::string name_;
		size_t initialCondition_;
		std::list<StateListener> removeListeners_;
		std::list<StateListener> addListeners_;
	};
}
