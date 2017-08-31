#pragma once
#include <memory>
#include <vector>
namespace stochsim
{
	// Forward declariation for the iteration
	template<class T> class CircularBufferIterator;

	/// <summary>
	/// An implementation of a circular buffer which automatically doubles its capacity when it is full.
	/// </summary>
	template<class T> class CircularBuffer
	{
	public:
		typedef CircularBufferIterator<T> iterator;
		typedef CircularBufferIterator<const T> const_iterator;
		typedef typename std::vector<T>::size_type size_type;
		iterator                                       begin() { return iterator(*this, 0); }
		iterator                                       end() { return iterator(*this, Size()); }

		const_iterator                                 cbegin() const { return const_iterator(*this, 0); }
		const_iterator                                 cend() const { return const_iterator(*this, Size()); }

		CircularBuffer(size_type initialCapacity = 1000) : capacity_(initialCapacity), elements_(initialCapacity), start_(0), end_(0)
		{
		}
		inline void Clear()
		{
			start_ = 0;
			end_ = 0;
		}
		inline size_type Size() const
		{
			return (end_ + capacity_ - start_) % capacity_;
		}
		inline T& PushTail()
		{
			size_type oldEnd = end_;
			end_ = (end_ + 1) % capacity_;
			if (end_ == start_)
			{
				doupleCapacity();
				return elements_[end_-1];
			}
			else
				return elements_[oldEnd];
		}

		inline void PopTop()
		{
			if (end_ == start_)
				throw std::exception("Circular buffer empty!");
			start_ = (start_ + 1) % capacity_;
		}
		/// <summary>
		/// Returns the pos^th element in the buffer. 
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Num().</param>
		/// <returns>Properties of the molecule.</returns>
		inline T& operator[](size_type pos)
		{
			return elements_[(start_ + pos) % capacity_];
		}

	private:
		size_type start_;
		size_type end_;
		std::vector<T> elements_;
		size_type capacity_;

		/// <summary>
		/// Should only be called if vector is completely full.
		/// </summary>
		void doupleCapacity()
		{
			assert(start_ == end_);
			std::vector<T> temp(capacity_ * 2);
			for (size_type i = 0; i < capacity_; i++)
			{
				temp[i] = std::move(elements_[(start_ + i) % capacity_]);
			}
			elements_ = std::move(temp);
			start_ = 0;
			end_ = capacity_;
			capacity_ *= 2;
		}
	};

	template<class T> class CircularBufferIterator
	{
	private:
		CircularBuffer<T> & buffer_;
		typename CircularBuffer<T>::size_type pos_;
	public:
		CircularBufferIterator(CircularBuffer<T> & buffer, typename CircularBuffer<T>::size_type pos)
			: buffer_(buffer), pos_(pos)
		{}

		operator bool()const
		{
			if (pos_ < buffer_.Size() && pos_ >= 0)
				return true;
			else
				return false;
		}

		bool operator==(const CircularBufferIterator<T>& rawIterator)const { return (pos_ == rawIterator.pos_); }
		bool operator!=(const CircularBufferIterator<T>& rawIterator)const { return (pos_ != rawIterator.pos_); }

		CircularBufferIterator<T>&                  operator+=(const ptrdiff_t& movement) { pos_ += movement; return (*this); }
		CircularBufferIterator<T>&                  operator-=(const ptrdiff_t& movement) { pos_ -= movement; return (*this); }
		CircularBufferIterator<T>&                  operator++() { ++pos_; return (*this); }
		CircularBufferIterator<T>&                  operator--() { --pos_; return (*this); }
		CircularBufferIterator<T>                   operator++(int) { auto temp(*this); ++pos_; return temp; }
		CircularBufferIterator<T>                   operator--(int) { auto temp(*this); --pos_; return temp; }
		CircularBufferIterator<T>                   operator+(const ptrdiff_t& movement) { auto oldPos = pos_; pos_ += movement; auto temp(*this); pos_ = oldPos; return temp; }
		CircularBufferIterator<T>                   operator-(const ptrdiff_t& movement) { auto oldPos = pos_; pos_ -= movement; auto temp(*this); pos_ = oldPos; return temp; }

		T&											operator*() { return buffer_.Get(pos_); }
		const T&									operator*()const { return buffer_.Get(pos_); }
		T*											operator->() { return &buffer_.Get(pos_); }
	};
}
