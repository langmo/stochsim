#pragma once
#include <memory>
#include "expression_common.h"
namespace expression
{
	/// <summary>
	/// A number expression is the simplest type of an expression, i.e. a constant number.
	/// </summary>
	class NumberExpression : public IExpression
	{
	public:
		NumberExpression(number number) : number_(number)
		{
		}
		virtual number Eval() const override
		{
			return number_;
		}
		virtual std::unique_ptr<IExpression> Clone() const override
		{
			return std::make_unique<NumberExpression>(number_);
		}
		virtual std::unique_ptr<IExpression> Simplify(const VariableLookup& variableLookup) const override
		{
			// we are already the most simple possible expression...
			return Clone();
		}
		number GetValue() const noexcept
		{
			return number_;
		}
		operator number() const noexcept
		{
			return number_;
		}
		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			stream << number_;
		}
		virtual void Bind(const BindingLookup& bindingLookup) override
		{
			// do nothing
		}
	private:
		number number_;
	};
}