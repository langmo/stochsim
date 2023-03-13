#pragma once
#include <memory>
#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <unordered_map>
#include <cmath>

namespace expression
{
	class IExpression;

	typedef double number;
	typedef std::string identifier;
	/// <summary>
	/// Canonical representation of the boolean value false as a number type. All numbers not equal to number_false are considered to be true.
	/// </summary>
	constexpr number number_false = 0;
	/// <summary>
	/// Canonical representation of the boolean value true as a number type. While this is the canonical representation of true, note that all numbers not equal to number_false are considered to be true, too.
	/// </summary>
	constexpr number number_true = 1;
	inline bool isTrue(const number& value)
	{
		return value != number_false;
	}
	inline bool isTrue(number&& value)
	{
		return value != number_false;
	}
	inline bool isFalse(const number& value)
	{
		return value == number_false;
	}
	inline bool isFalse(number&& value)
	{
		return value == number_false;
	}
		
	/// <summary>
	/// Template to check if a certain variadic parameter pack of types are all numbers.
	/// Can be used e.g. in static_assert by checking the value member of is_numbers.
	/// Usage:
	/// is_numbers&lt;&gt;::value == true
	/// is_numbers&lt;number&gt;::value == true
	/// is_numbers&lt;int&gt;::value == true
	/// is_numbers&lt;number, number, number&gt;::value == true
	/// is_numbers&lt;number, int, number&gt;::value == false
	/// </summary>
	template <typename... Args> struct IsNumbers : std::integral_constant<bool, true>
	{
	};
	/// <summary>
	/// Template to check if a certain variadic parameter pack of types are all numbers.
	/// Can be used e.g. in static_assert by checking the value member of is_numbers.
	/// Usage:
	/// is_numbers&lt;&gt;::value == true
	/// is_numbers&lt;number&gt;::value == true
	/// is_numbers&lt;int&gt;::value == true
	/// is_numbers&lt;number, number, number&gt;::value == true
	/// is_numbers&lt;number, int, number&gt;::value == false
	/// </summary>
	template<typename Arg, typename... Args> struct IsNumbers<Arg, Args...> : std::integral_constant<bool, std::is_same<number, typename std::remove_cv<Arg>::type>::value && IsNumbers<Args...>::value>
	{
	};
	/// <summary>
	/// Template used to hold a parameter pack representing a range of indices.
	/// </summary>
	template< std::size_t... Ns > struct Indices
	{
		typedef Indices< Ns..., sizeof...(Ns) > next;
	};

	/// <summary>
	/// Template to generate a template parameter pack representing the numbers from 0 to N-1.
	/// Specifically, calling the function 
	/// template&let;size_t... Is&gt; void foo(indices&lt;Is...&gt;);
	/// like this
	/// foo(typename make_indices&lt;3&gt;::type());
	/// results in Is... expanding to 0, 1 and 2, which can e.g. be used to call some function with parameters stored in a vector, e.g.
	/// 
	/// void my_fun(number n1, number n2, number n3)
	/// {
	///		// do something.
	/// }
	/// template&let;size_t... Is&gt; void call_my_fun(const std::vector<number>& args, indices&lt;Is...&gt;)
	/// {
	///		myFun(args[Is]...);
	/// }
	/// std::vector<number> args = { 1,2,3 };
	/// call_my_fun(args, typename make_indices<sizeof...(Args)>::type());
	/// </summary>
	template< std::size_t N > struct MakeIndices
	{
		typedef typename MakeIndices< N - 1 >::type::next type;
	};
	template<> struct MakeIndices< 0 >
	{
		typedef Indices<> type;
	};

