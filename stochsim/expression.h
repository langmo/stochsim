#pragma once
#include <memory>
#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <map>
#include <cmath>
namespace stochsim
{
	namespace expression
	{
		class expression_base;
		class number_expression;
		class variable_expression;

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
		inline bool is_true(const number& value)
		{
			return value != number_false;
		}
		inline bool is_true(number&& value)
		{
			return value != number_false;
		}
		inline bool is_false(const number& value)
		{
			return value == number_false;
		}
		inline bool is_false(number&& value)
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
		template <typename... Args> struct is_numbers : std::integral_constant<bool, true>
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
		template<typename Arg, typename... Args> struct is_numbers<Arg, Args...> : std::integral_constant<bool, std::is_same<number, typename std::remove_cv<Arg>::type>::value && is_numbers<Args...>::value>
		{
		};
		/// <summary>
		/// Template used to hold a parameter pack representing a range of indices.
		/// </summary>
		template< std::size_t... Ns > struct indices
		{
			typedef indices< Ns..., sizeof...(Ns) > next;
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
		template< std::size_t N > struct make_indices
		{
			typedef typename make_indices< N - 1 >::type::next type;
		};
		template<> struct make_indices< 0 >
		{
			typedef indices<> type;
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
		class function_holder_base
		{
		public:
			/// <summary>
			/// Virtual destructor.
			/// </summary>
			virtual ~function_holder_base()
			{
			}
			/// <summary>
			/// Checks if the supplied vector of arguments has the same size as the number of arguments expected by the function handled by this class.
			/// If this is true, it calls this function with the supplied arguments. If false, a std::exception is raised.
			/// </summary>
			/// <param name="arguments">Parameters to call the handled function with.</param>
			/// <returns>The result of calling the function with the respective parameters.</returns>
			virtual number operator()(const std::vector<number>& arguments) = 0;
			/// <summary>
			/// Returns the number of parameters expected by the function hold by this class.
			/// </summary>
			/// <returns></returns>
			virtual size_t num_arguments() const noexcept = 0;
			/// <summary>
			/// Creates a one to one copy of this object.
			/// </summary>
			/// <returns>Copy of this object.</returns>
			virtual std::unique_ptr<function_holder_base> clone() const = 0;
			/// <summary>
			/// Returns false if the function is guaranteed to always return the same value whenever called with the same arguments. Returns false otherwise.
			/// </summary>
			/// <returns>True if value might be different when called with the same arguments at different times, false if the result is guaranteed to always be the same.</returns>
			virtual bool is_mutable() const noexcept = 0;
		};
		/// <summary>
		/// Specialization of function_holder_base to hold a function having a specific number of arguments, parametrized by Args.
		/// Usually, this specialization is not used directly, but the function make_function_holder is used to create a transparent handler for a function.
		/// </summary>
		template<typename ...Args> class function_holder : function_holder_base
		{
		public:
			/// <summary>
			/// Creates a function holder for the given function, which simply stores the function.
			/// </summary>
			/// <param name="function">Function to be stored.</param>
			function_holder(std::function<number(Args...)>&& function, bool mutableFunction) : function_(std::move(function)), mutableFunction_(mutableFunction)
			{
			}
			function_holder(const std::function<number(Args...)>& function, bool mutableFunction) : function_(function), mutableFunction_(mutableFunction)
			{
			}
			virtual std::unique_ptr<function_holder_base> clone() const override
			{
				return std::unique_ptr<function_holder_base>(new function_holder<Args...>(function_, mutableFunction_));
			}
			virtual number operator()(const std::vector<number>& arguments) override
			{
				if (arguments.size() != sizeof...(Args))
					throw std::exception("Wrong number of arguments.");
				return call_helper(arguments, typename make_indices<sizeof...(Args)>::type());
			}
			virtual size_t num_arguments() const noexcept override
			{
				return static_cast<size_t>(sizeof...(Args));
			}
			virtual bool is_mutable() const noexcept
			{
				return mutableFunction_;
			}

		private:
			std::function<number(Args...)> function_;
			bool mutableFunction_;
			template<size_t... Is> number call_helper(const std::vector<number>& args, indices<Is...>)
			{
				return function_(args[Is]...); // expand the indices pack
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
		template<typename ...Args> std::unique_ptr<function_holder_base> make_function_holder(std::function<number(Args...)> function, bool mutableFunction)
		{
			static_assert(is_numbers<Args...>::value, "All function arguments must be of type stochsim::expression::number.");
			return std::unique_ptr<function_holder_base>(new function_holder<Args...>(std::move(function), mutableFunction));
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
		template<typename ...Args> std::unique_ptr<function_holder_base> make_function_holder(number (*function)(Args...), bool mutableFunction)
		{
			static_assert(is_numbers<Args...>::value, "All function arguments must be of type stochsim::expression::number.");
			std::function<number(Args...)> temp = function;
			return std::unique_ptr<function_holder_base>(new function_holder<Args...>(std::move(temp), mutableFunction));
		}

		/// <summary>
		/// Takes the name of a variable and returns its expression, for example a number wrapped in a number_expression. 
		/// Throws a std::exception if a variable with the given name does not
		/// exist. If variables are directly saved by value, a corresponding number_expression is returned.
		/// </summary>
		typedef std::function<std::unique_ptr<expression_base> (const identifier variableName)> variable_lookup;
		/// <summary>
		/// Returns a default variable lookup which always throws an error for every querry.
		/// </summary>
		/// <returns>Empty variable lookup</returns>
		inline const variable_lookup get_empty_variable_lookup()
		{
			return [](const identifier variableName) ->std::unique_ptr<expression_base>
			{
				std::stringstream errorMessage;
				errorMessage << "Variable with name \"" << variableName << "\" is not defined.";
				throw std::exception(errorMessage.str().c_str());
			};
		}
		/// <summary>
		/// Takes the name of a variable or function and returns its binding, a function handler which can be evaluated to return the value of the variable or function.
		/// For variables, the name has to correspond to the name of the variable, whereas for functions, the name has to be suceeded by a an opening and closing round bracket, without spaces
		/// (independently of the number of arguments of the function). 
		/// Throws a std::exception if a variable with the given name does not
		/// exist. 
		/// To store the handler, call clone() on the returned pointer.
		/// </summary>
		typedef std::function<std::unique_ptr<function_holder_base> (const identifier name)> binding_lookup;
		
		/// <summary>
		/// Abstract base class of all expressions.
		/// </summary>
		class expression_base
		{
		public:
			/// <summary>
			/// Destructor.
			/// </summary>
			virtual ~expression_base()
			{
			}
			/// <summary>
			/// Evaluates the expression. If an error occurs, e.g. because this expression or a child expression corresponds to a variable
			/// or function which is not bound, throws an std::exception.
			/// </summary>
			/// <param name="lookup">Function to lookup the value of variables based on their name.</param>
			/// <returns>Value of expression</returns>
			virtual number eval() const = 0;
			/// <summary>
			/// Creates a clone/copy of this expression.
			/// </summary>
			/// <returns>Copy of this expression</returns>
			virtual std::unique_ptr<expression_base> clone() const = 0;
			/// <summary>
			/// Creates a new expression which is a simplified version of the current expression. For this, all variables defined in the lookup
			/// are replaced by their corresponding expressions. It is guaranteed that the new expression, and all of its subexpressions, do not contain
			/// variables defined in the partial lookup. Variables not defined as a partial lookup are kept as name references. For this function, the simplest
			/// possible return value is considered to be a number_expression.
			/// This function may throw a std::exception if the simplification is mathematically incorrect, e.g. for division by zero errors and similar.
			/// </summary>
			/// <param name="variableLookup">A (partial) lookup for the variables which should be replaced by their corresponding expressions.</param>
			/// <returns>A simplified version of this expression. If simplification is not possible, returns a clone/copy of this expression.</returns>
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const = 0;
			/// <summary>
			/// Creates a new expression which is a simplified version of the current expression.
			/// This function may throw a std::exception if the simplification is mathematically incorrect, e.g. for division by zero errors and similar.
			/// </summary>
			/// <returns>A simplified version of this expression. If simplification is not possible, returns a clone/copy of this expression.</returns>
			virtual std::unique_ptr<expression_base> simplify() const
			{
				return simplify(get_empty_variable_lookup());
			}
			/// <summary>
			/// Binds all variables or functions to their corresponding function bindings which are used to evaluate their expression when evaluated. Variables or functions not 
			/// defined in the binding lookup are not bound.
			/// </summary>
			/// <param name="bindingLookup">Lookup to determine binding function given a function or variable name.</param>
			virtual void bind(const binding_lookup& bindingLookup) = 0;

			/// <summary>
			/// Prints a string representation in CMDL of this expression to the stream.
			/// </summary>
			/// <param name="o">Stream to print representation to.</param>
			/// <returns>Reference to provided stream.</returns>
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept = 0;

			virtual identifier toCmdl() const noexcept
			{
				std::ostringstream stream;
				printCmdl(stream);
				return stream.str();
			}
		};
	 
		/// <summary>
		/// Prints the a description of the expression in cmdl to the stream.
		/// </summary>
		/// <param name="stream">Stream to print on.</param>
		/// <param name="expression">Expression of which to print cmdl description.</param>
		/// <returns>The stream.</returns>
		inline std::ostream& operator<<(std::ostream &stream, const expression_base& expression) noexcept
		{
			return expression.printCmdl(stream);
		}
		/// <summary>
		/// Helper class to automatically print brackets around an equation if necessary.
		/// Usage:
		/// stream &lt;&lt; print_in_brackets(expression_base*);
		/// </summary>
		class print_in_brackets final
		{
		public:
			print_in_brackets(const expression_base* expression) noexcept: expression_(expression)
			{
			}
			print_in_brackets(const std::unique_ptr<const expression_base>& expression) noexcept : expression_(expression.get())
			{
			}
			print_in_brackets(const std::unique_ptr<expression_base>& expression) noexcept : expression_(expression.get())
			{
			}
		private:
			const expression_base* expression_;
			// Allow only stream function to do something useful
			friend std::ostream& operator<<(std::ostream &stream, const print_in_brackets&& expression) noexcept;
			// Prevent heap allocation and assignemnt
			static void *operator new     (size_t) = delete;
			static void *operator new[](size_t) = delete;
			static void  operator delete  (void*) = delete;
			static void  operator delete[](void*) = delete;
			print_in_brackets & operator=(const print_in_brackets&) = delete;
			print_in_brackets(const print_in_brackets&) = delete;
			print_in_brackets() = default;
		};
		
		/// <summary>
		/// A number expression is the simplest type of an expression, i.e. a constant number.
		/// </summary>
		class number_expression : public expression_base
		{
		public:
			number_expression(number number) : number_(number)
			{
			}
			virtual number eval() const override
			{
				return number_;
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				return std::make_unique<number_expression>(number_);
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				// we are already the most simple possible expression...
				return clone();
			}
			number get_number() const noexcept
			{
				return number_;
			}
			operator number() const noexcept
			{
				return number_;
			}
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				return stream << number_;
			}
			virtual void bind(const binding_lookup& bindingLookup)
			{
				// do nothing
			}
		private:
			number number_;
		};
		/// <summary>
		/// A variable expression represents the value of the corresponding variable.
		/// </summary>
		class variable_expression : public expression_base
		{
		public:
			variable_expression(identifier name, std::unique_ptr<function_holder_base> evalFunction=nullptr) : name_(name), evalFunction_(std::move(evalFunction))
			{
			}
			virtual number eval() const override
			{
				if (evalFunction_)
				{
					std::vector<number> empty;
					return evalFunction_->operator()(empty);
				}
				std::stringstream errorMessage;
				errorMessage << "Expression contains unbound variable with name \"" << name_ << "\".";
				throw std::exception(errorMessage.str().c_str());
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				try
				{
					return variableLookup(name_)->simplify(variableLookup);
				}
				catch (...)
				{
					// do nothing, means variable is not defined in lookup.
				}
				if (evalFunction_ && !evalFunction_->is_mutable())
				{
					std::vector<number> empty;
					return std::make_unique<number_expression>(evalFunction_->operator()(empty));
				}
				return clone();
			}
			identifier get_identifier() const noexcept
			{
				return name_;
			}
			operator identifier() const noexcept
			{
				return name_;
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				return std::make_unique<variable_expression>(name_, evalFunction_ ? evalFunction_->clone() : nullptr);
			}
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				return stream << name_;
			}
			virtual void bind(const binding_lookup& bindingLookup)
			{
				try
				{
					evalFunction_ = bindingLookup(name_);
				}
				catch (...)
				{
					// do nothing. There is simply no binding for this variable...
				}
			}
		private:
			identifier name_;
			std::unique_ptr<function_holder_base> evalFunction_;
		};

		/// <summary>
		/// Prints the a description of the expression in cmdl to the stream, surrounding it by brackets if necessary.
		/// </summary>
		/// <param name="stream">Stream to print on.</param>
		/// <param name="expression">Expression of which to print cmdl description.</param>
		/// <returns>The stream.</returns>
		inline std::ostream& operator<<(std::ostream &stream, const print_in_brackets&& expression) noexcept
		{
			if (!dynamic_cast<const number_expression*>(expression.expression_) && !dynamic_cast<const variable_expression*>(expression.expression_))
				return stream << "(" << *expression.expression_ << ")";
			else
				return stream << *expression.expression_;

		}

		/// <summary>
		/// A conditional expression can represent two different sub-expression, depending on if the condition evaluates to true of false.
		/// Thus, it can be interpreted to represent:
		/// conditional_expression = condition ? expressionIfTrue : expressionIfFalse;
		/// </summary>
		class conditional_expression : public expression_base
		{
		public:
			conditional_expression(std::unique_ptr<expression_base> condition, std::unique_ptr<expression_base> expressionIfTrue, std::unique_ptr<expression_base> expressionIfFalse) :
				condition_(std::move(condition)),
				expressionIfTrue_(std::move(expressionIfTrue)),
				expressionIfFalse_(std::move(expressionIfFalse))
			{
			}
			virtual number eval() const override
			{
				if (condition_->eval() != 0)
					return expressionIfTrue_->eval();
				else
					return expressionIfFalse_->eval();
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				return std::make_unique<conditional_expression>(condition_->clone(), expressionIfTrue_->clone(), expressionIfFalse_->clone());
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				auto simpCondition = condition_->simplify(variableLookup);
				if (dynamic_cast<number_expression*>(simpCondition.get()))
				{
					if (dynamic_cast<number_expression*>(simpCondition.get())->get_number() != 0)
					{
						return expressionIfTrue_->simplify(variableLookup);
					}
					else
					{
						return expressionIfFalse_->simplify(variableLookup);
					}
				}
				else
				{
					return std::make_unique<conditional_expression>(std::move(simpCondition), expressionIfTrue_->simplify(variableLookup), expressionIfFalse_->simplify(variableLookup));
				}
			}
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				return stream << print_in_brackets(condition_) << " ? " << print_in_brackets(expressionIfTrue_) << " : "<< print_in_brackets(expressionIfFalse_);
			}
			virtual void bind(const binding_lookup& bindingLookup)
			{
				condition_->bind(bindingLookup);
				expressionIfTrue_->bind(bindingLookup);
				expressionIfFalse_->bind(bindingLookup);
			}
		private:
			std::unique_ptr<expression_base> condition_;
			std::unique_ptr<expression_base> expressionIfTrue_;
			std::unique_ptr<expression_base> expressionIfFalse_;
		};
	
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
		class variadic_expression_base : public expression_base
		{
		public:
			class element
			{
			private:
				std::unique_ptr<expression_base> expression_;
				bool inverse_;
			public:
				element(bool inverse, std::unique_ptr<expression_base> expression) : expression_(std::move(expression)), inverse_(inverse)
				{
				}
				element(const element& other) : inverse_(other.inverse_), expression_(other.expression_->clone())
				{
				}
				element& operator= (element other)
				{
					expression_ = std::move(other.expression_);
					inverse_ = other.inverse_;
				}
				/// <summary>
				/// Returns a pointer to the expression of this element.
				/// This expression is still owned and memory managed by this object.
				/// </summary>
				/// <returns>Pointer to the expression</returns>
				expression_base* get_expression() noexcept
				{
					return expression_.get();
				}
				/// <summary>
				/// Returns a pointer to the expression of this element.
				/// This expression is still owned and memory managed by this object.
				/// </summary>
				/// <returns>Pointer to the expression</returns>
				const expression_base* get_expression() const noexcept
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
				bool is_inverse() const
				{
					return inverse_;
				}

				/// <summary>
				/// The opposite of is_inverse, i.e. is_not_inverse() == !is_inverse().
				/// See is_inverse for further details.
				/// </summary>
				/// <returns>False if the element should be treated as its inverse, true if it should be treated normally.</returns>
				bool is_not_inverse() const
				{
					return !inverse_;
				}
			};
		public:
			typedef element value_type;
			typedef std::vector<value_type> collection_type;
			typedef collection_type::size_type size_type;
			typedef value_type& reference;
			typedef const value_type& const_reference;
			typedef collection_type::iterator iterator;
			typedef collection_type::const_iterator const_iterator;

		public:
			variadic_expression_base(number baseValue) : baseValue_(baseValue)
			{
			}

			// Element access, iterators, ...
			void push_back(bool inverse, std::unique_ptr<expression_base> expression)
			{
				elems_.push_back(element(inverse, std::move(expression)));
			}
			value_type pop_back()
			{
				element elem(std::move(elems_.back()));
				elems_.pop_back();
				return std::move(elem);
			}
			reference back()
			{
				return elems_.back();
			}
			const_reference back() const
			{
				return elems_.back();
			}
			bool empty() const noexcept
			{
				return elems_.empty();
			}
			size_type size() const noexcept
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
			virtual void bind(const binding_lookup& bindingLookup)
			{
				for (auto& elem : elems_)
				{
					elem.get_expression()->bind(bindingLookup);
				}
			}
		public:
			/// <summary>
			/// Function returning the value of the null element of the respective variadic function. For example, the null element for a sum
			/// is 0 since 0+a=a+0=a. For a product, it's 1, since 1*a=a*1=a. For a boolean it's number_true, since number_true &amp;&amp; a = a &amp;&amp; number_true = a.
			/// Typically, the base value of a variadic element is initially set to the respective null element.
			/// </summary>
			/// <returns>Null element of the variadic function.</returns>
			virtual number get_null_element() const noexcept = 0;
			/// <summary>
			/// Function returning the unary inverse of the respective expression, for example -expression in the case of a sum.
			/// </summary>
			/// <returns>Unary inverse of expression.</returns>
			virtual std::unique_ptr<expression_base> get_unary_inverse(const expression_base* expression) const = 0;
		protected:
			std::vector<element> elems_;
			number baseValue_;
		};

		/// <summary>
		/// Expression representing the (unary) minus of an expression, i.e. 
		/// minus_expression = -expression.
		/// </summary>
		class minus_expression : public expression_base
		{
		public:
			minus_expression(std::unique_ptr<expression_base> expression) :
				expression_(std::move(expression))
			{
			}
			virtual number eval() const override
			{
				return -expression_->eval();
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				return std::make_unique<minus_expression>(expression_->clone());
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				auto simpExpression = expression_->simplify(variableLookup);
				auto numExpression = dynamic_cast<number_expression*>(simpExpression.get());
				if (numExpression)
				{
					auto result = -numExpression->get_number();
					return std::make_unique<number_expression>(result);
				}
				auto minusExpression = dynamic_cast<minus_expression*>(simpExpression.get());
				if (minusExpression)
				{
					return std::move(minusExpression->expression_);
				}
				return std::make_unique<minus_expression>(std::move(simpExpression));
			}
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				return stream << "-" << print_in_brackets(expression_);
			}
			expression_base* get_expression() noexcept
			{
				return expression_.get();
			}
			const expression_base* get_expression() const noexcept
			{
				return expression_.get();
			}
			virtual void bind(const binding_lookup& bindingLookup)
			{
				expression_->bind(bindingLookup);
			}
		private:
			std::unique_ptr<expression_base> expression_;
		};
		/// <summary>
		/// Variadic expression representing the sum of various elements.
		/// The inverse of an expression is thereby -expression. Thus, sum_expressions represent both sums and differences.
		/// </summary>
		class sum_expression : public variadic_expression_base
		{
		public:
			static constexpr number null_element = 0;
		public:
			sum_expression(number baseValue = null_element) : variadic_expression_base(baseValue)
			{
			}
			virtual number get_null_element() const noexcept override
			{
				return null_element;
			}
			virtual std::unique_ptr<expression_base> get_unary_inverse(const expression_base* expression) const override
			{
				return std::make_unique<minus_expression>(expression->clone());
			}
			virtual number eval() const override
			{
				number sum = baseValue_;
				for (auto& elem : elems_)
				{
					if(elem.is_not_inverse())
						sum += elem.get_expression()->eval();
					else
						sum -= elem.get_expression()->eval();
				}
				return sum;
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				auto value = baseValue_;
				std::vector<element> simElems;
				for (auto& elem : elems_)
				{
					auto simElem = elem.get_expression()->simplify(variableLookup);
					if (dynamic_cast<number_expression*>(simElem.get()))
					{
						if (elem.is_not_inverse())
						{
							value += static_cast<number_expression*>(simElem.get())->get_number();
						}
						else
						{
							value -= static_cast<number_expression*>(simElem.get())->get_number();
						}
					}
					else if (dynamic_cast<minus_expression*>(simElem.get()))
					{
						simElems.emplace_back(elem.is_not_inverse(), static_cast<minus_expression*>(simElem.get())->get_expression()->clone());
					}
					else if (dynamic_cast<sum_expression*>(simElem.get()))
					{
						auto& subExp = *static_cast<sum_expression*>(simElem.get());
						value += elem.is_not_inverse() ? subExp.baseValue_ : -subExp.baseValue_;
						for (auto subElem : subExp)
						{
							simElems.emplace_back(elem.is_inverse() ? subElem.is_not_inverse() : subElem.is_inverse(), subElem.get_expression()->clone());
						}
					}
					else
					{
						simElems.emplace_back(elem.is_inverse(), std::move(simElem));
					}
				}
				if (simElems.empty())
					return std::make_unique<number_expression>(value);
				else if (value == 0 && simElems.size() == 1)
				{
					if (simElems[0].is_not_inverse())
						return simElems[0].get_expression()->clone();
					else
						return get_unary_inverse(simElems[0].get_expression());
				}
				else
				{
					auto returnVal = std::make_unique<sum_expression>(value);
					for (auto& elem : simElems)
						returnVal->elems_.emplace_back(elem);
					return returnVal;
				}
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				auto val = std::make_unique<sum_expression>(baseValue_);
				for (auto& elem : elems_)
					val->elems_.emplace_back(elem);
				return std::move(val);
			}

			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				bool first;
				if (baseValue_ == 0)
					first = true;
				else
				{
					stream << baseValue_;
					first = false;
				}
				for (auto& elem : elems_)
				{
					bool positive = elem.is_not_inverse();
					if (dynamic_cast<const number_expression*>(elem.get_expression()))
					{
						auto value = static_cast<const number_expression*>(elem.get_expression())->get_number();
						if (value == 0)
							continue;
						else if (value < 0)
						{
							value *= -1;
							positive = !positive;
						}
						if (first)
						{
							if (positive)
								stream << value;
							else
								stream << "-" << value;
						}
						else
						{
							if (positive)
								stream << " + " << value;
							else
								stream << " - " << value;
						}
					}
					else
					{
						if (first)
						{
							if (positive)
								stream << print_in_brackets(elem.get_expression());
							else
								stream << "-" << print_in_brackets(elem.get_expression());
						}
						else
						{
							if (positive)
								stream << " + " << print_in_brackets(elem.get_expression());
							else
								stream << " - " << print_in_brackets(elem.get_expression());
						}
					}
					first = false;
				}
				return stream;
			}
		
		};
		/// <summary>
		/// Expression representing the (unary) division of an expression, i.e. raising it to the power of -1.
		/// divide_expression = 1/expression.
		/// </summary>
		class divide_expression : public expression_base
		{
		public:
			divide_expression(std::unique_ptr<expression_base> expression) :
				expression_(std::move(expression))
			{
			}
			virtual number eval() const override
			{
				return 1/expression_->eval();
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				return std::make_unique<divide_expression>(expression_->clone());
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				auto simpExpression = expression_->simplify(variableLookup);
				auto numExpression = dynamic_cast<number_expression*>(simpExpression.get());
				if (numExpression)
				{
					auto result = 1/numExpression->get_number();
					return std::make_unique<number_expression>(result);
				}
				auto divideExpression = dynamic_cast<divide_expression*>(simpExpression.get());
				if (divideExpression)
				{
					return std::move(divideExpression->expression_);
				}
				return std::make_unique<divide_expression>(std::move(simpExpression));
			}
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				return stream << "1 / " << print_in_brackets(expression_);
			}
			expression_base* get_expression() noexcept
			{
				return expression_.get();
			}
			const expression_base* get_expression() const noexcept
			{
				return expression_.get();
			}
			virtual void bind(const binding_lookup& bindingLookup)
			{
				expression_->bind(bindingLookup);
			}
		private:
			std::unique_ptr<expression_base> expression_;
		};
		/// <summary>
		/// Variadic expression representing the product of various elements.
		/// The inverse of an expression is thereby 1/expression. Thus, product_expression-s represent both sums and ratios.
		/// </summary>
		class product_expression : public variadic_expression_base
		{
		public:
			static constexpr number null_element = 1;
		public:
			product_expression(number baseValue = null_element) : variadic_expression_base(baseValue)
			{
			}
			virtual number get_null_element() const noexcept override
			{
				return null_element;
			}
			virtual std::unique_ptr<expression_base> get_unary_inverse(const expression_base* expression) const override
			{
				return std::make_unique<divide_expression>(expression->clone());
			}
			virtual number eval() const override
			{
				number sum = baseValue_;
				for (auto& elem : elems_)
				{
					if(elem.is_not_inverse())
						sum *= elem.get_expression()->eval();
					else
						sum /= elem.get_expression()->eval();
				}
				return sum;
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				auto value = baseValue_;
				std::vector<element> simElems;
				for (auto& elem : elems_)
				{
					auto simElem = elem.get_expression()->simplify(variableLookup);
					if (dynamic_cast<number_expression*>(simElem.get()))
					{
						if (elem.is_not_inverse())
						{
							value *= static_cast<number_expression*>(simElem.get())->get_number();
						}
						else
						{
							value /= static_cast<number_expression*>(simElem.get())->get_number();
						}
					}
					else if (dynamic_cast<divide_expression*>(simElem.get()))
					{
						simElems.emplace_back(elem.is_not_inverse(), static_cast<divide_expression*>(simElem.get())->get_expression()->clone());
					}
					else if (dynamic_cast<product_expression*>(simElem.get()))
					{
						auto& subExp = *static_cast<product_expression*>(simElem.get());
						value *= elem.is_not_inverse() ? subExp.baseValue_ : 1/subExp.baseValue_;
						for (auto subElem : subExp)
						{
							simElems.emplace_back(elem.is_inverse() ? subElem.is_not_inverse() : subElem.is_inverse(), subElem.get_expression()->clone());
						}
					}
					else
					{
						simElems.emplace_back(elem.is_inverse(), std::move(simElem));
					}
				}
				if (simElems.empty()|| value == 0)
					return std::make_unique<number_expression>(value);
				else if (value == 1 && simElems.size() == 1)
				{
					if (simElems[0].is_not_inverse())
						return simElems[0].get_expression()->clone();
					else
						return get_unary_inverse(simElems[0].get_expression());
				}
				else
				{
					auto returnVal = std::make_unique<product_expression>(value);
					for (auto& elem : simElems)
						returnVal->elems_.emplace_back(elem);
					return returnVal;
				}
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				auto val = std::make_unique<product_expression>(baseValue_);
				for (auto& elem : elems_)
					val->elems_.emplace_back(elem);
				return std::move(val);
			}
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				bool first;
				if (baseValue_ == 1)
					first = true;
				else
				{
					stream << baseValue_;
					first = false;
				}
				for (auto& elem : elems_)
				{
					bool positive = elem.is_not_inverse();
					if (first)
					{
						if (positive)
							stream << print_in_brackets(elem.get_expression());
						else
							stream << "1 / " << print_in_brackets(elem.get_expression());
					}
					else
					{
						if (positive)
							stream << " * " << print_in_brackets(elem.get_expression());
						else
							stream << " / " << print_in_brackets(elem.get_expression());
					}
					first = false;
				}
				return stream;
			}
		};
		/// <summary>
		/// Expression representing the (unary) not operator for an expression, i.e.
		/// not_expression = is_false(expression).
		/// </summary>
		class not_expression : public expression_base
		{
		public:
			not_expression(std::unique_ptr<expression_base> expression) :
				expression_(std::move(expression))
			{
			}
			virtual number eval() const override
			{
				return is_true(expression_->eval()) ? number_false : number_true;
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				return std::make_unique<not_expression>(expression_->clone());
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				auto simpExpression = expression_->simplify(variableLookup);
				auto numExpression = dynamic_cast<number_expression*>(simpExpression.get());
				if (numExpression)
				{
					auto result = is_true(numExpression->get_number()) ? number_false : number_true;
					return std::make_unique<number_expression>(result);
				}
				auto notExpression = dynamic_cast<not_expression*>(simpExpression.get());
				if (notExpression)
				{
					return std::move(notExpression->expression_);
				}
				return std::make_unique<not_expression>(std::move(simpExpression));
			}
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				return stream << "!" << print_in_brackets(expression_);
			}
			expression_base* get_expression() noexcept
			{
				return expression_.get();
			}
			const expression_base* get_expression() const noexcept
			{
				return expression_.get();
			}
			virtual void bind(const binding_lookup& bindingLookup)
			{
				expression_->bind(bindingLookup);
			}
		private:
			std::unique_ptr<expression_base> expression_;
		};
		/// <summary>
		/// Variadic expression representing the logical conjunction.
		/// The inverse of an expression is thereby !expression.
		/// </summary>
		class conjunction_expression : public variadic_expression_base
		{
		public:
			static constexpr number null_element = number_true;
		public:
			conjunction_expression(number baseValue = null_element) : variadic_expression_base(baseValue)
			{
			}
			virtual number get_null_element() const noexcept override
			{
				return null_element;
			}
			virtual std::unique_ptr<expression_base> get_unary_inverse(const expression_base* expression) const override
			{
				return std::make_unique<not_expression>(expression->clone());
			}
			virtual number eval() const override
			{
				bool value = is_true(baseValue_);
				for (auto& elem : elems_)
				{
					if (!value)
						return number_false;
					if (elem.is_not_inverse())
						value &= is_true(elem.get_expression()->eval());
					else
						value &= is_false(elem.get_expression()->eval());
				}
				return value ? number_true : number_false;
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				bool value = is_true(baseValue_);
				std::vector<element> simElems;
				for (auto& elem : elems_)
				{
					if (!value)
						return std::make_unique<number_expression>(number_false);
					auto simElem = elem.get_expression()->simplify(variableLookup);
					if (dynamic_cast<number_expression*>(simElem.get()))
					{
						if (elem.is_not_inverse())
						{
							value &= is_true(static_cast<number_expression*>(simElem.get())->get_number());
						}
						else
						{
							value &= is_false(static_cast<number_expression*>(simElem.get())->get_number());
						}
					}
					else if (dynamic_cast<not_expression*>(simElem.get()))
					{
						simElems.emplace_back(elem.is_not_inverse(), static_cast<not_expression*>(simElem.get())->get_expression()->clone());
					}
					else if (elem.is_not_inverse() && dynamic_cast<conjunction_expression*>(simElem.get()))
					{
						auto& subExp = *static_cast<conjunction_expression*>(simElem.get());
						for (auto subElem : subExp)
						{
							simElems.emplace_back(subElem.is_inverse(), subElem.get_expression()->clone());
						}
					}
					else
					{
						simElems.emplace_back(elem.is_inverse(), std::move(simElem));
					}
				}
				if (!value)
					return std::make_unique<number_expression>(number_false);
				else if (simElems.empty())
					return std::make_unique<number_expression>(number_true);
				else if (simElems.size() == 1)
				{
					if (simElems[0].is_not_inverse())
						return simElems[0].get_expression()->clone();
					else
						return get_unary_inverse(simElems[0].get_expression());
				}
				else
				{
					auto returnVal = std::make_unique<conjunction_expression>(number_true);
					for (auto& elem : simElems)
						returnVal->elems_.emplace_back(elem);
					return returnVal;
				}
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				auto val = std::make_unique<conjunction_expression>(baseValue_);
				for (auto& elem : elems_)
					val->elems_.emplace_back(elem);
				return std::move(val);
			}
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				bool first;
				if (is_true(baseValue_))
					first = true;
				else
				{
					stream << number_false;
					first = false;
				}
				for (auto& elem : elems_)
				{
					bool positive = elem.is_not_inverse();
					if (first)
					{
						if (positive)
							stream << print_in_brackets(elem.get_expression());
						else
							stream << "!" << print_in_brackets(elem.get_expression());
					}
					else
					{
						if (positive)
							stream << " && " << print_in_brackets(elem.get_expression());
						else
							stream << " && !" << print_in_brackets(elem.get_expression());
					}
					first = false;
				}
				return stream << ")";
			}
		};

		/// <summary>
		/// Variadic expression representing the logical disjunction.
		/// The inverse of an expression is thereby !expression.
		/// </summary>
		class disjunction_expression : public variadic_expression_base
		{
		public:
			static constexpr number null_element = number_false;
		public:
			disjunction_expression(number baseValue = null_element) : variadic_expression_base(baseValue)
			{
			}
			virtual number get_null_element() const noexcept override
			{
				return null_element;
			}
			virtual std::unique_ptr<expression_base> get_unary_inverse(const expression_base* expression) const override
			{
				return std::make_unique<not_expression>(expression->clone());
			}
			virtual number eval() const override
			{
				bool value = is_true(baseValue_);
				for (auto& elem : elems_)
				{
					if (value)
						return number_true;
					if (elem.is_not_inverse())
						value |= is_true(elem.get_expression()->eval());
					else
						value |= is_false(elem.get_expression()->eval());
				}
				return value ? number_true : number_false;
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				bool value = is_true(baseValue_);
				std::vector<element> simElems;
				for (auto& elem : elems_)
				{
					if (value)
						return std::make_unique<number_expression>(number_true);
					auto simElem = elem.get_expression()->simplify(variableLookup);
					if (dynamic_cast<number_expression*>(simElem.get()))
					{
						if (elem.is_not_inverse())
						{
							value |= is_true(static_cast<number_expression*>(simElem.get())->get_number());
						}
						else
						{
							value |= is_false(static_cast<number_expression*>(simElem.get())->get_number());
						}
					}
					else if (dynamic_cast<not_expression*>(simElem.get()))
					{
						simElems.emplace_back(elem.is_not_inverse(), static_cast<not_expression*>(simElem.get())->get_expression()->clone());
					}
					else if (elem.is_not_inverse() && dynamic_cast<disjunction_expression*>(simElem.get()))
					{
						auto& subExp = *static_cast<disjunction_expression*>(simElem.get());
						for (auto subElem : subExp)
						{
							simElems.emplace_back(subElem.is_inverse(), subElem.get_expression()->clone());
						}
					}
					else
					{
						simElems.emplace_back(elem.is_inverse(), std::move(simElem));
					}
				}
				if (value)
					return std::make_unique<number_expression>(number_true);
				else if (simElems.empty())
					return std::make_unique<number_expression>(number_false);
				else if (simElems.size() == 1)
				{
					if (simElems[0].is_not_inverse())
						return simElems[0].get_expression()->clone();
					else
						return get_unary_inverse(simElems[0].get_expression());
				}
				else
				{
					auto returnVal = std::make_unique<disjunction_expression>(number_true);
					for (auto& elem : simElems)
						returnVal->elems_.emplace_back(elem);
					return returnVal;
				}
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				auto val = std::make_unique<disjunction_expression>(baseValue_);
				for (auto& elem : elems_)
					val->elems_.emplace_back(elem);
				return std::move(val);
			}
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				bool first;
				if (is_false(baseValue_))
					first = true;
				else
				{
					stream << number_true;
					first = false;
				}
				for (auto& elem : elems_)
				{
					bool positive = elem.is_not_inverse();
					if (first)
					{
						if (positive)
							stream << print_in_brackets(elem.get_expression());
						else
							stream << "!" << print_in_brackets(elem.get_expression());
					}
					else
					{
						if (positive)
							stream << " || " << print_in_brackets(elem.get_expression());
						else
							stream << " || !" << print_in_brackets(elem.get_expression());
					}
					first = false;
				}
				return stream;
			}
		};
		
		/// <summary>
		/// A exponentiation represents a base value raised to a given power, i.e.
		/// exponentiation_expression = base ^ exponent;
		/// With the operator ^ here representing the exponentiation operator.
		/// </summary>
		class exponentiation_expression : public expression_base
		{
		public:
			exponentiation_expression(std::unique_ptr<expression_base> base, std::unique_ptr<expression_base> exponent) :
				base_(std::move(base)),
				exponent_(std::move(exponent))
			{
			}
			virtual number eval() const override
			{
				return ::pow(base_->eval(), exponent_->eval());
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				return std::make_unique<exponentiation_expression>(base_->clone(), exponent_->clone());
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				auto simpBase = base_->simplify(variableLookup);
				auto simpExponent = exponent_->simplify(variableLookup);
				auto numBase = dynamic_cast<number_expression*>(simpBase.get());
				auto numExponent = dynamic_cast<number_expression*>(simpExponent.get());
				if (numBase && numExponent)
				{
					auto result = ::pow(numBase->get_number(), numExponent->get_number());
					return std::make_unique<number_expression>(result);
				}
				else if (numExponent)
				{
					auto exp = numExponent->get_number();
					if (exp == 0)
						return std::make_unique<number_expression>(1);
					else if (exp == 1)
						return std::move(simpBase);
					else if (exp == -1)
					{
						return std::make_unique<divide_expression>(std::move(simpBase));
					}
				}
				else if (numBase)
				{
					auto ba = numBase->get_number();
					if (ba == 0)
						return std::make_unique<number_expression>(0);
					else if (ba == 1)
						return std::make_unique<number_expression>(1);
				}
				return std::make_unique<exponentiation_expression>(std::move(simpBase), std::move(simpExponent));
			}
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				return stream << print_in_brackets(base_) << "^" << print_in_brackets(exponent_);
			}
			virtual void bind(const binding_lookup& bindingLookup)
			{
				base_->bind(bindingLookup);
				exponent_->bind(bindingLookup);
			}
		private:
			std::unique_ptr<expression_base> base_;
			std::unique_ptr<expression_base> exponent_;
		};


		class comparison_expression : public expression_base
		{
		public:
			enum type
			{
				type_equal,
				type_not_equal,
				type_greater,
				type_greater_equal,
				type_less,
				type_less_equal
			};
		private:
			bool eval_int(number left, number right, type type) const noexcept
			{
				switch (type)
				{
				case type_equal:
					return left == right;
				case type_not_equal:
					return left != right;
				case type_greater:
					return left > right;
				case type_greater_equal:
					return left >= right;
				case type_less:
					return left < right;
				case type_less_equal:
					return left <= right;
				}
			}
		public:
			comparison_expression(std::unique_ptr<expression_base> left, std::unique_ptr<expression_base> right, type type) :
				left_(std::move(left)),
				right_(std::move(right)),
				type_(type)
			{
			}
			virtual number eval() const override
			{
				return eval_int(left_->eval(), right_->eval(), type_) ? 1 : 0;
			}
			virtual std::unique_ptr<expression_base> clone() const override
			{
				return std::make_unique<comparison_expression>(left_->clone(), right_->clone(), type_);
			}
			virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
			{
				auto simpLeft = left_->simplify(variableLookup);
				auto simpRight = right_->simplify(variableLookup);
				auto numLeft = dynamic_cast<number_expression*>(simpLeft.get());
				auto numRight = dynamic_cast<number_expression*>(simpRight.get());
				if (numLeft && numRight)
				{
					return std::make_unique<number_expression>(eval_int(numLeft->get_number(), numRight->get_number(), type_) ? 1 : 0);
				}
				else 
					return std::make_unique<comparison_expression>(std::move(simpLeft), std::move(simpRight), type_);
			}
			virtual std::ostream& printCmdl(std::ostream& stream) const noexcept override
			{
				std::string symbol;
				switch (type_)
				{
				case type_equal:
					symbol = "==";
				case type_not_equal:
					symbol = "!=";
				case type_greater:
					symbol = ">";
				case type_greater_equal:
					symbol = ">=";
				case type_less:
					symbol = "<";
				case type_less_equal:
					symbol = "<=";
				}
				return stream << print_in_brackets(left_) << " " << symbol << " " << print_in_brackets(right_);
			}
			virtual void bind(const binding_lookup& bindingLookup)
			{
				left_->bind(bindingLookup);
				right_->bind(bindingLookup);
			}
		private:
			std::unique_ptr<expression_base> left_;
			std::unique_ptr<expression_base> right_;
			type type_;
		};
	}
}