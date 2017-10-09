#pragma once
#include <memory>
#include "expression.h"
#include "number_expression.h"
namespace expression
{
	/// <summary>
	/// Expression representing the (unary) minus of an expression, i.e. 
	/// minus_expression = -expression.
	/// </summary>
	class unary_minus_expression : public expression_base
	{
	public:
		unary_minus_expression(std::unique_ptr<expression_base> expression) :
			expression_(std::move(expression))
		{
		}
		virtual number eval() const override
		{
			return -expression_->eval();
		}
		virtual std::unique_ptr<expression_base> clone() const override
		{
			return std::make_unique<unary_minus_expression>(expression_->clone());
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
			auto minusExpression = dynamic_cast<unary_minus_expression*>(simpExpression.get());
			if (minusExpression)
			{
				return std::move(minusExpression->expression_);
			}
			return std::make_unique<unary_minus_expression>(std::move(simpExpression));
		}
		virtual void printCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			stream << "-";
			expression_->printCmdl(stream, true);
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
			return std::make_unique<unary_minus_expression>(expression->clone());
		}
		virtual number eval() const override
		{
			number sum = baseValue_;
			for (auto& elem : elems_)
			{
				if (elem.is_not_inverse())
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
				else if (dynamic_cast<unary_minus_expression*>(simElem.get()))
				{
					simElems.emplace_back(elem.is_not_inverse(), static_cast<unary_minus_expression*>(simElem.get())->get_expression()->clone());
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

		virtual void printCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			if (subExpression)
				stream << "(";
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
						if (!positive)
							stream << "-";
						stream << value;
					}
					else
					{
						if (positive)
							stream << " + ";
						else
							stream << " - ";
						stream << value;
					}
				}
				else
				{
					if (first)
					{
						if (!positive)
							stream << "-";
						elem.get_expression()->printCmdl(stream, true);
					}
					else
					{
						if (positive)
							stream << " + ";
						else
							stream << " - ";
						elem.get_expression()->printCmdl(stream, true);
					}
				}
				first = false;
			}

			if (subExpression)
				stream << ")";
		}

	};
}