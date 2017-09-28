#pragma once
#include "stochsim_interfaces.h"
#include "muParser.h"
#include <string>
#include <codecvt>
#include <memory>
namespace stochsim
{
	/// <summary>
	/// A reaction rate is a formula stored as a string, which can be evaluated to obtain the reaction rate as a double value.
	/// The string may contain typical math functions like min, sqrt and similar, as well as variables having the name of the reactants of the reaction.
	/// </summary>
	class ReactionRate
	{
	public:
		/// <summary>
		/// Default constructor. Before usage, Initialize(...) must be called.
		/// </summary>
		ReactionRate()
		{
			// do nothing.
		}
		/// <summary>
		/// Constructor automatically calling Initialize(...).
		/// Same as
		/// <code>
		/// auto rate = ReactionRate();
		/// rate.Initialize(rateEquation, reactants);
		/// </code>
		/// </summary>
		/// <param name="rateEquation">The equation this reaction rate implements.</param>
		/// <param name="reactants">The reactants of this rate. Only the names of these species might occur in the rate equation.</param>
		template<class E, class R>  ReactionRate(E rateEquation, R reactants)
		{
			Initialize(std::forward<E>(rateEquation), std::forward<R>(reactants));
		}
		/// <summary>
		/// Initializes the reaction rate.
		/// </summary>
		/// <param name="rateEquation">The equation this reaction rate implements.</param>
		/// <param name="reactants">The reactants of this rate. Only the names of these species might occur in the rate equation.</param>
		template<class E,class R> void Initialize(E rateEquation, R reactants) 
		{
			parser_.ClearVar();
			reactants_ = std::forward<R>(reactants);
			molecularNumbers_.resize(reactants_.size());
			for (std::vector<std::shared_ptr<IState>>::size_type i = 0; i < reactants_.size(); i++)
			{
				auto stateName = reactants_[i]->GetName();
				mu::string_type stateNameMu;
				std::transform(stateName.begin(), stateName.end(), std::back_inserter(stateNameMu), [](std::string::value_type value) {return static_cast<mu::string_type::value_type>(value); });
				try
				{
					parser_.DefineVar(stateNameMu, &molecularNumbers_[i]);
				}
				catch(mu::Parser::exception_type &e)
				{
					mu::string_type messageMu = e.GetMsg();
					std::string message;
					std::transform(messageMu.begin(), messageMu.end(), std::back_inserter(message), [](mu::string_type::value_type value) {return static_cast<std::string::value_type>(value); });

					std::stringstream errorMessage;
					errorMessage << "Could not set variable representing state "<<stateName<<" in parser for custom rate equation: " << message;
					throw std::exception(errorMessage.str().c_str());
				}
			}
			mu::string_type rateEquationMu;
			std::copy(rateEquation.begin(), rateEquation.end(), std::back_inserter(rateEquationMu));

			try
			{
				parser_.SetExpr(rateEquationMu);
			}
			catch (mu::Parser::exception_type &e)
			{
				mu::string_type messageMu = e.GetMsg();
				std::string message;
				std::transform(messageMu.begin(), messageMu.end(), std::back_inserter(message), [](mu::string_type::value_type value) {return static_cast<std::string::value_type>(value); });

				std::stringstream errorMessage;
				errorMessage << "Could not parse custom rate equation: " << message;
				throw std::exception(errorMessage.str().c_str());
			}
			
		}
		/// <summary>
		/// Calculates the current rate of the reaction by solving the rate equation with the current species concentrations.
		/// Must be called only after initialize has been called.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		/// <returns>The current rate of the reaction.</returns>
		double CalculateRate(ISimInfo& simInfo) const
		{
			try
			{
				for (std::vector<std::shared_ptr<IState>>::size_type i = 0; i < reactants_.size(); i++)
				{
					molecularNumbers_[i] = static_cast<mu::value_type>(reactants_[i]->Num());
				}
				return static_cast<double>(parser_.Eval());
			}
			catch (mu::Parser::exception_type &e)
			{
				mu::string_type messageMu = e.GetMsg();
				std::string message;
				std::transform(messageMu.begin(), messageMu.end(), std::back_inserter(message), [](mu::string_type::value_type value) {return static_cast<std::string::value_type>(value); });
				
				std::stringstream errorMessage;
				errorMessage << "Could not evaluate custom rate equation: "<< message;
				throw std::exception(errorMessage.str().c_str());
			}
		}
	private:
		std::vector<std::shared_ptr<IState>> reactants_;
		mutable std::vector<mu::value_type> molecularNumbers_;
		mu::Parser parser_;
	};
}