	/// <summary>
	/// The purpose of this class is to be able to hold a pointer to spezializations of this class which represent
	/// functions with variable numbers of arguments, and to call these functions at runtime with a vector of arguments
	/// which might or might not have the correct number of arguments. If the number of arguments in the vector is correct, the function is called, 
	/// otherwise an std::exception is raised.
	/// Example:
	/// std::function&lt;number(number, number)&gt; sum_fun = [](number a1, number a2)->number {return a1 + a2; };
	/// std::unique_ptr&lt;function_holder_base&gt; sum_holder= make_function_holder(sum_fun);
	/// // later...
	/// std::vector&lt;number&gt; numbers = { 2,5};
	/// number sum = (*sum_holder)(numbers); // sum is now 7.
	/// </summary>
	class IFunctionHolder
	{
	public:
		/// <summary>
		/// Virtual destructor.
		/// </summary>
		virtual ~IFunctionHolder()
		{
		}
		/// <summary>
		/// Checks if the supplied vector of arguments has the same size as the number of arguments expected by the function handled by this class.
		/// If this is true, it calls this function with the supplied arguments. If false, a std::exception is raised.
		/// </summary>
		/// <param name="arguments">Parameters to call the handled function with.</param>
		/// <returns>The result of calling the function with the respective parameters.</returns>
		virtual number Eval(const std::vector<number>& arguments) const = 0;
		/// <summary>
		/// Checks if the supplied vector of arguments has the same size as the number of arguments expected by the function handled by this class.
		/// If this is true, it calls this function with the supplied arguments. If false, a std::exception is raised.
		/// </summary>
		/// <param name="arguments">Parameters to call the handled function with.</param>
		/// <returns>The result of calling the function with the respective parameters.</returns>
		virtual number operator()(const std::vector<number>& arguments) const final
		{
			return Eval(arguments);
		}
		/// <summary>
		/// Returns the number of parameters expected by the function hold by this class.
		/// </summary>
		/// <returns></returns>
		virtual size_t GetNumArguments() const noexcept = 0;
		/// <summary>
		/// Creates a one to one copy of this object.
		/// </summary>
		/// <returns>Copy of this object.</returns>
		virtual std::unique_ptr<IFunctionHolder> Clone() const = 0;
		/// <summary>
		/// Returns false if the function is guaranteed to always return the same value whenever called with the same arguments. Returns false otherwise.
		/// </summary>
		/// <returns>True if value might be different when called with the same arguments at different times, false if the result is guaranteed to always be the same.</returns>
		virtual bool IsMutable() const noexcept = 0;
	};
	/// <summary>
	/// Specialization of function_holder_base to hold a function having a specific number of arguments, parametrized by Args.
	/// Usually, this specialization is not used directly, but the function make_function_holder is used to create a transparent handler for a function.
	/// </summary>
	template<typename ...Args> class FunctionHolderImpl : public IFunctionHolder
	{
	public:
		/// <summary>
		/// Creates a function holder for the given function, which simply stores the function.
		/// </summary>
		/// <param name="function">Function to be stored.</param>
		FunctionHolderImpl(std::function<number(Args...)>&& function, bool mutableFunction) : function_(std::move(function)), mutableFunction_(mutableFunction)
		{
		}
		FunctionHolderImpl(const std::function<number(Args...)>& function, bool mutableFunction) : function_(function), mutableFunction_(mutableFunction)
		{
		}
		virtual std::unique_ptr<IFunctionHolder> Clone() const override
		{
			return std::unique_ptr<IFunctionHolder>(new FunctionHolderImpl<Args...>(function_, mutableFunction_));
		}
		virtual number Eval(const std::vector<number>& arguments) const override
		{
			if (arguments.size() != sizeof...(Args))
			{
				std::stringstream errorMessage;
				errorMessage << "Wrong number of arguments (expected "<< sizeof...(Args) << ", found " << arguments.size() <<").";
				throw std::runtime_error(errorMessage.str().c_str());
			}
			return CallHelper(arguments, typename MakeIndices<sizeof...(Args)>::type());
		}
		virtual size_t GetNumArguments() const noexcept override
		{
			return static_cast<size_t>(sizeof...(Args));
		}
		virtual bool IsMutable() const noexcept override
		{
			return mutableFunction_;
		}

	private:
		std::function<number(Args...)> function_;
		bool mutableFunction_;
		template<size_t... Is> number CallHelper(const std::vector<number>& args, Indices<Is...>) const
		{
			// expand the indices pack Is, resulting in the function being called with args[0], args[1], ..., args[sizeof...(Args)-1].
			return function_(args[Is]...); 
		}
	};
	/// <summary>
	/// Takes a function, e.g. a lambda, which takes an arbitrary amount of numbers as arguments and returns a number. Then, it returns a pointer
	/// to a a class storing this function. Importantly, in this process the number of arguments with thich the original function must be called becomes hidden,
	/// such that the pointer can be easily stored and passed around. Having this pointer, the number of arguments can be obtained by calling 
	/// function_holder_base::num_arguments();
	/// The original function can be called by using a vector of these arguments by calling the operator() implementation.
	/// Example: 
	/// std::function&lt;number(number, number)&gt; sum_fun = [](number a1, number a2)->number {return a1 + a2; };
	/// std::unique_ptr&lt;function_holder_base&gt; sum_holder= make_function_holder(sum_fun);
	/// // later...
	/// std::vector&lt;number&gt; numbers = { 2,5};
	/// number sum = (*sum_holder)(numbers); // sum is now 7.
	/// </summary>
	/// <param name="function">A function with various arguments whose number of arguments should be made transparent.</param>
	/// <param name="mutableFunction">False if the function is guaranteed to always return the same value when called with the same argument (e.g. max). True otherwise (e.g. rand).</param>
	/// <returns>A pointer to handler for this function, where the number of arguments of the original function became transparent.</returns>
	template<typename ...Args> std::unique_ptr<IFunctionHolder> makeFunctionHolder(std::function<number(Args...)> function, bool mutableFunction)
	{
		static_assert(IsNumbers<Args...>::value, "All function arguments must be of type stochsim::expression::number.");
		return std::unique_ptr<IFunctionHolder>(new FunctionHolderImpl<Args...>(std::move(function), mutableFunction));
	}

