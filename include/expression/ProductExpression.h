#pragma once
#include <memory>
#include "expression_common.h"
#include "NumberExpression.h"
#include "UnaryDivideExpression.h"
namespace expression
{
	/// <summary>
	/// Variadic expression representing the product of various elements.
	/// The inverse of an expression is thereby 1/expression. Thus, product_expression-s represent both sums and ratios.
	/// </summary>
	class ProductExpression : public IVariadicExpression
	{
	public:
		static constexpr number NullElement = 1;
	public:
		ProductExpression(number baseValue = NullElement) : IVariadicExpression(baseValue)
		{
		}
		virtual number GetNullElement() const noexcept override
		{
			return NullElement;
		}
		virtual std::unique_ptr<IExpression> GetUnaryInverse(const IExpression* expression) const override
		{
			return std::make_unique<UnaryDivideExpression>(expression->Clone());
		}
		virtual number Eval() const override
		{
			number sum = baseValue_;
			for (auto& elem : elems_)
			{
				if (elem.IsNotInverse())
					sum *= elem.GetExpression()->Eval();
				else
					sum /= elem.GetExpression()->Eval();
			}
			return sum;
		}
		virtual std::unique_ptr<IExpression> Simplify(const VariableRegister& variableRegister) const override
		{
			auto value = baseValue_;
			std::vector<Element> simElems;
			for (auto& elem : elems_)
			{
				auto simElem = elem.GetExpression()->Simplify(variableRegister);
				if (dynamic_cast<NumberExpression*>(simElem.get()))
				{
					if (elem.IsNotInverse())
					{
						value *= static_cast<NumberExpression*>(simElem.get())->GetValue();
					}
					else
					{
						value /= static_cast<NumberExpression*>(simElem.get())->GetValue();
					}
				}
				else if (dynamic_cast<UnaryDivideExpression*>(simElem.get()))
				{
					simElems.emplace_back(elem.IsNotInverse(), static_cast<UnaryDivideExpression*>(simElem.get())->get_expression()->Clone());
				}
				else if (dynamic_cast<ProductExpression*>(simElem.get()))
				{
					auto& subExp = *static_cast<ProductExpression*>(simElem.get());
					value *= elem.IsNotInverse() ? subExp.baseValue_ : 1 / subExp.baseValue_;
					for (auto subElem : subExp)
					{
						simElems.emplace_back(elem.IsInverse() ? subElem.IsNotInverse() : subElem.IsInverse(), subElem.GetExpression()->Clone());
					}
				}
				else
				{
					simElems.emplace_back(elem.IsInverse(), std::move(simElem));
				}
			}
			if (simElems.empty() || value == 0)
				return std::make_unique<NumberExpression>(value);
			else if (value == 1 && simElems.size() == 1)
			{
				if (simElems[0].IsNotInverse())
					return simElems[0].GetExpression()->Clone();
				else
					return GetUnaryInverse(simElems[0].GetExpression());
			}
			else
			{
				auto returnVal = std::make_unique<ProductExpression>(value);
				for (auto& elem : simElems)
					returnVal->elems_.emplace_back(elem);
				return returnVal;
			}
		}
		virtual std::unique_ptr<IExpression> Clone() const override
		{
			auto val = std::make_unique<ProductExpression>(baseValue_);
			for (auto& elem : elems_)
				val->elems_.emplace_back(elem);
			return std::move(val);
		}
		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			if (subExpression)
				stream << "(";
			bool first;
			if (baseValue_ == 1)
				first = true;
			else
			{
				stream << baseValue_;
				first = false;
			}
			for (auto& elem : elems_)
			{
				bool positive = elem.IsNotInverse();
				if (first)
				{
					if (!positive)
						stream << "1 / ";
					elem.GetExpression()->PrintCmdl(stream, true);
				}
				else
				{
					if (positive)
						stream << " * ";
					else
						stream << " / ";
					elem.GetExpression()->PrintCmdl(stream, true);
				}
				first = false;
			}
			if (subExpression)
				stream << ")";
		}
	};
}