#pragma once
#include <memory>
#include "expression_common.h"
#include "NumberExpression.h"
namespace expression
{
	/// <summary>
	/// Expression representing the (unary) division of an expression, i.e. raising it to the power of -1.
	/// divide_expression = 1/expression.
	/// </summary>
	class UnaryDivideExpression : public IExpression
	{
	public:
		UnaryDivideExpression(std::unique_ptr<IExpression> expression) :
			expression_(std::move(expression))
		{
		}
		virtual number Eval() const override
		{
			return 1 / expression_->Eval();
		}
		virtual std::unique_ptr<IExpression> Clone() const override
		{
			return std::make_unique<UnaryDivideExpression>(expression_->Clone());
		}
		virtual std::unique_ptr<IExpression> Simplify(const VariableRegister& variableRegister) const override
		{
			auto simpExpression = expression_->Simplify(variableRegister);
			auto numExpression = dynamic_cast<NumberExpression*>(simpExpression.get());
			if (numExpression)
			{
				auto result = 1 / numExpression->GetValue();
				return std::make_unique<NumberExpression>(result);
			}
			auto divideExpression = dynamic_cast<UnaryDivideExpression*>(simpExpression.get());
			if (divideExpression)
			{
				return std::move(divideExpression->expression_);
			}
			return std::make_unique<UnaryDivideExpression>(std::move(simpExpression));
		}
		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			if (subExpression)
				stream << "(";
			stream << "1 / ";
			expression_->PrintCmdl(stream, true);
			if (subExpression)
				stream << ")";
		}
		IExpression* get_expression() noexcept
		{
			return expression_.get();
		}
		const IExpression* get_expression() const noexcept
		{
			return expression_.get();
		}
		virtual void Bind(const BindingRegister& bindingRegister) override
		{
			expression_->Bind(bindingRegister);
		}
	private:
		std::unique_ptr<IExpression> expression_;
	};
}