#pragma once
#include <memory>
#include <vector>
#include <algorithm> 
namespace stochsim
{
	// Forward declariation for the iteration
	template<class T> class CircularBufferIterator;

	/// <summary>
	/// An implementation of a circular buffer which automatically doubles its capacity when it is full.
	/// </summary>
	template<class T> class CircularBuffer
	{
		constexpr static double sizeChangeFactor = 1.5;
	public:
		typedef CircularBufferIterator<T> iterator;
		typedef CircularBufferIterator<const T> const_iterator;
		typedef typename std::vector<T>::size_type size_type;
		iterator                                       begin() 
		{ 
			return iterator(this, 0); 
		}
		iterator                                       end() 
		{ 
			return iterator(this, Size()); 
		}

		const_iterator                                 cbegin() const 
		{ 
			return const_iterator(this, 0); 
		}
		const_iterator                                 cend() const 
		{ 
			return const_iterator(this, Size()); 
		}

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

		inline void PopTop(size_type num=1)
		{
			if (end_ == start_)
				throw std::runtime_error("Circular buffer empty!");
			start_ = (start_ + num) % capacity_;
		}
		/// <summary>
		/// Returns the pos^th element in the buffer. 
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Size().</param>
		/// <returns>pos^th element in the buffer.</returns>
		inline T& operator[](size_type pos)
		{
			return elements_[(start_ + pos) % capacity_];
		}

		/// <summary>
		/// Returns the pos^th element in the buffer. 
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Size().</param>
		/// <returns>pos^th element in the buffer.</returns>
		inline const T& operator[](size_type pos) const
		{
			return elements_[(start_ + pos) % capacity_];
		}

		/// <summary>
		/// Returns the pos^th element in the buffer. 
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Size().</param>
		/// <returns>pos^th element in the buffer.</returns>
		inline T& Get(size_type pos)
		{
			return elements_[(start_ + pos) % capacity_];
		}

		/// <summary>
		/// Returns the pos^th element in the buffer. 
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Size().</param>
		/// <returns>pos^th element in the buffer.</returns>
		inline const T& Get(size_type pos) const
		{
			return elements_[(start_ + pos) % capacity_];
		}

		/// <summary>
		/// Sets the pos^th element in the buffer
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Size().</param>
		/// <param name="other">New value of pos^th element.</param>
		inline void Set(size_type pos, const T& other)
		{
			elements_[(start_ + pos) % capacity_] = other;
		}
		/// <summary>
		/// Sets the pos^th element in the buffer
		/// </summary>
		/// <param name="pos">Index greater equal to zero and smaller than Size().</param>
		/// <param name="other">New value of pos^th element.</param>
		inline void Set(size_type pos, T&& other)
		{
			elements_[(start_ + pos) % capacity_] = std::move(other);
		}

		/// <summary>
		/// Sorts the buffer according to the order defined by the comparator.
		/// The comparatormust satisfy the requirements of Compare (see std documentation), e.g. a lambda function
		/// [](const T& a, const T& b) -> bool, which returns true if a is smaller than b. The ordering must be a strict weak ordering relation,
		/// which implies that false should be returned if two elements are equal.
		/// </summary>
		/// <param name="comp">Comparator.</param>
		template<class Compare> inline void Sort(Compare comp)
		{
			std::sort(begin(), end(), comp);
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
			size_t newCapacity = static_cast<size_t>(capacity_ * sizeChangeFactor);
			std::vector<T> temp(newCapacity);
			for (size_type i = 0; i < capacity_; i++)
			{
				temp[i] = std::move(elements_[(start_ + i) % capacity_]);
			}
			elements_ = std::move(temp);
			start_ = 0;
			end_ = capacity_;
			capacity_ = newCapacity;
		}
	};

