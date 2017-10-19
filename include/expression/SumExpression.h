#pragma once
#include <memory>
#include "expression_common.h"
#include "NumberExpression.h"
#include "UnaryMinusExpression.h"
namespace expression
{
	/// <summary>
	/// Variadic expression representing the sum of various elements.
	/// The inverse of an expression is thereby -expression. Thus, sum_expressions represent both sums and differences.
	/// </summary>
	class SumExpression : public IVariadicExpression
	{
	public:
		static constexpr number nullElement = 0;
	public:
		SumExpression(number baseValue = nullElement) : IVariadicExpression(baseValue)
		{
		}
		virtual number GetNullElement() const noexcept override
		{
			return nullElement;
		}
		virtual std::unique_ptr<IExpression> GetUnaryInverse(const IExpression* expression) const override
		{
			return std::make_unique<UnaryMinusExpression>(expression->Clone());
		}
		virtual number Eval() const override
		{
			number sum = baseValue_;
			for (auto& elem : elems_)
			{
				if (elem.IsNotInverse())
					sum += elem.GetExpression()->Eval();
				else
					sum -= elem.GetExpression()->Eval();
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
						value += static_cast<NumberExpression*>(simElem.get())->GetValue();
					}
					else
					{
						value -= static_cast<NumberExpression*>(simElem.get())->GetValue();
					}
				}
				else if (dynamic_cast<UnaryMinusExpression*>(simElem.get()))
				{
					simElems.emplace_back(elem.IsNotInverse(), static_cast<UnaryMinusExpression*>(simElem.get())->get_expression()->Clone());
				}
				else if (dynamic_cast<SumExpression*>(simElem.get()))
				{
					auto& subExp = *static_cast<SumExpression*>(simElem.get());
					value += elem.IsNotInverse() ? subExp.baseValue_ : -subExp.baseValue_;
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
			if (simElems.empty())
				return std::make_unique<NumberExpression>(value);
			else if (value == 0 && simElems.size() == 1)
			{
				if (simElems[0].IsNotInverse())
					return simElems[0].GetExpression()->Clone();
				else
					return GetUnaryInverse(simElems[0].GetExpression());
			}
			else
			{
				auto returnVal = std::make_unique<SumExpression>(value);
				for (auto& elem : simElems)
					returnVal->elems_.emplace_back(elem);
				return returnVal;
			}
		}
		virtual std::unique_ptr<IExpression> Clone() const override
		{
			auto val = std::make_unique<SumExpression>(baseValue_);
			for (auto& elem : elems_)
				val->elems_.emplace_back(elem);
			return std::move(val);
		}

		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			if (subExpression)
				stream << "(";
			bool first;
			if (baseValue_ == 0)
				first = true;
			else
			{
				stream << baseValue_;
				first = false;
			}
			for (auto& elem : elems_)
			{
				bool positive = elem.IsNotInverse();
				if (dynamic_cast<const NumberExpression*>(elem.GetExpression()))
				{
					auto value = static_cast<const NumberExpression*>(elem.GetExpression())->GetValue();
					if (value == 0)
						continue;
					else if (value < 0)
					{
						value *= -1;
						positive = !positive;
					}
					if (first)
					{
						if (!positive)
							stream << "-";
						stream << value;
					}
					else
					{
						if (positive)
							stream << " + ";
						else
							stream << " - ";
						stream << value;
					}
				}
				else
				{
					if (first)
					{
						if (!positive)
							stream << "-";
						elem.GetExpression()->PrintCmdl(stream, true);
					}
					else
					{
						if (positive)
							stream << " + ";
						else
							stream << " - ";
						elem.GetExpression()->PrintCmdl(stream, true);
					}
				}
				first = false;
			}

			if (subExpression)
				stream << ")";
		}

	};
}