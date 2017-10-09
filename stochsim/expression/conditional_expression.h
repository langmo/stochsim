#pragma once
#include <memory>
#include "expression.h"
#include "number_expression.h"
namespace expression
{
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
		virtual void printCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			if (subExpression)
				stream << "(";
			condition_->printCmdl(stream, true);
			stream << " ? ";
			expressionIfTrue_->printCmdl(stream, true);
			stream << " : ";
			expressionIfFalse_->printCmdl(stream, true);
			if (subExpression)
				stream << ")";
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
}