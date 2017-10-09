#pragma once
#include <memory>
#include "expression.h"
#include "number_expression.h"
namespace expression
{
	/// <summary>
	/// Expression representing the (unary) not operator for an expression, i.e.
	/// not_expression = is_false(expression).
	/// </summary>
	class unary_not_expression : public expression_base
	{
	public:
		unary_not_expression(std::unique_ptr<expression_base> expression) :
			expression_(std::move(expression))
		{
		}
		virtual number eval() const override
		{
			return is_true(expression_->eval()) ? number_false : number_true;
		}
		virtual std::unique_ptr<expression_base> clone() const override
		{
			return std::make_unique<unary_not_expression>(expression_->clone());
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
			auto notExpression = dynamic_cast<unary_not_expression*>(simpExpression.get());
			if (notExpression)
			{
				return std::move(notExpression->expression_);
			}
			return std::make_unique<unary_not_expression>(std::move(simpExpression));
		}
		virtual void printCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			stream << "!";
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
			return std::make_unique<unary_not_expression>(expression->clone());
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
				else if (dynamic_cast<unary_not_expression*>(simElem.get()))
				{
					simElems.emplace_back(elem.is_not_inverse(), static_cast<unary_not_expression*>(simElem.get())->get_expression()->clone());
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
		virtual void printCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			if (subExpression)
				stream << "(";
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
					if (!positive)
						stream << "!";
					elem.get_expression()->printCmdl(stream, true);
				}
				else
				{
					if (positive)
						stream << " && ";
					else
						stream << " && !";
					elem.get_expression()->printCmdl(stream, true);
				}
				first = false;
			}
			if (subExpression)
				stream << ")";
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
			return std::make_unique<unary_not_expression>(expression->clone());
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
				else if (dynamic_cast<unary_not_expression*>(simElem.get()))
				{
					simElems.emplace_back(elem.is_not_inverse(), static_cast<unary_not_expression*>(simElem.get())->get_expression()->clone());
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
		virtual void printCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			if (subExpression)
				stream << "(";
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
					if (!positive)
						stream << "!";
					elem.get_expression()->printCmdl(stream, true);
				}
				else
				{
					if (positive)
						stream << " || ";
					else
						stream << " || !";
					elem.get_expression()->printCmdl(stream, true);
				}
				first = false;
			}
			if (subExpression)
				stream << ")";
		}
	};
}