#pragma once
#include <memory>
#include <string>
#include <functional>
#include <vector>
namespace stochsim
{
	namespace expression
	{
		class basic_expression;
		typedef double number;
		typedef std::string identifier;
		/// <summary>
		/// Function taking the name of a variable and returning its value. Throws a std::exception if a variable with the given name does not
		/// exist, or if the variable is an expression and the expression could not be evaluated.
		/// </summary>
		typedef std::function<number(const identifier& variableName)> variable_lookup;
		/// <summary>
		/// Function taking the name of a variable and returning its expression. Throws a std::exception if a variable with the given name does not
		/// exist. If variables are directly saved by value, a corresponding number_expression is returned.
		/// </summary>
		typedef std::function<const basic_expression* (const identifier& variableName)> variable_expression_lookup;
		/// <summary>
		/// Abstract base class of all expressions.
		/// </summary>
		class basic_expression
		{
		public:
			/// <summary>
			/// Destructor.
			/// </summary>
			virtual ~basic_expression()
			{
			}
			/// <summary>
			/// Evaluates the expression. If an error occurs, e.g. because this expression or a child expression corresponds to a variable
			/// not defined in the lookup, throws an std::exception.
			/// </summary>
			/// <param name="lookup">Function to lookup the value of variables based on their name.</param>
			/// <returns>Value of expression</returns>
			virtual number eval(const variable_lookup& lookup) const = 0;
			/// <summary>
			/// Creates a clone/copy of this expression.
			/// </summary>
			/// <returns>Copy of this expression</returns>
			virtual std::unique_ptr<basic_expression> clone() const noexcept = 0;
			/// <summary>
			/// Creates a new expression which is a simplified version of the current expression. For this, all variables defined in the partial lookup
			/// are replaced by their corresponding expressions. It is guaranteed that the new expression, and all of its subexpressions, do not contain
			/// variables defined in the partial lookup. Variables not defined as a partial lookup are kept as name references. For this function, the simplest
			/// possible return value is considered to be a number_expression.
			/// This function may throw a std::exception if the simplification is mathematically incorrect, e.g. for division by zero errors and similar.
			/// </summary>
			/// <param name="partialLookup">The partial lookup for the variables which should be replaced by their corresponding expressions.</param>
			/// <returns>A simplified version of this expression. If simplification is not possible, returns a clone/copy of this expression.</returns>
			virtual std::unique_ptr<basic_expression> simplify(const variable_expression_lookup& partialLookup) const noexcept = 0;
		};
		class number_expression : public basic_expression
		{
		public:
			number_expression(number number) : number_(number)
			{
			}
			virtual number eval(const variable_lookup& lookup) const override
			{
				return number_;
			}
			virtual std::unique_ptr<basic_expression> clone() const noexcept override
			{
				return std::make_unique<number_expression>(number_);
			}
			virtual std::unique_ptr<basic_expression> simplify(const variable_expression_lookup& partialLookup) const noexcept override
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
		private:
			number number_;
		};
		class conditional_expression : public basic_expression
		{
		public:
			conditional_expression(std::unique_ptr<basic_expression> condition, std::unique_ptr<basic_expression> expressionIfTrue, std::unique_ptr<basic_expression> expressionIfFalse) :
				condition_(std::move(condition)),
				expressionIfTrue_(std::move(expressionIfTrue)),
				expressionIfFalse_(std::move(expressionIfFalse))
			{
			}
			virtual number eval(const variable_lookup& lookup) const override
			{
				if (condition_->eval(lookup) != 0)
					return expressionIfTrue_->eval(lookup);
				else
					return expressionIfFalse_->eval(lookup);
			}
			virtual std::unique_ptr<basic_expression> clone() const noexcept override
			{
				return std::make_unique<conditional_expression>(condition_->clone(), expressionIfTrue_->clone(), expressionIfFalse_->clone());
			}
			virtual std::unique_ptr<basic_expression> simplify(const variable_expression_lookup& partialLookup) const noexcept override
			{
				auto simpCondition = condition_->simplify(partialLookup);
				if (dynamic_cast<number_expression*>(simpCondition.get()))
				{
					if (dynamic_cast<number_expression*>(simpCondition.get())->get_number() != 0)
					{
						return expressionIfTrue_->simplify(partialLookup);
					}
					else
					{
						return expressionIfFalse_->simplify(partialLookup);
					}
				}
				else
				{
					return std::make_unique<conditional_expression>(std::move(simpCondition), expressionIfTrue_->simplify(partialLookup), expressionIfFalse_->simplify(partialLookup));
				}
			}
		private:
			std::unique_ptr<basic_expression> condition_;
			std::unique_ptr<basic_expression> expressionIfTrue_;
			std::unique_ptr<basic_expression> expressionIfFalse_;
		};
		class variable_expression : public basic_expression
		{
		public:
			variable_expression(identifier name) : name_(name)
			{
			}
			virtual number eval(const variable_lookup& lookup) const override
			{
				return lookup(name_);
			}
			virtual std::unique_ptr<basic_expression> simplify(const variable_expression_lookup& partialLookup) const noexcept override
			{
				const basic_expression* value;
				try
				{
					value = partialLookup(name_);
				}
				catch (...)
				{
					return clone();
				}
				return value->simplify(partialLookup);
			}
			identifier get_identifier() const noexcept
			{
				return name_;
			}
			operator identifier() const noexcept
			{
				return name_;
			}
			virtual std::unique_ptr<basic_expression> clone() const noexcept override
			{
				return std::make_unique<variable_expression>(name_);
			}
		private:
			identifier name_;
		};

