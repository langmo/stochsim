#pragma once
#include <memory>
#include "expression_common.h"
#include "NumberExpression.h"
namespace expression
{
	/// <summary>
	/// A variable expression represents the value of the corresponding variable.
	/// </summary>
	class VariableExpression : public IExpression
	{
	public:
		VariableExpression(identifier name, std::unique_ptr<IFunctionHolder> evalFunction = nullptr) : name_(name), evalFunction_(std::move(evalFunction))
		{
		}
		virtual number Eval() const override
		{
			if (evalFunction_)
			{
				std::vector<number> empty;
				return evalFunction_->operator()(empty);
			}
			std::stringstream errorMessage;
			errorMessage << "Expression contains unbound variable with name \"" << name_ << "\".";
			throw std::runtime_error(errorMessage.str().c_str());
		}
		virtual std::unique_ptr<IExpression> Simplify(const VariableRegister& variableRegister) const override
		{
			auto newVal = variableRegister(name_);
			if (newVal)
			{
				return newVal->Simplify(variableRegister);
			}
			
			if (evalFunction_ && !evalFunction_->IsMutable())
			{
				std::vector<number> empty;
				return std::make_unique<NumberExpression>(evalFunction_->operator()(empty));
			}
			return Clone();
		}
		identifier GetName() const noexcept
		{
			return name_;
		}
		operator identifier() const noexcept
		{
			return name_;
		}
		virtual std::unique_ptr<IExpression> Clone() const override
		{
			return std::make_unique<VariableExpression>(name_, evalFunction_ ? evalFunction_->Clone() : nullptr);
		}
		virtual void PrintCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			stream << name_;
		}
		virtual void Bind(const BindingRegister& bindingRegister) override
		{
			auto newFunction = bindingRegister(name_);
			if (newFunction)
			{
				evalFunction_ = std::move(newFunction);
			}
		}
	private:
		identifier name_;
		std::unique_ptr<IFunctionHolder> evalFunction_;
	};
}