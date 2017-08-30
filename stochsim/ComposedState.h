#pragma once
#include "ComplexState.h"
namespace stochsim
{
	/// <summary>
	/// A molecule is one element of a ComposedState, having a creation time and a number stating how often it was modified.
	/// </summary>
	struct Molecule
	{
		unsigned long numModified;
		double creationTime;
	};
	
	
	/// <summary>
	/// A state representing the concentration of a species, where, however, each molecule has its own identiy/properties. That is, the molecules can be distinguished, which means that this
	/// class represents something like a meta-state.
	/// </summary>
	template<class T> class CircularBuffer
	{
	public:
		CircularBuffer(unsigned long initialCapacity = 1000) : maxNum_(initialCapacity), elements_(initialCapacity), start_(0), end_(0)
		{
		}
		inline void Clear()
		{
			start_ = 0;
			end_ = 0;
		}
		inline unsigned long Size() const
		{
			return (end_ + maxNum_ - start_) % maxNum_;
		}
		inline T& PushTail()
		{
			T& temp = elements_[end_];
			end_ = (end_ + 1) % maxNum_;
			if (end_ == start_)
				doupleCapacity();
			return temp;
		}
	
		inline void PopTop()
		{
			if (end_ == start_)
				throw "Circular buffer empty!";
			start_ = (start_ + 1) % maxNum_;
		}
		/// <summary>
		/// Returns the pos^th element in the buffer. 
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Num().</param>
		/// <returns>Properties of the molecule.</returns>
		inline T& operator[](unsigned long pos)
		{
			return elements_[(start_ + pos) % maxNum_];
		}

	private:
		unsigned long start_;
		unsigned long end_;
		std::vector<T> elements_;
		unsigned long maxNum_;

		friend class ComplexStateIterator<T>;
		/// <summary>
		/// Should only be called if vector is completely full.
		/// </summary>
		void doupleCapacity()
		{
			assert(start_ == end_);
			std::vector<T> temp(maxNum_ * 2);
			for (unsigned long i = 0; i < maxNum_; i++)
			{
				temp[i] = std::move(elements_[(start_ + i) % maxNum_]);
			}
			elements_ = std::move(temp);
			start_ = 0;
			end_ = maxNum_;
			maxNum_ *= 2;
		}
	};



