#pragma once
#include <memory>
#include "expression.h"
#include "number_expression.h"
namespace expression
{
	/// <summary>
	/// A variable expression represents the value of the corresponding variable.
	/// </summary>
	class variable_expression : public expression_base
	{
	public:
		variable_expression(identifier name, std::unique_ptr<function_holder_base> evalFunction = nullptr) : name_(name), evalFunction_(std::move(evalFunction))
		{
		}
		virtual number eval() const override
		{
			if (evalFunction_)
			{
				std::vector<number> empty;
				return evalFunction_->operator()(empty);
			}
			std::stringstream errorMessage;
			errorMessage << "Expression contains unbound variable with name \"" << name_ << "\".";
			throw std::exception(errorMessage.str().c_str());
		}
		virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
		{
			try
			{
				return variableLookup(name_)->simplify(variableLookup);
			}
			catch (...)
			{
				// do nothing, means variable is not defined in lookup.
			}
			if (evalFunction_ && !evalFunction_->is_mutable())
			{
				std::vector<number> empty;
				return std::make_unique<number_expression>(evalFunction_->operator()(empty));
			}
			return clone();
		}
		identifier get_identifier() const noexcept
		{
			return name_;
		}
		operator identifier() const noexcept
		{
			return name_;
		}
		virtual std::unique_ptr<expression_base> clone() const override
		{
			return std::make_unique<variable_expression>(name_, evalFunction_ ? evalFunction_->clone() : nullptr);
		}
		virtual void printCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			stream << name_;
		}
		virtual void bind(const binding_lookup& bindingLookup)
		{
			try
			{
				evalFunction_ = bindingLookup(name_);
			}
			catch (...)
			{
				// do nothing. There is simply no binding for this variable...
			}
		}
	private:
		identifier name_;
		std::unique_ptr<function_holder_base> evalFunction_;
	};
}