	/// <summary>
	/// Takes a function, e.g. a lambda, which takes an arbitrary amount of numbers as arguments and returns a number. Then, it returns a pointer
	/// to a a class storing this function. Importantly, in this process the number of arguments with thich the original function must be called becomes hidden,
	/// such that the pointer can be easily stored and passed around. Having this pointer, the number of arguments can be obtained by calling 
	/// function_holder_base::num_arguments();
	/// The original function can be called by using a vector of these arguments by calling the operator() implementation.
	/// Example: 
	/// std::function&lt;number(number, number)&gt; sum_fun = [](number a1, number a2)->number {return a1 + a2; };
	/// std::unique_ptr&lt;function_holder_base&gt; sum_holder= make_function_holder(sum_fun);
	/// // later...
	/// std::vector&lt;number&gt; numbers = { 2,5};
	/// number sum = (*sum_holder)(numbers); // sum is now 7.
	/// </summary>
	/// <param name="function">A function with various arguments whose number of arguments should be made transparent.</param>
	/// <param name="mutableFunction">False if the function is guaranteed to always return the same value when called with the same argument (e.g. max). True otherwise (e.g. rand).</param>
	/// <returns>A pointer to handler for this function, where the number of arguments of the original function became transparent.</returns>
	template<typename ...Args> std::unique_ptr<IFunctionHolder> makeFunctionHolder(number (*function)(Args...), bool mutableFunction)
	{
		static_assert(IsNumbers<Args...>::value, "All function arguments must be of type stochsim::expression::number.");
		std::function<number(Args...)> temp = function;
		return std::unique_ptr<IFunctionHolder>(new FunctionHolderImpl<Args...>(std::move(temp), mutableFunction));
	}

	/// <summary>
	/// Takes the name of a variable and returns its expression, for example a number wrapped in a number_expression. 
	/// Returns nullptr if variable with given name is not known/not in the register.
	/// </summary>
	typedef std::function<std::unique_ptr<IExpression> (const identifier variableName)> VariableRegister;
	/// <summary>
	/// Returns a default variable register which always returns a nullptr for every querry.
	/// </summary>
	/// <returns>Empty variable register</returns>
	inline const VariableRegister makeEmptyVariableRegister()
	{
		return [](const identifier variableName) ->std::unique_ptr<IExpression>
		{
			return nullptr;
		};
	}
	/// <summary>
	/// Takes the name of a variable or function and returns its binding, a function handler which can be evaluated to return the value of the variable or function.
	/// For variables, the name has to correspond to the name of the variable, whereas for functions, the name has to be suceeded by a an opening and closing round bracket, without spaces
	/// (independently of the number of arguments of the function). 
	/// Returns nullptr if variable or function with given name is not known/not in the register. 
	/// To store the handler, call clone() on the returned pointer.
	/// </summary>
	typedef std::function<std::unique_ptr<IFunctionHolder> (const identifier name)> BindingRegister;
		
