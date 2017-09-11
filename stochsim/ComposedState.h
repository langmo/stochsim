#pragma once
#include <vector>
#include <list>
#include <string>
#include <memory>
#include <functional>
#include <cassert>
#include "stochsim_interfaces.h"
#include "CircularBuffer.h"
namespace stochsim
{
	/// <summary>
	/// A molecule is one element of a ComposedState, having a creation time and a number stating how often it was modified.
	/// </summary>
	struct Molecule
	{
		unsigned int numModified;
		double creationTime;
	};
	
	/// <summary>
	/// A state representing the concentration of a species, where, however, each molecule has its own identiy/properties. That is, the molecules can be distinguished, which means that this
	/// class represents something like a meta-state.
	/// </summary>
	template<class T> class ComposedState :
		public IState
	{
	public:
		/// <summary>
		/// Typedef for listener which gets called when a molecule is removed from this state.
		/// </summary>
		typedef std::function<void(T& molecule, double time)> RemoveListener;
		/// <summary>
		/// Type of the function which has to be supplied to a complex state to initilize the properties of a molecule whenever a new molecule of the species represented by this state is produced.
		/// </summary>
		typedef std::function<void(T&, double)> Initializer;
		/// <summary>
		/// Type of the function which has to be supplied to a complex state to modify the properties of a molecule whenever a molecule of the species represented by this state is modified, i.e.
		/// when State::Modify is called on this state and a given molecule represented by this state was chosen to be modified.
		/// </summary>
		typedef std::function<void(T&, double)> Modifier;

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of the state.</param>
		/// <param name="initialCondition">Initial number of molecules which are there when the simulation starts.</param>
		/// <param name="initialCapacity">Initial maximum amount of molecules which are expected to be hold by this state. If the number of molecules increases over the maximum, the maximum is doubled which requires reallocation of space.</param>
		/// <param name="initializer">Function which initilize the properties of a molecule whenever a new molecule of the species represented by this state is produced.</param>
		/// <param name="modifier">Function which modifies the properties of a molecule whenever a molecule of the species represented by this state is modified, i.e.
		/// when State::Modify is called on this state and a given molecule represented by this state was chosen to be modified.</param>
		ComposedState(std::string name, size_t initialCondition, Initializer initializer, Modifier modifier, size_t initialCapacity = 1000) : name_(name), initialCondition_(initialCondition), initializer_(initializer), modifier_(modifier), buffer_(initialCapacity>initialCondition ? initialCapacity : initialCondition)
		{
		}

		virtual void Initialize(ISimInfo& simInfo) override
		{
			buffer_.Clear();
			for (size_t i = 0; i < InitialCondition(); i++)
			{
				initializer_(buffer_.PushTail(), simInfo.SimTime());
			}
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			buffer_.Clear();
		}
		virtual inline size_t Num() const override
		{
			return buffer_.Size();
		}
		virtual void Add(ISimInfo& simInfo, size_t num = 1) override
		{
			for (size_t i = 0; i < num; i++)
			{
				initializer_(buffer_.PushTail(), simInfo.SimTime());
			}
		}
		
		virtual void Remove(ISimInfo& simInfo, size_t num = 1) override
		{
			for (size_t i = 0; i < num; i++)
			{
				if (!removeListeners_.empty())
				{
					T& molecule = buffer_[0];
					double time = simInfo.SimTime();
					for (auto& removeListener : removeListeners_)
					{
						removeListener(molecule, time);
					}
				}
				buffer_.PopTop();
			}
		}
		virtual inline void Transform(ISimInfo& simInfo) override
		{
			modifier_(buffer_[simInfo.Rand(0, buffer_.Size() - 1)], simInfo.SimTime());
		}
		/// <summary>
		/// Returns the pos^th element/molecule represented by this state. 
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Num().</param>
		/// <returns>Properties of the molecule.</returns>
		inline T& operator[](size_t pos)
		{
			return buffer_[pos];
		}

		virtual std::string Name() const override
		{
			return name_;
		}
		virtual size_t InitialCondition() const override
		{
			return initialCondition_;
		}
		inline void AddRemoveListener(RemoveListener fireListener)
		{
			removeListeners_.push_back(std::move(fireListener));
		}
	private:
		Initializer initializer_;
		Modifier modifier_;

		CircularBuffer<T> buffer_;
		std::list<RemoveListener> removeListeners_;
		const std::string name_;
		const size_t initialCondition_;
	};

	template<> class ComposedState<Molecule> :
		public IState
	{
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
		ComposedState(std::string name, size_t initialCondition, size_t initialCapacity = 1000) : name_(name), initialCondition_(initialCondition), buffer_(initialCapacity>initialCondition ? initialCapacity : initialCondition)
		{
		}

		virtual void Initialize(ISimInfo& simInfo) override
		{
			buffer_.Clear();
			for (size_t i = 0; i < InitialCondition(); i++)
			{
				Molecule& molecule = buffer_.PushTail();
				molecule.numModified = 0;
				molecule.creationTime = simInfo.SimTime();
			}
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			buffer_.Clear();
		}
		virtual inline size_t Num() const override
		{
			return buffer_.Size();
		}
		inline void AddRemoveListener(RemoveListener fireListener)
		{
			removeListeners_.push_back(std::move(fireListener));
		}
		virtual void Add(ISimInfo& simInfo, size_t num = 1) override
		{
			for (size_t i = 0; i < num; i++)
			{
				Molecule& molecule = buffer_.PushTail();
				molecule.numModified = 0;
				molecule.creationTime = simInfo.SimTime();
			}
		}

		virtual void Remove(ISimInfo& simInfo, size_t num = 1) override
		{
			for (size_t i = 0; i < num; i++)
			{
				if (!removeListeners_.empty())
				{
					Molecule& molecule = buffer_[0];
					double time = simInfo.SimTime();
					for (auto& removeListener : removeListeners_)
					{
						removeListener(molecule, time);
					}
				}
				buffer_.PopTop();
			}
		}
		virtual inline void Transform(ISimInfo& simInfo) override
		{
			buffer_[simInfo.Rand(0, buffer_.Size() - 1)].numModified++;
		}
		/// <summary>
		/// Returns the pos^th element/molecule represented by this state. 
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Num().</param>
		/// <returns>Properties of the molecule.</returns>
		inline Molecule& operator[](size_t pos)
		{
			return buffer_[pos];
		}

		virtual std::string Name() const override
		{
			return name_;
		}
		virtual size_t InitialCondition() const override
		{
			return initialCondition_;
		}
	private:
		CircularBuffer<Molecule> buffer_;
		std::list<RemoveListener> removeListeners_;
		const std::string name_;
		const size_t initialCondition_;
	};

	
}