		class sum_expression : public basic_expression
		{
		public:
			class element
			{
			private:
				std::unique_ptr<basic_expression> expression_;
				bool positive_;
			public:
				element(std::unique_ptr<basic_expression> expression, bool positive) noexcept : expression_(std::move(expression)), positive_(positive)
				{
				}
				element(const element& other) : positive_(other.positive_), expression_(other.expression_->clone())
				{
				}
				element& operator= (element other)
				{
					expression_ = std::move(other.expression_);
					positive_ = other.positive_;
				}

				expression::number eval(const variable_lookup& lookup) const
				{
					const std::string* test = new std::string();
					delete test;

					return positive_ ? expression_->eval(lookup) : 1 / expression_->eval(lookup);
				}
				/// <summary>
				/// Returns a pointer to the expression of this element.
				/// This expression is still owned and memory managed by this object.
				/// </summary>
				/// <returns>Pointer to the expression</returns>
				basic_expression* get_expression() noexcept
				{
					return expression_.get();
				}

				const basic_expression& get_expression() const noexcept
				{
					return *expression_;
				}

				bool is_positive() const noexcept
				{
					return positive_;
				}
			};
		public:
			typedef std::vector<element>::size_type size_type;
			typedef element value_type;
			typedef value_type& reference;
			typedef const value_type& const_reference;
			typedef std::vector<element>::iterator iterator;
			typedef std::vector<element>::const_iterator const_iterator;

		public:
			// Element access, iterators, ...
			void push_back(std::unique_ptr<basic_expression> expression, bool positive)
			{
				elems_.push_back(element(std::move(expression), positive));
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
		public:
			sum_expression(number baseValue = 0) : baseValue_(baseValue)
			{
			}
			virtual number eval(const variable_lookup& lookup) const override
			{
				number sum = baseValue_;
				for (auto& elem : elems_)
				{
					sum += elem.eval(lookup);
				}
				return sum;
			}
			virtual std::unique_ptr<basic_expression> simplify(const variable_expression_lookup& partialLookup) const noexcept override
			{
				auto value = baseValue_;
				std::vector<element> simElems;
				for (auto& elem : elems_)
				{
					auto simElem = elem.get_expression().simplify(partialLookup);
					if (dynamic_cast<number_expression*>(simElem.get()))
					{
						if (elem.is_positive())
						{
							value += static_cast<number_expression*>(simElem.get())->get_number();
						}
						else
						{
							value -= static_cast<number_expression*>(simElem.get())->get_number();
						}
					}
					else if (dynamic_cast<sum_expression*>(simElem.get()))
					{
						auto& subExp = *static_cast<sum_expression*>(simElem.get());
						value += elem.is_positive() ? subExp.baseValue_ : -subExp.baseValue_;
						for (auto subElem : subExp)
						{
							simElems.emplace_back(subElem.get_expression()->clone(), elem.is_positive() ? subElem.is_positive() : !subElem.is_positive());
						}
					}
					else
					{
						simElems.emplace_back(std::move(simElem), elem.is_positive());
					}
				}
				if (simElems.empty())
					return std::make_unique<number_expression>(value);
				else if (value == 0 && simElems.size() == 1 && simElems[0].is_positive())
					return simElems[0].get_expression()->clone();
				else
				{
					auto returnVal = std::make_unique<sum_expression>(value);
					for (auto& elem : simElems)
						returnVal->elems_.emplace_back(elem);
					return returnVal;
				}
			}
			virtual std::unique_ptr<basic_expression> clone() const noexcept override
			{
				auto val = std::make_unique<sum_expression>(baseValue_);
				for (auto& elem : elems_)
					val->elems_.emplace_back(elem);
				return std::move(val);
			}
		private:
			std::vector<element> elems_;
			number baseValue_;
		};