	/// <summary>
	/// Abstract base class of all expressions.
	/// </summary>
	class IExpression
	{
	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~IExpression()
		{
		}
		/// <summary>
		/// Evaluates the expression. If an error occurs, e.g. because this expression or a child expression corresponds to a variable
		/// or function which is not bound, throws an std::exception.
		/// </summary>
		/// <param name="lookup">Function to lookup the value of variables based on their name.</param>
		/// <returns>Value of expression</returns>
		virtual number Eval() const = 0;
		/// <summary>
		/// Creates a clone/copy of this expression.
		/// </summary>
		/// <returns>Copy of this expression</returns>
		virtual std::unique_ptr<IExpression> Clone() const = 0;
		/// <summary>
		/// Creates a new expression which is a simplified version of the current expression. For this, all variables defined in the register
		/// are replaced by their corresponding expressions. It is guaranteed that the new expression, and all of its subexpressions, do not contain
		/// variables defined in the register. Variables not defined in the register are kept as name references.
		/// This function may throw a std::exception if the simplification is mathematically incorrect, e.g. for division by zero errors and similar.
		/// </summary>
		/// <param name="variableRegister">A register for the variables which should be replaced by their corresponding expressions.</param>
		/// <returns>A simplified version of this expression. If simplification is not possible, returns a clone/copy of this expression.</returns>
		virtual std::unique_ptr<IExpression> Simplify(const VariableRegister& variableRegister) const = 0;
		/// <summary>
		/// Creates a new expression which is a simplified version of the current expression.
		/// This function may throw a std::exception if the simplification is mathematically incorrect, e.g. for division by zero errors and similar.
		/// </summary>
		/// <returns>A simplified version of this expression. If simplification is not possible, returns a clone/copy of this expression.</returns>
		virtual std::unique_ptr<IExpression> Simplify() const
		{
			return Simplify(makeEmptyVariableRegister());
		}
		/// <summary>
		/// Binds all variables or functions to their corresponding function bindings which are used to evaluate their expression when evaluated. Variables or functions not 
		/// defined in the register are not bound.
		/// </summary>
		/// <param name="bindingRegister">Register to determine binding function given a function or variable name.</param>
		virtual void Bind(const BindingRegister& bindingRegister) = 0;

		/// <summary>
		/// Prints a string representation in CMDL of this expression to the stream.
		/// If subExpression is true, the expression is used as part of a larger expression. In this case, the string representation is sourrounded by brackets, except if there
		/// is no possibility that the expression might be differently interpreted in larger expressions.
		/// </summary>
		/// <param name="stream">Stream to print representation to.</param>
		/// <param name="subExpression">True if expression is part of a larger expression and should be escaped in brackets if necessary.</param>
		/// <returns>Reference to provided stream.</returns>
		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept = 0;

		virtual identifier ToCmdl() const noexcept final
		{
			std::ostringstream stream;
			PrintCmdl(stream, false);
			return stream.str();
		}
	};
	 
	/// <summary>
	/// Prints the a description of the expression in cmdl to the stream.
	/// </summary>
	/// <param name="stream">Stream to print on.</param>
	/// <param name="expression">Expression of which to print cmdl description.</param>
	/// <returns>The stream.</returns>
	inline std::ostream& operator<<(std::ostream &stream, const IExpression& expression) noexcept
	{
		expression.PrintCmdl(stream, false);
		return stream;
	}
		
	/// <summary>
	/// An variadic expression is an expression which contains a variable number of sub-expressions, which are referred to as its elements.
	/// Examples for variadic expressions are sums, products and similar.
	/// Each element can thereby either represent itself or its inverse, indicated by the return values of is_inverse and is_not_inverse. 
	/// Thereby, the inverse is defined with respect to the mathematical function
	/// realized by the specific variadic expression.
	/// Examples:
	/// - In a sum, being NOT the inverse e.g. means that the value of the element should be added, while being the inverse means that it should be substracted.
	/// - In a product, being NOT the inverse e.g. means that the value of the element should be multiplied, and being the inverse means that it should be divided by.
	/// - In a logical expression, being the inverse e.g. means that the value of the element should be treated as its logical compliment, i.e. whenever the element evaluates to true the value should be false and vice versa.
	/// </summary>
	class IVariadicExpression : public IExpression
	{
	public:
		class Element
		{
		private:
			std::unique_ptr<IExpression> expression_;
			bool inverse_;
		public:
			Element(bool inverse, std::unique_ptr<IExpression> expression) : expression_(std::move(expression)), inverse_(inverse)
			{
			}
			Element(const Element& other) : inverse_(other.inverse_), expression_(other.expression_->Clone())
			{
			}
			Element& operator= (Element other)
			{
				expression_ = std::move(other.expression_);
				inverse_ = other.inverse_;
				return *this;
			}
			/// <summary>
			/// Returns a pointer to the expression of this element.
			/// This expression is still owned and memory managed by this object.
			/// </summary>
			/// <returns>Pointer to the expression</returns>
			IExpression* GetExpression() noexcept
			{
				return expression_.get();
			}
			/// <summary>
			/// Returns a pointer to the expression of this element.
			/// This expression is still owned and memory managed by this object.
			/// </summary>
			/// <returns>Pointer to the expression</returns>
			const IExpression* GetExpression() const noexcept
			{
				return expression_.get();
			}
			/// <summary>
			/// Returns a boolean value indicating if the element should be interpreted as its inverse. Thereby, the inverse is defined with respect to the mathematical function
			/// realized by the specific variadic expression.
			/// Examples:
			/// - In a sum, being NOT the inverse e.g. means that the value of the element should be added, while being the inverse means that it should be substracted.
			/// - In a product, being NOT the inverse e.g. means that the value of the element should be multiplied, and being the inverse means that it should be divided by.
			/// /// - In a logical expression, being the inverse e.g. means that the value of the element should be treated as its logical compliment, i.e. whenever the element evaluates to true the value should be false and vice versa.
			/// </summary>
			/// <returns>True if the element should be treated as its inverse, false if it should be treated normally.</returns>
			bool IsInverse() const
			{
				return inverse_;
			}

