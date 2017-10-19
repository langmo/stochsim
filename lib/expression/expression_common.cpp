#include <random>
#include "expression_common.h"
#include "NumberExpression.h"
namespace expression
{
	std::unordered_map<identifier, number> makeDefaultVariables() noexcept
	{
		std::unordered_map<identifier, number> defaultVariables;
		defaultVariables.emplace(identifier("true"), number_true);
		defaultVariables.emplace(identifier("false"), number_false);
		defaultVariables.emplace(identifier("pi"), number(3.141592653589793));
		defaultVariables.emplace(identifier("e"), number(2.718281828459046));
		defaultVariables.emplace(identifier("Navo"), number(6.02214085774e23));
		return std::move(defaultVariables);
	}

	std::unordered_map<identifier, std::unique_ptr<IFunctionHolder>> makeDefaultFunctions() noexcept
	{
		std::unordered_map<identifier, std::unique_ptr<IFunctionHolder>> defaultFunctions;

		defaultFunctions.emplace("min", makeFunctionHolder(
			(number(*)(number, number))(&fmin), false));
		defaultFunctions.emplace("max", makeFunctionHolder(
			(number(*)(number, number))(&fmax), false));
		defaultFunctions.emplace("mod", makeFunctionHolder(
			(number(*)(number, number))(&fmod), false));

		defaultFunctions.emplace("sin", makeFunctionHolder(
			(number(*)(number))(&sin), false));
		defaultFunctions.emplace("cos", makeFunctionHolder(
			(number(*)(number))&cos, false));
		defaultFunctions.emplace("tan", makeFunctionHolder(
			(number(*)(number))&tan, false));

		defaultFunctions.emplace("asin", makeFunctionHolder(
			(number(*)(number))(&asin), false));
		defaultFunctions.emplace("acos", makeFunctionHolder(
			(number(*)(number))&acos, false));
		defaultFunctions.emplace("atan", makeFunctionHolder(
			(number(*)(number))&atan, false));

		defaultFunctions.emplace("sinh", makeFunctionHolder(
			(number(*)(number))(&sinh), false));
		defaultFunctions.emplace("cosh", makeFunctionHolder(
			(number(*)(number))&cosh, false));
		defaultFunctions.emplace("tanh", makeFunctionHolder(
			(number(*)(number))&tanh, false));

		defaultFunctions.emplace("asinh", makeFunctionHolder(
			(number(*)(number))(&asinh), false));
		defaultFunctions.emplace("acosh", makeFunctionHolder(
			(number(*)(number))&acosh, false));
		defaultFunctions.emplace("atanh", makeFunctionHolder(
			(number(*)(number))&atanh, false));

		defaultFunctions.emplace("abs", makeFunctionHolder(
			(number(*)(number))&abs, false));

		defaultFunctions.emplace("ceil", makeFunctionHolder(
			(number(*)(number))&ceil, false));
		defaultFunctions.emplace("floor", makeFunctionHolder(
			(number(*)(number))&floor, false));
		defaultFunctions.emplace("round", makeFunctionHolder(
			(number(*)(number))&round, false));

		defaultFunctions.emplace("erf", makeFunctionHolder(
			(number(*)(number))&erf, false));
		defaultFunctions.emplace("exp", makeFunctionHolder(
			(number(*)(number))&exp, false));
		defaultFunctions.emplace("exp2", makeFunctionHolder(
			(number(*)(number))&exp2, false));
		defaultFunctions.emplace("log", makeFunctionHolder(
			(number(*)(number))&log, false));
		defaultFunctions.emplace("log10", makeFunctionHolder(
			(number(*)(number))&log10, false));
		defaultFunctions.emplace("log2", makeFunctionHolder(
			(number(*)(number))&log2, false));

		defaultFunctions.emplace("pow", makeFunctionHolder(
			(number(*)(number, number))&pow, false));
		defaultFunctions.emplace("sqrt", makeFunctionHolder(
			(number(*)(number))&sqrt, false));

		defaultFunctions.emplace("rand", makeFunctionHolder(
			static_cast<std::function<number()>>(
				[]() -> number
		{
			static std::default_random_engine randomEngine(std::random_device{}());
			static std::uniform_real<number> randomUniform;
			return randomUniform(randomEngine);
		}
		), true));
		return std::move(defaultFunctions);
	}


}