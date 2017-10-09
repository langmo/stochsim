#pragma once
#include <memory>
#include "expression.h"
namespace expression
{
	/// <summary>
	/// A number expression is the simplest type of an expression, i.e. a constant number.
	/// </summary>
	class number_expression : public expression_base
	{
	public:
		number_expression(number number) : number_(number)
		{
		}
		virtual number eval() const override
		{
			return number_;
		}
		virtual std::unique_ptr<expression_base> clone() const override
		{
			return std::make_unique<number_expression>(number_);
		}
		virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
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
		virtual void printCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			stream << number_;
		}
		virtual void bind(const binding_lookup& bindingLookup)
		{
			// do nothing
		}
	private:
		number number_;
	};
}