		class product_expression : public basic_expression
		{
		public:
			class element
			{
			private:
				std::unique_ptr<basic_expression> expression_;
				bool positive_;
			public:
				element(std::unique_ptr<basic_expression> expression, bool positive) : expression_(std::move(expression)), positive_(positive)
				{
				}
				element(const element& other) : positive_(other.positive_), expression_(other.expression_->clone())
				{
				}
				element& operator= (element other)
				{
					expression_ = std::move(other.expression_);
					positive_ = other.positive_;
				}
				expression::number eval(const variable_lookup& lookup) const
				{
					return positive_ ? expression_->eval(lookup) : 1 / expression_->eval(lookup);
				}

				/// <summary>
				/// Returns a pointer to the expression of this element.
				/// This expression is still owned and memory managed by this object.
				/// </summary>
				/// <returns>Pointer to the expression</returns>
				basic_expression* get_expression() noexcept
				{
					return expression_.get();
				}

				const basic_expression& get_expression() const
				{
					return *expression_;
				}

				bool is_positive() const
				{
					return positive_;
				}
			};
		public:
			typedef std::vector<element>::size_type size_type;
			typedef element value_type;
			typedef value_type& reference;
			typedef const value_type& const_reference;
			typedef std::vector<element>::iterator iterator;
			typedef std::vector<element>::const_iterator const_iterator;
		
		public:
			// Element access, iterators, ...
			void push_back(std::unique_ptr<basic_expression> expression, bool positive)
			{
				elems_.push_back(element(std::move(expression), positive));
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

		public:
			product_expression(number baseValue = 1) : baseValue_(baseValue)
			{
			}
			virtual number eval(const variable_lookup& lookup) const override
			{
				number sum = baseValue_;
				for (auto& elem : elems_)
				{
					sum *= elem.eval(lookup);
				}
				return sum;
			}
			virtual std::unique_ptr<basic_expression> simplify(const variable_expression_lookup& partialLookup) const noexcept override
			{
				auto value = baseValue_;
				std::vector<element> simElems;
				for (auto& elem : elems_)
				{
					auto simElem = elem.get_expression().simplify(partialLookup);
					if (dynamic_cast<number_expression*>(simElem.get()))
					{
						if (elem.is_positive())
						{
							value *= static_cast<number_expression*>(simElem.get())->get_number();
						}
						else
						{
							value /= static_cast<number_expression*>(simElem.get())->get_number();
						}
					}
					else if (dynamic_cast<product_expression*>(simElem.get()))
					{
						auto& subExp = *static_cast<product_expression*>(simElem.get());
						value *= elem.is_positive() ? subExp.baseValue_ : 1/subExp.baseValue_;
						for (auto subElem : subExp)
						{
							simElems.emplace_back(subElem.get_expression()->clone(), elem.is_positive() ? subElem.is_positive() : !subElem.is_positive());
						}
					}
					else
					{
						simElems.emplace_back(std::move(simElem), elem.is_positive());
					}
				}
				if (simElems.empty()|| value == 0)
					return std::make_unique<number_expression>(value);
				else if (value == 1 && simElems.size() == 1 && simElems[0].is_positive())
					return simElems[0].get_expression()->clone();
				else
				{
					auto returnVal = std::make_unique<product_expression>(value);
					for (auto& elem : simElems)
						returnVal->elems_.emplace_back(elem);
					return returnVal;
				}
			}
			virtual std::unique_ptr<basic_expression> clone() const noexcept override
			{
				auto val = std::make_unique<product_expression>(baseValue_);
				for (auto& elem : elems_)
					val->elems_.emplace_back(elem);
				return std::move(val);
			}
		private:
			std::vector<element> elems_;
			number baseValue_;
		};
	}
}