			/// <summary>
			/// The opposite of is_inverse, i.e. is_not_inverse() == !is_inverse().
			/// See is_inverse for further details.
			/// </summary>
			/// <returns>False if the element should be treated as its inverse, true if it should be treated normally.</returns>
			bool IsNotInverse() const
			{
				return !inverse_;
			}
		};
	public:
		typedef Element value_type;
		typedef std::vector<value_type> collection_type;
		typedef collection_type::size_type size_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef collection_type::iterator iterator;
		typedef collection_type::const_iterator const_iterator;

	public:
		IVariadicExpression(number baseValue) : baseValue_(baseValue)
		{
		}

		// Element access, iterators, ...
		void PushBack(bool inverse, std::unique_ptr<IExpression> expression)
		{
			elems_.push_back(Element(inverse, std::move(expression)));
		}
		value_type PopBack()
		{
			Element elem(std::move(elems_.back()));
			elems_.pop_back();
			return std::move(elem);
		}
		reference PeekBack()
		{
			return elems_.back();
		}
		const_reference PeekBack() const
		{
			return elems_.back();
		}
		bool IsEmpty() const noexcept
		{
			return elems_.empty();
		}
		size_type GetSize() const noexcept
		{
			return elems_.size();
		}
		reference operator[](size_type pos)
		{
			return elems_[pos];
		}
		const_reference operator[](size_type pos) const
		{
			return elems_[pos];
		}
		iterator begin() noexcept
		{
			return elems_.begin();
		}
		const_iterator begin() const noexcept
		{
			return elems_.begin();
		}
		const_iterator cbegin() const noexcept
		{
			return elems_.cbegin();
		}
		iterator end() noexcept
		{
			return elems_.end();
		}
		const_iterator end() const noexcept
		{
			return elems_.end();
		}
		const_iterator cend() const noexcept
		{
			return elems_.cend();
		}
		virtual void Bind(const BindingRegister& bindingRegister)
		{
			for (auto& elem : elems_)
			{
				elem.GetExpression()->Bind(bindingRegister);
			}
		}
	public:
		/// <summary>
		/// Function returning the value of the null element of the respective variadic function. For example, the null element for a sum
		/// is 0 since 0+a=a+0=a. For a product, it's 1, since 1*a=a*1=a. For a boolean it's number_true, since number_true &amp;&amp; a = a &amp;&amp; number_true = a.
		/// Typically, the base value of a variadic element is initially set to the respective null element.
		/// </summary>
		/// <returns>Null element of the variadic function.</returns>
		virtual number GetNullElement() const noexcept = 0;
		/// <summary>
		/// Function returning the unary inverse of the respective expression, for example -expression in the case of a sum.
		/// </summary>
		/// <returns>Unary inverse of expression.</returns>
		virtual std::unique_ptr<IExpression> GetUnaryInverse(const IExpression* expression) const = 0;
	protected:
		std::vector<Element> elems_;
		number baseValue_;
	};

	std::unordered_map<identifier, number> makeDefaultVariables() noexcept;

	std::unordered_map<identifier, std::unique_ptr<IFunctionHolder>> makeDefaultFunctions() noexcept;
}