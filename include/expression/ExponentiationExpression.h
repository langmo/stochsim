#pragma once
#include <memory>
#include "expression_common.h"
#include "NumberExpression.h"
#include "ProductExpression.h"
namespace expression
{
	/// <summary>
	/// A exponentiation represents a base value raised to a given power, i.e.
	/// exponentiation_expression = base ^ exponent;
	/// With the operator ^ here representing the exponentiation operator.
	/// </summary>
	class ExponentiationExpression : public IExpression
	{
	public:
		ExponentiationExpression(std::unique_ptr<IExpression> base, std::unique_ptr<IExpression> exponent) :
			base_(std::move(base)),
			exponent_(std::move(exponent))
		{
		}
		virtual number Eval() const override
		{
			return ::pow(base_->Eval(), exponent_->Eval());
		}
		virtual std::unique_ptr<IExpression> Clone() const override
		{
			return std::make_unique<ExponentiationExpression>(base_->Clone(), exponent_->Clone());
		}
		virtual std::unique_ptr<IExpression> Simplify(const VariableLookup& variableLookup) const override
		{
			auto simpBase = base_->Simplify(variableLookup);
			auto simpExponent = exponent_->Simplify(variableLookup);
			auto numBase = dynamic_cast<NumberExpression*>(simpBase.get());
			auto numExponent = dynamic_cast<NumberExpression*>(simpExponent.get());
			if (numBase && numExponent)
			{
				auto result = ::pow(numBase->GetValue(), numExponent->GetValue());
				return std::make_unique<NumberExpression>(result);
			}
			else if (numExponent)
			{
				auto exp = numExponent->GetValue();
				if (exp == 0)
					return std::make_unique<NumberExpression>(1);
				else if (exp == 1)
					return std::move(simpBase);
				else if (exp == -1)
				{
					return std::make_unique<UnaryDivideExpression>(std::move(simpBase));
				}
			}
			else if (numBase)
			{
				auto ba = numBase->GetValue();
				if (ba == 0)
					return std::make_unique<NumberExpression>(0);
				else if (ba == 1)
					return std::make_unique<NumberExpression>(1);
			}
			return std::make_unique<ExponentiationExpression>(std::move(simpBase), std::move(simpExponent));
		}
		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			if (subExpression)
				stream << "(";
			base_->PrintCmdl(stream, true);
			stream << "^";
			exponent_->PrintCmdl(stream, true);
			if (subExpression)
				stream << ")";
		}
		virtual void Bind(const BindingLookup& bindingLookup) override
		{
			base_->Bind(bindingLookup);
			exponent_->Bind(bindingLookup);
		}
	private:
		std::unique_ptr<IExpression> base_;
		std::unique_ptr<IExpression> exponent_;
	};
}