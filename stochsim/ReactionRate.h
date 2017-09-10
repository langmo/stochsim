#pragma once
#include "stochsim_interfaces.h"
#include "muParser.h"
#include <string>
#include <memory>
namespace stochsim
{
	class ReactionRate
	{
	public:
		ReactionRate(std::string rateEquation, std::vector<const std::shared_ptr<IState>> reactants) : rateEquation_(std::move(rateEquation)), reactants_(std::move(reactants))
		{

		}
	private:
		std::vector<const std::shared_ptr<IState>> reactants_;
		const std::string rateEquation_;
		mu::Parser reactionRateParser_;
	};
}