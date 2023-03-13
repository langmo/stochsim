#pragma once
#include <memory>
#include "expression_common.h"
#include "NumberExpression.h"
namespace expression
{
	/// <summary>
	/// A variable expression represents the value of the corresponding variable.
	/// </summary>
	class FunctionExpression : public IExpression
	{
	public:
		FunctionExpression(identifier name, std::unique_ptr<IFunctionHolder> evalFunction = nullptr) : name_(name), evalFunction_(std::move(evalFunction))
		{
		}
		virtual number Eval() const override
		{
			if (evalFunction_)
			{
				std::vector<number> arguments;
				for (auto& elem : elems_)
				{
					arguments.push_back(elem->Eval());
				}
				try
				{
					return evalFunction_->operator()(arguments);
				}
				catch (const std::runtime_error& e)
				{
					std::stringstream errorMessage;
					errorMessage << "Error while evaluating function \"" << name_ << "\": "<<e.what();
					throw std::runtime_error(errorMessage.str().c_str());
				}
			}
			std::stringstream errorMessage;
			errorMessage << "Function with name \"" << name_ << "\" is unknown.";
			throw std::runtime_error(errorMessage.str().c_str());
		}
		virtual std::unique_ptr<IExpression> Simplify(const VariableRegister& variableRegister) const override
		{
			std::vector<std::unique_ptr<IExpression>> simElems;
			bool allNumbers = true;
			for (auto& elem : elems_)
			{
				auto simElem = elem->Simplify(variableRegister);
				if (!dynamic_cast<NumberExpression*>(simElem.get()))
				{
					allNumbers = false;
				}
				simElems.emplace_back(std::move(simElem));
			}
			if (allNumbers && evalFunction_ && !evalFunction_->IsMutable())
			{
				std::vector<number> arguments;
				for (auto& elem : simElems)
				{
					arguments.push_back(static_cast<NumberExpression*>(elem.get())->GetValue());
				}
				return std::make_unique<NumberExpression>(evalFunction_->operator()(arguments));
			}
			auto returnVal = std::make_unique<FunctionExpression>(name_, evalFunction_ ? evalFunction_->Clone() : nullptr);
			returnVal->elems_ = std::move(simElems);
			return std::unique_ptr<IExpression>(returnVal.release());
		}
		identifier GetName() const noexcept
		{
			return name_;
		}
		operator identifier() const noexcept
		{
			return name_;
		}
		void PushBack(std::unique_ptr<IExpression> expression)
		{
			elems_.push_back(std::move(expression));
		}

		virtual std::unique_ptr<IExpression> Clone() const override
		{
			auto val = std::make_unique<FunctionExpression>(name_, evalFunction_ ? evalFunction_->Clone() : nullptr);
			for (auto& elem : elems_)
				val->elems_.emplace_back(elem->Clone());
			return std::move(val);
		}

		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			stream << name_ << "(";
			bool first = true;
			for (auto& elem : elems_)
			{
				if (first)
					first = false;
				else
					stream << ", ";
				elem->PrintCmdl(stream, false);
			}
			stream << ")";
		}
		virtual void Bind(const BindingRegister& bindingRegister) override
		{
			auto newBinding = bindingRegister(name_ + "()");
			if (newBinding)
			{
				evalFunction_ = std::move(newBinding);
			}
			for (auto& elem : elems_)
			{
				elem->Bind(bindingRegister);
			}
		}
	private:
		identifier name_;
		std::unique_ptr<IFunctionHolder> evalFunction_;
		std::vector<std::unique_ptr<IExpression>> elems_;
	};
}