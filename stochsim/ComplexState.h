#pragma once
#include "types.h"
#include <string>
#include <functional>
#include <vector>
#include <random>
#include <cassert>
namespace stochsim
{
template<class T> class ComplexStateIterator;

/// <summary>
/// A state representing the concentration of a species, where, however, each molecule has its own identiy/properties. That is, the molecules can be distinguished, which means that this
/// class represents something like a meta-state.
/// </summary>
template<class T> class ComplexState :
	public State
{
public:
	typedef ComplexStateIterator<T> iterator;
	typedef ComplexStateIterator<const T> const_iterator;
	iterator                                       begin() { return iterator(*this, 0); }
	iterator                                       end() { return iterator(*this, Num()); }

	const_iterator                                 cbegin() const { return const_iterator(*this, 0); }
	const_iterator                                 cend() const { return const_iterator(*this, Num()); }

	/// <summary>
	/// Type of the function which has to be supplied to a complex state to initilize the properties of a molecule whenever a new molecule of the species represented by this state is produced.
	/// </summary>
	typedef std::function<void (T&, double)> Initializer;
	/// <summary>
	/// Type of the function which has to be supplied to a complex state to modify the properties of a molecule whenever a molecule of the species represented by this state is modified, i.e.
	/// when State::Modify is called on this state and a given molecule represented by this state was chosen to be modified.
	/// </summary>
	typedef std::function<void (T&, double)> Modifier;

	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="name">Name of the state.</param>
	/// <param name="initialCondition">Initial number of molecules which are there when the simulation starts.</param>
	/// <param name="initialCapacity">Initial maximum amount of molecules which are expected to be hold by this state. If the number of molecules increases over the maximum, the maximum is doubled which requires reallocation of space.</param>
	/// <param name="initializer">Function which initilize the properties of a molecule whenever a new molecule of the species represented by this state is produced.</param>
	/// <param name="modifier">Function which modifies the properties of a molecule whenever a molecule of the species represented by this state is modified, i.e.
	/// when State::Modify is called on this state and a given molecule represented by this state was chosen to be modified.</param>
	ComplexState(std::string name, unsigned long initialCondition, Initializer initializer, Modifier modifier, unsigned long initialCapacity=1000) : name_(name), initialCondition_(initialCondition), initializer_(initializer), modifier_(modifier), start_(0), end_(0), maxNum_(initialCapacity>initialCondition ? initialCapacity: initialCondition), elements_(initialCapacity>initialCondition ? initialCapacity : initialCondition)
	{
	}

	virtual void Initialize(SimInfo& simInfo) override
	{
		start_ = 0;
		end_ = 0;
		for (unsigned long i = 0; i < InitialCondition(); i++)
		{
			initializer_(elements_[end_], simInfo.SimTime());
			end_ = (end_ + 1) % maxNum_;
			if (end_ == start_)
				doupleCapacity();
		}
	}
	virtual void Uninitialize(SimInfo& simInfo) override
	{
		start_ = 0;
		end_ = 0;
	}
	virtual inline unsigned long Num() const override
	{
		return (end_ + maxNum_ - start_) % maxNum_;
	}
	virtual void Add(SimInfo& simInfo, unsigned long num = 1) override
	{
		for (unsigned long i = 0; i < num; i++)
		{
			initializer_(elements_[end_], simInfo.SimTime());
			end_ = (end_ + 1) % maxNum_;
			if (end_ == start_)
				doupleCapacity();
		}
	}
	/// <summary>
	/// Adds a molecule which is already initialized.
	/// The molecule is copied by value and added to the set of molecules represented by this state. The initializer is not called on this molecule.
	/// </summary>
	/// <param name="simInfo">Simulation context.</param>
	/// <param name="molecule">Molecule to add.</param>
	inline void Add(SimInfo& simInfo, T molecule)
	{
		elements_[end_] = std::move(molecule);
		end_ = (end_ + 1) % maxNum_;
		if (end_ == start_)
			doupleCapacity();
	}
	virtual void Remove(SimInfo& simInfo, unsigned long num = 1) override
	{
		for (unsigned long i = 0; i < num; i++)
		{
			if (end_ == start_)
				throw "Complex state empty!";
			start_ = (start_ + 1) % maxNum_;
		}
	}
	virtual inline void Modify(SimInfo& simInfo) override
	{
		modifier_(Get(simInfo.Rand(0, Num() - 1)), simInfo.SimTime());
	}
	/// <summary>
	/// Returns the pos^th element/molecule represented by this state. 
	/// </summary>
	/// <param name="pos">Index greater equal to zero and smaller than Num().</param>
	/// <returns>Properties of the molecule.</returns>
	inline T& Get(unsigned long pos)
	{
		return elements_[(start_ + pos) % maxNum_];
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
	
	unsigned long start_;
	unsigned long end_;
	std::vector<T> elements_;
	unsigned long maxNum_;

	const std::string name_;
	const unsigned long initialCondition_;

	friend class ComplexStateIterator<T>;
	/// <summary>
	/// Should only be called if vector is completely full.
	/// </summary>
	void doupleCapacity()
	{
		assert(start_ == end_);
		std::vector<T> temp(maxNum_*2);
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

template<class T> class ComplexStateIterator
{
private:
	ComplexState<T> & state_;
	unsigned long pos_;
public:
	ComplexStateIterator(ComplexState<T> & state, unsigned long pos)
		: state_(state), pos_(pos)
	{}

	operator bool()const
	{
		if (pos_ < state_.Num() && pos_ >= 0)
			return true;
		else
			return false;
	}

	bool operator==(const ComplexStateIterator<T>& rawIterator)const { return (pos_ == rawIterator.pos_); }
	bool operator!=(const ComplexStateIterator<T>& rawIterator)const { return (pos_ != rawIterator.pos_); }

	ComplexStateIterator<T>&                  operator+=(const ptrdiff_t& movement) { pos_ += movement; return (*this); }
	ComplexStateIterator<T>&                  operator-=(const ptrdiff_t& movement) { pos_ -= movement; return (*this); }
	ComplexStateIterator<T>&                  operator++() { ++pos_; return (*this); }
	ComplexStateIterator<T>&                  operator--() { --pos_; return (*this); }
	ComplexStateIterator<T>                   operator++(int) { auto temp(*this); ++pos_; return temp; }
	ComplexStateIterator<T>                   operator--(int) { auto temp(*this); --pos_; return temp; }
	ComplexStateIterator<T>                   operator+(const ptrdiff_t& movement) { auto oldPos = pos_; pos_ += movement; auto temp(*this); pos_ = oldPos; return temp; }
	ComplexStateIterator<T>                   operator-(const ptrdiff_t& movement) { auto oldPos = pos_; pos_ -= movement; auto temp(*this); pos_ = oldPos; return temp; }

	T&											operator*() { return state_.Get(pos_); }
	const T&									operator*()const { return state_.Get(pos_); }
	T*											operator->() { return &state_.Get(pos_);}
};
}