#pragma once
#include <memory>
#include "expression_common.h"
#include "UnaryNotExpression.h"
#include "NumberExpression.h"
namespace expression
{
	/// <summary>
	/// Variadic expression representing the logical disjunction.
	/// The inverse of an expression is thereby !expression.
	/// </summary>
	class DisjunctionExpression : public IVariadicExpression
	{
	public:
		static constexpr number nullElement = number_false;
	public:
		DisjunctionExpression(number baseValue = nullElement) : IVariadicExpression(baseValue)
		{
		}
		virtual number GetNullElement() const noexcept override
		{
			return nullElement;
		}
		virtual std::unique_ptr<IExpression> GetUnaryInverse(const IExpression* expression) const override
		{
			return std::make_unique<UnaryNotExpression>(expression->Clone());
		}
		virtual number Eval() const override
		{
			bool value = isTrue(baseValue_);
			for (auto& elem : elems_)
			{
				if (value)
					return number_true;
				if (elem.IsNotInverse())
					value |= isTrue(elem.GetExpression()->Eval());
				else
					value |= isFalse(elem.GetExpression()->Eval());
			}
			return value ? number_true : number_false;
		}
		virtual std::unique_ptr<IExpression> Simplify(const VariableRegister& variableRegister) const override
		{
			bool value = isTrue(baseValue_);
			std::vector<Element> simElems;
			for (auto& elem : elems_)
			{
				if (value)
					return std::make_unique<NumberExpression>(number_true);
				auto simElem = elem.GetExpression()->Simplify(variableRegister);
				if (dynamic_cast<NumberExpression*>(simElem.get()))
				{
					if (elem.IsNotInverse())
					{
						value |= isTrue(static_cast<NumberExpression*>(simElem.get())->GetValue());
					}
					else
					{
						value |= isFalse(static_cast<NumberExpression*>(simElem.get())->GetValue());
					}
				}
				else if (dynamic_cast<UnaryNotExpression*>(simElem.get()))
				{
					simElems.emplace_back(elem.IsNotInverse(), static_cast<UnaryNotExpression*>(simElem.get())->get_expression()->Clone());
				}
				else if (elem.IsNotInverse() && dynamic_cast<DisjunctionExpression*>(simElem.get()))
				{
					auto& subExp = *static_cast<DisjunctionExpression*>(simElem.get());
					for (auto subElem : subExp)
					{
						simElems.emplace_back(subElem.IsInverse(), subElem.GetExpression()->Clone());
					}
				}
				else
				{
					simElems.emplace_back(elem.IsInverse(), std::move(simElem));
				}
			}
			if (value)
				return std::make_unique<NumberExpression>(number_true);
			else if (simElems.empty())
				return std::make_unique<NumberExpression>(number_false);
			else if (simElems.size() == 1)
			{
				if (simElems[0].IsNotInverse())
					return simElems[0].GetExpression()->Clone();
				else
					return GetUnaryInverse(simElems[0].GetExpression());
			}
			else
			{
				auto returnVal = std::make_unique<DisjunctionExpression>(number_true);
				for (auto& elem : simElems)
					returnVal->elems_.emplace_back(elem);
				return returnVal;
			}
		}
		virtual std::unique_ptr<IExpression> Clone() const override
		{
			auto val = std::make_unique<DisjunctionExpression>(baseValue_);
			for (auto& elem : elems_)
				val->elems_.emplace_back(elem);
			return std::move(val);
		}
		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			if (subExpression)
				stream << "(";
			bool first;
			if (isFalse(baseValue_))
				first = true;
			else
			{
				stream << number_true;
				first = false;
			}
			for (auto& elem : elems_)
			{
				bool positive = elem.IsNotInverse();
				if (first)
				{
					if (!positive)
						stream << "!";
					elem.GetExpression()->PrintCmdl(stream, true);
				}
				else
				{
					if (positive)
						stream << " || ";
					else
						stream << " || !";
					elem.GetExpression()->PrintCmdl(stream, true);
				}
				first = false;
			}
			if (subExpression)
				stream << ")";
		}
	};
}