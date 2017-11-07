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
			/// True if the molecule is already deleted. Keeping this variable is necessary due to implementation details.
			/// Molecules which are returned must always have invalidated=false.
			/// </summary>
			bool invalidated;
		};
	public:
		/// <summary>
		/// Typedef for listener which gets called when a molecule is removed from this state.
		/// </summary>
		typedef std::function<void(Molecule& molecule, double time)> RemoveListener;

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
				MoleculeHolder& molecule = buffer_.PushTail();
				molecule.molecule.properties = defaultMoleculeProperties;
				molecule.molecule.creationTime = simInfo.GetSimTime();
				molecule.invalidated = false;
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
		virtual void Add(ISimInfo& simInfo, size_t num = 1, const MoleculeProperties& moleculeProperties = defaultMoleculeProperties, const std::vector<Variable>& variables = {}) override
		{
			for (size_t i = 0; i < num; i++)
			{
				MoleculeHolder& molecule = buffer_.PushTail();
				molecule.molecule.properties = moleculeProperties;
				molecule.molecule.creationTime = simInfo.GetSimTime();
				molecule.invalidated = false;
			}
			size_ += num;
		}

		virtual void Remove(ISimInfo& simInfo, size_t num = 1, const std::vector<Variable>& variables = {}) override
		{
			for (size_t i = 0; i < num; i++)
			{
				size_t idx = randomBufferIndex(simInfo);
				if (idx == 0)
				{
					RemoveFirst(simInfo, 1, variables);
				}
				else
				{
					if (!removeListeners_.empty())
					{
						MoleculeHolder& molecule = buffer_[idx];
						double time = simInfo.GetSimTime();
						for (auto& removeListener : removeListeners_)
						{
							removeListener(molecule.molecule, time);
						}
					}
					buffer_[idx].invalidated = true;
					size_--;
				}
			}
		}

		void RemoveFirst(ISimInfo& simInfo, size_t num = 1, const std::vector<Variable>& variables = {})
		{
			for (size_t i = 0; i < num; i++)
			{
				if (!removeListeners_.empty())
				{
					MoleculeHolder& molecule = buffer_[0];
					double time = simInfo.GetSimTime();
					for (auto& removeListener : removeListeners_)
					{
						removeListener(molecule.molecule, time);
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
			}
		}
		/// <summary>
		/// Returns the first, that is, oldest molecule. Behvior undefined if size is zero.
		/// </summary>
		/// <returns>Oldest element</returns>
		inline const Molecule& PeakFirst()
		{
			return buffer_[0].molecule;
		}
		virtual inline void Transform(ISimInfo& simInfo, size_t num = 1, const MoleculeProperties& moleculeProperties = defaultMoleculeTransformation, const std::vector<Variable>& variables = {}) override
		{
			for (size_t i = 0; i < num; i++)
			{
				auto& molecule = buffer_[randomBufferIndex(simInfo)].molecule;
				for (int j = 0; j < numMoleculeProperties; j++)
				{
					molecule.properties[j] += moleculeProperties[j];
				}
			}
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
		inline size_t randomBufferIndex(ISimInfo& simInfo)
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
				return !b.invalidated && (a.invalidated || a.molecule.creationTime < b.molecule.creationTime);
			});
			// remove all invalidated elements, which are now at the front.
			buffer_.PopTop(buffer_.Size() - size_);
			// return just a uniformly sampled index. Since all elements are now valid, this index is valid, too.
			return simInfo.Rand(0, buffer_.Size() - 1);
		}

		CircularBuffer<MoleculeHolder> buffer_;
		std::list<RemoveListener> removeListeners_;
		const std::string name_;
		size_t initialCondition_;
		size_t size_;
	};
}
