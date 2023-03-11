#pragma once
#include <memory>
#include "expression_common.h"
#include "NumberExpression.h"
namespace expression
{
	class ComparisonExpression : public IExpression
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
		bool EvalInternal(number left, number right, type type) const
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
			default:
				throw std::runtime_error("Type of comparison operation internally unknown.");
			}
		}
	public:
		ComparisonExpression(std::unique_ptr<IExpression> left, std::unique_ptr<IExpression> right, type type) :
			left_(std::move(left)),
			right_(std::move(right)),
			type_(type)
		{
		}
		virtual number Eval() const override
		{
			return EvalInternal(left_->Eval(), right_->Eval(), type_) ? 1 : 0;
		}
		virtual std::unique_ptr<IExpression> Clone() const override
		{
			return std::make_unique<ComparisonExpression>(left_->Clone(), right_->Clone(), type_);
		}
		virtual std::unique_ptr<IExpression> Simplify(const VariableRegister& variableRegister) const override
		{
			auto simpLeft = left_->Simplify(variableRegister);
			auto simpRight = right_->Simplify(variableRegister);
			auto numLeft = dynamic_cast<NumberExpression*>(simpLeft.get());
			auto numRight = dynamic_cast<NumberExpression*>(simpRight.get());
			if (numLeft && numRight)
			{
				return std::make_unique<NumberExpression>(EvalInternal(numLeft->GetValue(), numRight->GetValue(), type_) ? 1 : 0);
			}
			else
				return std::make_unique<ComparisonExpression>(std::move(simpLeft), std::move(simpRight), type_);
		}
		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept override
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
			left_->PrintCmdl(stream, true);

			stream << " " << symbol << " ";
			right_->PrintCmdl(stream, true);
			if (subExpression)
				stream << ")";
		}
		virtual void Bind(const BindingRegister& bindingRegister) override
		{
			left_->Bind(bindingRegister);
			right_->Bind(bindingRegister);
		}
	private:
		std::unique_ptr<IExpression> left_;
		std::unique_ptr<IExpression> right_;
		type type_;
	};
}