	/// <summary>
	/// A state representing the concentration of a species, where, however, each molecule has its own identiy/properties. That is, the molecules can be distinguished, which means that this
	/// class represents something like a meta-state.
	/// </summary>
	template<class T> class ComposedState :
		public State
	{
	public:
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
		ComposedState(std::string name, unsigned long initialCondition, Initializer initializer, Modifier modifier, unsigned long initialCapacity = 1000) : name_(name), initialCondition_(initialCondition), initializer_(initializer), modifier_(modifier), buffer_(initialCapacity>initialCondition ? initialCapacity : initialCondition)
		{
		}

		virtual void Initialize(SimInfo& simInfo) override
		{
			buffer_.Clear();
			for (unsigned long i = 0; i < InitialCondition(); i++)
			{
				initializer_(buffer_.PushTail(), simInfo.SimTime());
			}
		}
		virtual void Uninitialize(SimInfo& simInfo) override
		{
			buffer_.Clear();
		}
		virtual inline unsigned long Num() const override
		{
			return buffer_.Size();
		}
		virtual void Add(SimInfo& simInfo, unsigned long num = 1) override
		{
			for (unsigned long i = 0; i < num; i++)
			{
				initializer_(buffer_.PushTail(), simInfo.SimTime());
			}
		}
		
		virtual void Remove(SimInfo& simInfo, unsigned long num = 1) override
		{
			for (unsigned long i = 0; i < num; i++)
			{
				buffer_.PopTop();
			}
		}
		virtual inline void Modify(SimInfo& simInfo) override
		{
			modifier_(buffer_[simInfo.Rand(0, buffer_.Size() - 1)], simInfo.SimTime());
		}
		/// <summary>
		/// Returns the pos^th element/molecule represented by this state. 
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Num().</param>
		/// <returns>Properties of the molecule.</returns>
		inline T& Get(unsigned long pos)
		{
			return buffer_[pos];
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
		Initializer initializer_;
		Modifier modifier_;

		CircularBuffer<T> buffer_;

		const std::string name_;
		const unsigned long initialCondition_;
	};

	template<> class ComposedState<Molecule> :
		public State
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of the state.</param>
		/// <param name="initialCondition">Initial number of molecules which are there when the simulation starts.</param>
		/// <param name="initialCapacity">Initial maximum amount of molecules which are expected to be hold by this state. If the number of molecules increases over the maximum, the maximum is doubled which requires reallocation of space.</param>
		/// <param name="initializer">Function which initilize the properties of a molecule whenever a new molecule of the species represented by this state is produced.</param>
		/// <param name="modifier">Function which modifies the properties of a molecule whenever a molecule of the species represented by this state is modified, i.e.
		/// when State::Modify is called on this state and a given molecule represented by this state was chosen to be modified.</param>
		ComposedState(std::string name, unsigned long initialCondition, unsigned long initialCapacity = 1000) : name_(name), initialCondition_(initialCondition), buffer_(initialCapacity>initialCondition ? initialCapacity : initialCondition)
		{
		}

		virtual void Initialize(SimInfo& simInfo) override
		{
			buffer_.Clear();
			for (unsigned long i = 0; i < InitialCondition(); i++)
			{
				Molecule& molecule = buffer_.PushTail();
				molecule.numModified = 0;
				molecule.creationTime = simInfo.SimTime();
			}
		}
		virtual void Uninitialize(SimInfo& simInfo) override
		{
			buffer_.Clear();
		}
		virtual inline unsigned long Num() const override
		{
			return buffer_.Size();
		}
		virtual void Add(SimInfo& simInfo, unsigned long num = 1) override
		{
			for (unsigned long i = 0; i < num; i++)
			{
				Molecule& molecule = buffer_.PushTail();
				molecule.numModified = 0;
				molecule.creationTime = simInfo.SimTime();
			}
		}

		virtual void Remove(SimInfo& simInfo, unsigned long num = 1) override
		{
			for (unsigned long i = 0; i < num; i++)
			{
				buffer_.PopTop();
			}
		}
		virtual inline void Modify(SimInfo& simInfo) override
		{
			buffer_[simInfo.Rand(0, buffer_.Size() - 1)].numModified++;
		}
		/// <summary>
		/// Returns the pos^th element/molecule represented by this state. 
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Num().</param>
		/// <returns>Properties of the molecule.</returns>
		inline Molecule& Get(unsigned long pos)
		{
			return buffer_[pos];
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
		CircularBuffer<Molecule> buffer_;

		const std::string name_;
		const unsigned long initialCondition_;
	};




	/*
	/// <summary>
	/// The most common case of a ComplexState:
	/// A state representing the concentration of a species, where each molecule has a creation time and counts how often it was modified.
	/// </summary>
	class ComposedState : public ComplexState<Molecule>
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of the state.</param>
		/// <param name="initialCondition">Initial number of molecules which are there when the simulation starts.</param>
		/// <param name="initialCapacity">Initial maximum amount of molecules which are expected to be hold by this state. If the number of molecules increases over the maximum, the maximum is doubled which requires reallocation of space.</param>
		/// <param name="initializer">Function which initilize the properties of a molecule whenever a new molecule of the species represented by this state is produced.</param>
		/// <param name="modifier">Function which modifies the properties of a molecule whenever a molecule of the species represented by this state is modified, i.e.
		/// when State::Modify is called on this state and a given molecule represented by this state was chosen to be modified.</param>
		ComposedState(std::string name, unsigned long initialCondition, unsigned long initialCapacity = 1000) : ComplexState<Molecule>(name, initialCondition, [](Molecule& molecule, double time) {
			molecule.numModified = 0;
			molecule.creationTime = time;
		}, [](Molecule& molecule, double time) {
			molecule.numModified++;
		}, initialCapacity)
		{
		}
	};*/
	
}
