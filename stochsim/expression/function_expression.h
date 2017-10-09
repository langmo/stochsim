#pragma once
#include <memory>
#include "expression.h"
#include "number_expression.h"
namespace expression
{
	/// <summary>
	/// A variable expression represents the value of the corresponding variable.
	/// </summary>
	class function_expression : public expression_base
	{
	public:
		function_expression(identifier name, std::unique_ptr<function_holder_base> evalFunction = nullptr) : name_(name), evalFunction_(std::move(evalFunction))
		{
		}
		virtual number eval() const override
		{
			if (evalFunction_)
			{
				std::vector<number> arguments;
				for (auto& elem : elems_)
				{
					arguments.push_back(elem->eval());
				}
				return evalFunction_->operator()(arguments);
			}
			std::stringstream errorMessage;
			errorMessage << "Expression contains unbound function with name \"" << name_ << "\".";
			throw std::exception(errorMessage.str().c_str());
		}
		virtual std::unique_ptr<expression_base> simplify(const variable_lookup& variableLookup) const override
		{
			std::vector<std::unique_ptr<expression_base>> simElems;
			bool allNumbers = true;
			for (auto& elem : elems_)
			{
				auto simElem = elem->simplify(variableLookup);
				if (!dynamic_cast<number_expression*>(simElem.get()))
				{
					allNumbers = false;
				}
				simElems.emplace_back(std::move(simElem));
			}
			if (allNumbers && evalFunction_ && !evalFunction_->is_mutable())
			{
				std::vector<number> arguments;
				for (auto& elem : simElems)
				{
					arguments.push_back(static_cast<number_expression*>(elem.get())->get_number());
				}
				return std::make_unique<number_expression>(evalFunction_->operator()(arguments));
			}
			auto returnVal = std::make_unique<function_expression>(name_, evalFunction_->clone());
			returnVal->elems_ = std::move(simElems);
			return std::unique_ptr<expression_base>(returnVal.release());
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
			auto val = std::make_unique<function_expression>(name_, evalFunction_->clone());
			for (auto& elem : elems_)
				val->elems_.emplace_back(elem->clone());
			return std::move(val);
		}

		virtual void printCmdl(std::ostream& stream, bool subExpression) const noexcept override
		{
			stream << name_ << "(";
			bool first = true;
			for (auto& elem : elems_)
			{
				if (first)
					first = false;
				else
					stream << ", ";
				elem->printCmdl(stream, false);
			}
			stream << ")";
		}
		virtual void bind(const binding_lookup& bindingLookup)
		{
			try
			{
				evalFunction_ = bindingLookup(name_+"()");
			}
			catch (...)
			{
				// do nothing. There is simply no binding for this variable...
			}
		}
	private:
		identifier name_;
		std::unique_ptr<function_holder_base> evalFunction_;
		std::vector<std::unique_ptr<expression_base>> elems_;
	};
}