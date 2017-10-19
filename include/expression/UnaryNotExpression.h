#pragma once
#include <memory>
#include "expression_common.h"
#include "NumberExpression.h"
namespace expression
{
	/// <summary>
	/// Expression representing the (unary) not operator for an expression, i.e.
	/// not_expression = is_false(expression).
	/// </summary>
	class UnaryNotExpression : public IExpression
	{
	public:
		UnaryNotExpression(std::unique_ptr<IExpression> expression) :
			expression_(std::move(expression))
		{
		}
		virtual number Eval() const override
		{
			return isTrue(expression_->Eval()) ? number_false : number_true;
		}
		virtual std::unique_ptr<IExpression> Clone() const override
		{
			return std::make_unique<UnaryNotExpression>(expression_->Clone());
		}
		virtual std::unique_ptr<IExpression> Simplify(const VariableRegister& variableRegister) const override
		{
			auto simpExpression = expression_->Simplify(variableRegister);
			auto numExpression = dynamic_cast<NumberExpression*>(simpExpression.get());
			if (numExpression)
			{
				auto result = isTrue(numExpression->GetValue()) ? number_false : number_true;
				return std::make_unique<NumberExpression>(result);
			}
			auto notExpression = dynamic_cast<UnaryNotExpression*>(simpExpression.get());
			if (notExpression)
			{
				return std::move(notExpression->expression_);
			}
			return std::make_unique<UnaryNotExpression>(std::move(simpExpression));
		}
		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			stream << "!";
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
		virtual void Bind(const BindingRegister& bindingRegister) override
		{
			expression_->Bind(bindingRegister);
		}
	private:
		std::unique_ptr<IExpression> expression_;
	};
}