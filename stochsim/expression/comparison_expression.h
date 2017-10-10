#pragma once
#include <memory>
#include "expression.h"
#include "number_expression.h"
namespace expression
{
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
		virtual void printCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			if (subExpression)
				stream << "(";
			std::string symbol;
			switch (type_)
			{
			case type_equal:
				symbol = "==";
				break;
			case type_not_equal:
				symbol = "!=";
				break;
			case type_greater:
				symbol = ">";
				break;
			case type_greater_equal:
				symbol = ">=";
				break;
			case type_less:
				symbol = "<";
				break;
			case type_less_equal:
				symbol = "<=";
				break;
			}
			left_->printCmdl(stream, true);

			stream << " " << symbol << " ";
			right_->printCmdl(stream, true);
			if (subExpression)
				stream << ")";
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