	template<class T> class CircularBufferIterator : public std::iterator<std::random_access_iterator_tag, T, ptrdiff_t>
	{
	private:
		CircularBuffer<T>* buffer_;
		typename CircularBuffer<T>::size_type pos_;
	public:
		CircularBufferIterator(CircularBuffer<T>* buffer, typename CircularBuffer<T>::size_type pos)
			: buffer_(buffer), pos_(pos)
		{
		}
		CircularBufferIterator(const CircularBufferIterator<T>& other)
			: buffer_(other.buffer_), pos_(other.pos_)
		{
		}
		CircularBufferIterator() : buffer_(nullptr), pos_(0)
		{
		}
		CircularBufferIterator<T>& operator= (const CircularBufferIterator<T>& other)
		{
			buffer_ = other.buffer_;
			pos_ = other.pos_;
			return *this;
		}

		bool IsValid() const
		{
			if (buffer_ && pos_ < buffer_->Size() && pos_ >= 0)
				return true;
			else
				return false;
		}

		bool operator==(const CircularBufferIterator<T>& rawIterator) const 
		{ 
			return (pos_ == rawIterator.pos_ && buffer_ == rawIterator.buffer_); 
		}
		bool operator<=(const CircularBufferIterator<T>& rawIterator) const
		{
			return pos_ <= rawIterator.pos_;
		}
		bool operator>=(const CircularBufferIterator<T>& rawIterator) const
		{
			return pos_ >= rawIterator.pos_;
		}
		bool operator<(const CircularBufferIterator<T>& rawIterator) const
		{
			return pos_ < rawIterator.pos_;
		}
		bool operator>(const CircularBufferIterator<T>& rawIterator) const
		{
			return pos_ > rawIterator.pos_;
		}
		bool operator!=(const CircularBufferIterator<T>& rawIterator) const 
		{ 
			return (pos_ != rawIterator.pos_ || buffer_ != rawIterator.buffer_);
		}

		CircularBufferIterator<T>&                  operator+=(const ptrdiff_t& movement) 
		{ 
			pos_ += movement; 
			return (*this); 
		}
		CircularBufferIterator<T>&                  operator-=(const ptrdiff_t& movement) 
		{ 
			pos_ -= movement; 
			return (*this); 
		}
		CircularBufferIterator<T>&                  operator++() 
		{ 
			++pos_; 
			return (*this); 
		}
		CircularBufferIterator<T>&                  operator--() 
		{ 
			--pos_; return (*this); 
		}
		CircularBufferIterator<T>                   operator++(int) 
		{ 
			auto temp(*this); 
			++pos_; 
			return std::move(temp);
		}
		CircularBufferIterator<T>                   operator--(int) 
		{ 
			auto temp(*this); 
			--pos_; 
			return std::move(temp);
		}
		CircularBufferIterator<T>                   operator+(const ptrdiff_t& movement) 
		{ 
			auto oldPos = pos_; 
			pos_ += movement; 
			auto temp(*this); 
			pos_ = oldPos; 
			return std::move(temp);
		}
		CircularBufferIterator<T>                   operator-(const ptrdiff_t& movement) const
		{ 
			auto temp(*this);
			temp.pos_ -= movement;
			return std::move(temp); 
		}

		ptrdiff_t									operator-(const CircularBufferIterator<T>& other) const
		{
			return static_cast<ptrdiff_t>(pos_) - static_cast<ptrdiff_t>(other.pos_);
		}

		T&											operator*() 
		{ 
			return buffer_->Get(pos_);
		}
		const T&									operator*() const 
		{ 
			return buffer_->Get(pos_); 
		}
		T*											operator->() 
		{ 
			return buffer_ ? &buffer_->Get(pos_) : nullptr;
		}
		const T*									operator->() const
		{
			return buffer_ ? &buffer_->Get(pos_) : nullptr;
		}

		T&											operator[](ptrdiff_t movement)
		{
			return buffer_->Get(pos_+ movement);
		}
		const T&									operator[](ptrdiff_t movement) const
		{
			return buffer_->Get(pos_+ movement);
		}
	};
}
