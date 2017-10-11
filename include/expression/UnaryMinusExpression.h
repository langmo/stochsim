#pragma once
#include <memory>
#include "expression_common.h"
#include "NumberExpression.h"
namespace expression
{
	/// <summary>
	/// Expression representing the (unary) minus of an expression, i.e. 
	/// minus_expression = -expression.
	/// </summary>
	class UnaryMinusExpression : public IExpression
	{
	public:
		UnaryMinusExpression(std::unique_ptr<IExpression> expression) :
			expression_(std::move(expression))
		{
		}
		virtual number Eval() const override
		{
			return -expression_->Eval();
		}
		virtual std::unique_ptr<IExpression> Clone() const override
		{
			return std::make_unique<UnaryMinusExpression>(expression_->Clone());
		}
		virtual std::unique_ptr<IExpression> Simplify(const VariableLookup& variableLookup) const override
		{
			auto simpExpression = expression_->Simplify(variableLookup);
			auto numExpression = dynamic_cast<NumberExpression*>(simpExpression.get());
			if (numExpression)
			{
				auto result = -numExpression->GetValue();
				return std::make_unique<NumberExpression>(result);
			}
			auto minusExpression = dynamic_cast<UnaryMinusExpression*>(simpExpression.get());
			if (minusExpression)
			{
				return std::move(minusExpression->expression_);
			}
			return std::make_unique<UnaryMinusExpression>(std::move(simpExpression));
		}
		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			stream << "-";
			expression_->PrintCmdl(stream, true);
		}
		IExpression* get_expression() noexcept
		{
			return expression_.get();
		}
		const IExpression* get_expression() const noexcept
		{
			return expression_.get();
		}
		virtual void Bind(const BindingLookup& bindingLookup) override
		{
			expression_->Bind(bindingLookup);
		}
	private:
		std::unique_ptr<IExpression> expression_;
	};
}