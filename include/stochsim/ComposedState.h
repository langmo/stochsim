#pragma once
#include <vector>
#include <list>
#include <string>
#include <memory>
#include <functional>
#include <cassert>
#include "stochsim_common.h"
#include "CircularBuffer.h"
namespace stochsim
{	
	/// <summary>
	/// A state representing the concentration of a species, where, however, each molecule has its own identiy/properties. That is, the molecules can be distinguished, which means that this
	/// class represents something like a meta-state.
	/// </summary>
	class ComposedState:
		public IState
	{
	private:
		struct MoleculeHolder
		{
			Molecule molecule;
			/// <summary>
			/// The simulation time when the molecule was created.
			/// </summary>
			double creationTime;
			/// <summary>
			/// True if the molecule is already deleted. Keeping this variable is necessary due to implementation details.
			/// Molecules which are returned must always have invalidated=false.
			/// </summary>
			bool invalidated;
		};
	public:
		/// <summary>
		/// Typedef for listener which gets called when a molecule is removed from this state.
		/// </summary>
		typedef std::function<void(const Molecule& molecule, double time)> RemoveListener;

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of the state.</param>
		/// <param name="initialCondition">Initial number of molecules which are there when the simulation starts.</param>
		/// <param name="initialCapacity">Initial maximum amount of molecules which are expected to be hold by this state. If the number of molecules increases over the maximum, the maximum is doubled which requires reallocation of space.</param>
		/// <param name="initializer">Function which initilize the properties of a molecule whenever a new molecule of the species represented by this state is produced.</param>
		/// <param name="modifier">Function which modifies the properties of a molecule whenever a molecule of the species represented by this state is modified, i.e.
		/// when State::Modify is called on this state and a given molecule represented by this state was chosen to be modified.</param>
		ComposedState(std::string name, size_t initialCondition, size_t initialCapacity = 1000) : name_(name), size_(0), initialCondition_(initialCondition), buffer_(initialCapacity>initialCondition ? initialCapacity : initialCondition)
		{
		}

		virtual void Initialize(ISimInfo& simInfo) override
		{
			buffer_.Clear();
			size_ = GetInitialCondition();
			for (size_t i = 0; i < size_; i++)
			{
				MoleculeHolder& holder = buffer_.PushTail();
				holder.molecule.Reset();
				holder.creationTime = simInfo.GetSimTime();
				holder.invalidated = false;
			}
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			buffer_.Clear();
			size_ = 0;
		}
		virtual inline size_t Num(ISimInfo& simInfo) const override
		{
			return size_;
		}
		inline void AddRemoveListener(RemoveListener fireListener)
		{
			removeListeners_.push_back(std::move(fireListener));
		}
		virtual void Add(ISimInfo& simInfo, const Molecule& molecule = defaultMolecule, const Variables& variables = {}) override
		{
			MoleculeHolder& holder = buffer_.PushTail();
			holder.molecule = molecule;
			holder.creationTime = simInfo.GetSimTime();
			holder.invalidated = false;
			size_ ++;
		}

		virtual Molecule Remove(ISimInfo& simInfo, const Variables& variables = {}) override
		{
			size_t idx = randomBufferIndex(simInfo);
			if (idx == 0)
			{
				return RemoveFirst(simInfo, variables);
			}
			else
			{
				MoleculeHolder& holder = buffer_[idx];
				if (!removeListeners_.empty())
				{
					double time = simInfo.GetSimTime();
					for (auto& removeListener : removeListeners_)
					{
						removeListener(holder.molecule, time);
					}
				}
				holder.invalidated = true;
				size_--;
				return holder.molecule;
			}
		}

		Molecule RemoveFirst(ISimInfo& simInfo, const Variables& variables = {})
		{
			Molecule molecule = buffer_[0].molecule;
			if (!removeListeners_.empty())
			{
				double time = simInfo.GetSimTime();
				for (auto& removeListener : removeListeners_)
				{
					removeListener(molecule, time);
				}
			}
			// First element guaranteed to be valid.
			buffer_.PopTop();
			size_--;
			// Remove new first element if it happens to be invalid to guarantee that first element is always valid.
			while (buffer_.Size() > 0 && buffer_[0].invalidated)
			{
				buffer_.PopTop();
			}
			return molecule;
		}
		/// <summary>
		/// Returns the creation time of the first, that is, oldest molecule. Behavior undefined if size is zero.
		/// </summary>
		/// <returns>Creation time of oldest element</returns>
		inline double PeakFirstCreationTime(ISimInfo& simInfo) const
		{
			return buffer_[0].creationTime;
		}
		virtual const Molecule& Peak(ISimInfo& simInfo) const
		{
			return buffer_[randomBufferIndex(simInfo)].molecule;
		}
		virtual inline Molecule& Transform(ISimInfo& simInfo, const Variables& variables = {}) override
		{
			return buffer_[randomBufferIndex(simInfo)].molecule;
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
		/// <summary>
		/// Returns a (uniform) random index in the buffer. Since the buffer might contain already invalidated elements,
		/// the random index might be drawn from a broader range.
		/// </summary>
		/// <returns>A uniform random index to a valid buffer element.</returns>
		inline size_t randomBufferIndex(ISimInfo& simInfo) const
		{
			size_t idx;
			// try three times to draw a valid random number. If this fails, clean up the buffer.
			for (int trial = 0; trial < 3; trial++)
			{
				idx = simInfo.Rand(0, buffer_.Size() - 1);
				if (!buffer_[idx].invalidated)
					return idx;
			}
			// Remove all invalidated molecules.
			// first, sort buffer by creation time, however, with all invalidated molecules coming first.
			buffer_.Sort([](const MoleculeHolder& a, const MoleculeHolder& b) -> bool
			{
				return !b.invalidated && (a.invalidated || a.creationTime < b.creationTime);
			});
			// remove all invalidated elements, which are now at the front.
			buffer_.PopTop(buffer_.Size() - size_);
			// return just a uniformly sampled index. Since all elements are now valid, this index is valid, too.
			return simInfo.Rand(0, buffer_.Size() - 1);
		}

		mutable CircularBuffer<MoleculeHolder> buffer_;
		std::list<RemoveListener> removeListeners_;
		const std::string name_;
		size_t initialCondition_;
		size_t size_;
	};
}
