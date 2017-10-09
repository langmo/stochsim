#pragma once
#include <memory>
#include "expression.h"
#include "number_expression.h"
#include "product_expression.h"
namespace expression
{
	/// <summary>
	/// A exponentiation represents a base value raised to a given power, i.e.
	/// exponentiation_expression = base ^ exponent;
	/// With the operator ^ here representing the exponentiation operator.
	/// </summary>
	class exponentiation_expression : public expression_base
	{
	public:
		exponentiation_expression(std::unique_ptr<expression_base> base, std::unique_ptr<expression_base> exponent) :
			base_(std::move(base)),
			exponent_(std::move(exponent))
		{
		}
		virtual number eval() const override
		{
			return ::pow(base_->eval(), exponent_->eval());
		}
		virtual std::unique_ptr<expression_base> clone() const override
		{
			return std::make_unique<exponentiation_expression>(base_->clone(), exponent_->clone());
		}
		virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
		{
			auto simpBase = base_->simplify(variableLookup);
			auto simpExponent = exponent_->simplify(variableLookup);
			auto numBase = dynamic_cast<number_expression*>(simpBase.get());
			auto numExponent = dynamic_cast<number_expression*>(simpExponent.get());
			if (numBase && numExponent)
			{
				auto result = ::pow(numBase->get_number(), numExponent->get_number());
				return std::make_unique<number_expression>(result);
			}
			else if (numExponent)
			{
				auto exp = numExponent->get_number();
				if (exp == 0)
					return std::make_unique<number_expression>(1);
				else if (exp == 1)
					return std::move(simpBase);
				else if (exp == -1)
				{
					return std::make_unique<unary_divide_expression>(std::move(simpBase));
				}
			}
			else if (numBase)
			{
				auto ba = numBase->get_number();
				if (ba == 0)
					return std::make_unique<number_expression>(0);
				else if (ba == 1)
					return std::make_unique<number_expression>(1);
			}
			return std::make_unique<exponentiation_expression>(std::move(simpBase), std::move(simpExponent));
		}
		virtual void printCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			if (subExpression)
				stream << "(";
			base_->printCmdl(stream, true);
			stream << "^";
			exponent_->printCmdl(stream, true);
			if (subExpression)
				stream << ")";
		}
		virtual void bind(const binding_lookup& bindingLookup)
		{
			base_->bind(bindingLookup);
			exponent_->bind(bindingLookup);
		}
	private:
		std::unique_ptr<expression_base> base_;
		std::unique_ptr<expression_base> exponent_;
	};
}