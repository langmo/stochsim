#pragma once
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include "stochsim_common.h"
#include "expression_common.h"
#include "ExpressionParser.h"
#include "ExpressionHolder.h"
namespace stochsim
{
	/// <summary>
	/// A choice is a special kind of state. A call to Add does not increase the concentration of the choice. Instead, the boolean formula
	/// associated with this choice is evaluated. Depending on the outcome of this evaluation, either the concentrations of one or the other set of element states is increased according to their
	/// stochiometry. The same holds for Remove and Transform. 
	/// The typical use case is to use a Choice to implement conditionals for reactions. For example, a reaction might result in a product B with a certain probability, and in a product C with another, i.e.
	///      { B     if rand()>0.2
	/// A -> {
	///      { C     otherwise.
	/// This would then be implemented as a propensity reaction with A as the reactant, and a choice as the product. The boolean formula associated to the choice would be "rand()>0.2", the first set of
	/// products of the choice would contain only B with stochiometry 1, and the second set only C with stochiometry 1.
	/// The usage of a choice becomes specifically interesting when combined with reactions providing additional information (variables) which can be used in the boolean expression. For example, a delay
	/// reaction passes the number of how often its (sole) reactant was transformed.
	/// To be compatible with mass action kinetics, the concentration of a choice (Num()) is calculated as follows: First, the boolean expression is evaluated. Then,
	/// either only the elementsIfTrue, or the elementsIfFalse are considered based on the outcome of the evaluation of the condition. The return value is then the product
	/// of these elements, according to their stochiometry. That is, if the choice is between 2*A+C and 3*D, we return A*(A-1)*C if the choice condition evaluates to true, and D*(D-1)*(D-2) if not.
	/// </summary>
	class Choice :
		public IState
	{
	private:
		class Product
		{
		public:
			Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			std::array<ExpressionHolder, Molecule::size_> propertyExpressions_;
			Product(std::shared_ptr<IState> state, Stochiometry stochiometry, Molecule::PropertyExpressions propertyExpressions) noexcept : stochiometry_(stochiometry), state_(std::move(state))
			{
				for (size_t i = 0; i < Molecule::size_; i++)
				{
					propertyExpressions_[i].SetExpression(std::move(propertyExpressions[i]));
				}
			}
			inline void Initialize(ISimInfo& simInfo)
			{
				for (auto& propertyExpression : propertyExpressions_)
				{
					if (propertyExpression)
						propertyExpression.Initialize(simInfo);
				}
			}
			inline void Uninitialize(ISimInfo& simInfo)
			{
				for (auto& propertyExpression : propertyExpressions_)
				{
					if (propertyExpression)
						propertyExpression.Uninitialize(simInfo);
				}
			}
			inline Molecule operator() (ISimInfo& simInfo, const std::vector<Variable>& variables = {}) const
			{
				Molecule molecule;
				for (size_t i = 0; i < Molecule::size_; i++)
				{
					if (propertyExpressions_[i])
						molecule[i] = propertyExpressions_[i](simInfo, variables);
				}
				return molecule;
			}
		};

		typedef std::unordered_map<expression::identifier, std::unique_ptr<expression::number>> VariablesMap;
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of this choice. Since a choice is conveniently treated as a normal state to be able to make a choice whenever being able to make no choice (i.e. just increase the
		/// concentration of a given product), a choice also has to have a name which is not clashing with other state names.</param>
		/// <param name="condition">The boolean equation for this choice. The expression
		/// can contain any number of arbitrary variable names. These variables are automatically initialized to be zero. This value is only changed before an evaluation of the expression if the reaction responsible
		/// for triggering the choice (the reaction invoking the Add method) passes a varibale with the same name.</param>
		Choice(std::string name, std::unique_ptr<expression::IExpression> condition) : name_(std::move(name))
		{
			SetCondition(std::move(condition));
		}
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of this choice. Since a choice is conveniently treated as a normal state to be able to make a choice whenever being able to make no choice (i.e. just increase the
		/// concentration of a given product), a choice also has to have a name which is not clashing with other state names.</param>
		/// <param name="condition">The boolean equation for this choice as a string. The string is parsed immediately. If parsing fails e.g. due to a syntax error, an std::exception is thrown. The expression
		/// can contain any number of arbitrary variable names. These variables are automatically initialized to be zero. This value is only changed before an evaluation of the expression if the reaction responsible
		/// for triggering the choice (the reaction invoking the Add method) passes a varibale with the same name.</param>
		Choice(std::string name, std::string condition) : name_(std::move(name))
		{
			SetCondition(std::move(condition));
		}
		/// <summary>
		/// A choice is not really a state, but only implements the state interface such that it can be added as a reactant or product of any reaction.
		/// Instead of the molecular number of the choice, we here evaluate the choice condition, and return a value which is compatible with mass action kinetics.
		/// That is, if the choice is between 2*A+C and 3*D, we return A*(A-1)*C if the choice condition evaluates to true, and D*(D-1)*(D-2) if not.
		/// </summary>
		/// <returns>Returns value compatible to mass action kinetics.</returns>
		virtual size_t Num(ISimInfo& simInfo) const override
		{
			return 0;
		}
		virtual void Add(ISimInfo& simInfo, const Molecule& molecule = defaultMolecule, const Variables& variables = {}) override
		{
			if (!addListeners_.empty())
			{
				double time = simInfo.GetSimTime();
				for (auto& addListener : addListeners_)
				{
					addListener(molecule, time);
				}
			}

			// Find out which choice was made by evaluating the formula with the current variable values.
			expression::number choice = choiceEquation_(simInfo, variables);
			
			// Depending of the choice, either increase one or the other sets of products.
			if (choice != 0)
			{
				for (const auto& product : elementsIfTrue_)
				{
					Molecule molecule = product(simInfo, variables);
					for (size_t i = 0; i < product.stochiometry_; i++)
					{
						product.state_->Add(simInfo, molecule, variables);
					}
				}
			}
			else
			{
				for (const auto& product : elementsIfFalse_)
				{
					Molecule molecule = product(simInfo, variables);
					for (size_t i = 0; i < product.stochiometry_; i++)
					{
						product.state_->Add(simInfo, molecule, variables);
					}
				}
			}
		}
		virtual const Molecule& Peak(ISimInfo& simInfo) const override
		{
			throw std::exception("Choices must only be used as products of a reaction, not as reactants (i.e. Peak must not be called).");
		}
		virtual Molecule Remove(ISimInfo& simInfo, const Variables& variables = {}) override
		{
			throw std::exception("Choices must only be used as products of a reaction, not as reactants (i.e. Remove must not be called).");
		}
		virtual Molecule& Transform(ISimInfo& simInfo, const Variables& variables = {}) override
		{
			throw std::exception("Choices must only be used as products of a reaction, not as transformees (i.e. Transform must not be called).");
		}
		virtual void Initialize(ISimInfo& simInfo) override
		{
			for (auto& product : elementsIfTrue_)
			{
				product.Initialize(simInfo);
			}
			for (auto& product : elementsIfFalse_)
			{
				product.Initialize(simInfo);
			}
			if (choiceEquation_)
				choiceEquation_.Initialize(simInfo);
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			if (choiceEquation_)
				choiceEquation_.Uninitialize(simInfo);
			for (auto& product : elementsIfTrue_)
			{
				product.Uninitialize(simInfo);
			}
			for (auto& product : elementsIfFalse_)
			{
				product.Uninitialize(simInfo);
			}
		}
		virtual std::string GetName() const noexcept override
		{
			return name_;
		}

		/// <summary>
		/// Adds a species as a product of the Choice, which gets increased according to its stochiometry when the Add method of the Choice is invoked, and if the boolean expression associated to this Choice evaluates to true.
		/// </summary>
		/// <param name="state">Species to add as a product when the boolean expression evaluates to true.</param>
		/// <param name="stochiometry">Number of molecules produced when the boolean expression evaluates to true.</param>
		void AddProductIfTrue(std::shared_ptr<IState> state, Stochiometry stochiometry = 1, Molecule::PropertyExpressions propertyExpressions = Molecule::PropertyExpressions())
		{
			for (auto& product : elementsIfTrue_)
			{
				if (state == product.state_)
				{
					for (auto i = 0; i < propertyExpressions.size(); i++)
					{
						std::string expressionOld("<none>");
						if (product.propertyExpressions_[i])
							expressionOld = product.propertyExpressions_[i].GetExpression()->ToCmdl();
						std::string expressionNew("<none>");
						if (propertyExpressions[i])
							expressionNew = propertyExpressions[i]->ToCmdl();
						if (expressionOld != expressionNew)
						{
							std::stringstream errorMessage;
							errorMessage << "Property " << std::to_string(i) << " of product " << state->GetName() << " in choice " << GetName() << " was already assigned to the expression " << expressionOld << ". Cannot re-assign it to the expression " << expressionNew << ".";
							throw std::exception(errorMessage.str().c_str());
						}
					}
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			elementsIfTrue_.emplace_back(state, stochiometry, std::move(propertyExpressions));
		}

		/// <summary>
		/// Adds a species as a product of the Choice, which gets increased according to its stochiometry when the Add method of the Choice is invoked, and if the boolean expression associated to this Choice evaluates to false.
		/// </summary>
		/// <param name="state">Species to add as a product when the boolean expression evaluates to false.</param>
		/// <param name="stochiometry">Number of molecules produced when the boolean expression evaluates to false.</param>
		void AddProductIfFalse(std::shared_ptr<IState> state, Stochiometry stochiometry = 1, Molecule::PropertyExpressions propertyExpressions = Molecule::PropertyExpressions())
		{
			for (auto& product : elementsIfFalse_)
			{
				if (state == product.state_)
				{
					for (auto i = 0; i < propertyExpressions.size(); i++)
					{
						std::string expressionOld("<none>");
						if (product.propertyExpressions_[i])
							expressionOld = product.propertyExpressions_[i].GetExpression()->ToCmdl();
						std::string expressionNew("<none>");
						if (propertyExpressions[i])
							expressionNew = propertyExpressions[i]->ToCmdl();
						if (expressionOld != expressionNew)
						{
							std::stringstream errorMessage;
							errorMessage << "Property " << std::to_string(i) << " of product " << state->GetName() << " in choice " << GetName() << " was already assigned to the expression " << expressionOld << ". Cannot re-assign it to the expression " << expressionNew << ".";
							throw std::exception(errorMessage.str().c_str());
						}
					}
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			elementsIfFalse_.emplace_back(state, stochiometry, std::move(propertyExpressions));
		}

		/// <summary>
		/// Returns all products and their stochiometries in the case the expression associated to this choice evaluates to true.
		/// </summary>
		/// <returns>Products of the reaction if expression evaluates to true.</returns>
		stochsim::Collection<stochsim::ReactionRightElement> GetProductsIfTrue() const noexcept
		{
			stochsim::Collection<stochsim::ReactionRightElement> returnVal;
			for (auto& product : elementsIfTrue_)
			{
				Molecule::PropertyExpressions expressions;
				for (size_t i=0; i < product.propertyExpressions_.size(); i++)
				{
					expressions[i] = product.propertyExpressions_[i]? product.propertyExpressions_[i].GetExpression()->Clone() : nullptr;
				}
				returnVal.emplace_back(product.state_, product.stochiometry_, std::move(expressions));
			}
			return std::move(returnVal);
		}

		/// <summary>
		/// Returns all products and their stochiometries in the case the expression associated to this choice evaluates to false.
		/// </summary>
		/// <returns>Products of the reaction if expression evaluates to false.</returns>
		stochsim::Collection<stochsim::ReactionRightElement> GetProductsIfFalse() const noexcept
		{
			stochsim::Collection<stochsim::ReactionRightElement> returnVal;
			for (auto& product : elementsIfFalse_)
			{
				Molecule::PropertyExpressions expressions;
				for (size_t i = 0; i < product.propertyExpressions_.size(); i++)
				{
					expressions[i] = product.propertyExpressions_[i] ? product.propertyExpressions_[i].GetExpression()->Clone() : nullptr;
				}
				returnVal.emplace_back(product.state_, product.stochiometry_, std::move(expressions));
			}
			return std::move(returnVal);
		}

		/// <summary>
		/// Returns the equation determining for which set of products the concentration is increased according to the stochiometry.
		/// </summary>
		/// <returns>Choice equation.</returns>
		const expression::IExpression* GetCondition() const noexcept
		{
			return choiceEquation_.GetExpression();
		}
		/// <summary>
		/// Sets the equation determining for which set of products the concentration is increased according to the stochiometry.
		/// </summary>
		/// <param name="choiceEquation">Choice equation.</param>
		void SetCondition(std::unique_ptr<expression::IExpression> choiceEquation) noexcept
		{
			choiceEquation_.SetExpression(std::move(choiceEquation));
		}
		/// <summary>
		/// Sets the equation determining for which set of products the concentration is increased according to the stochiometry.
		/// The provided string is parsed to an IExpression, i.e. an internal representation of the formula. If this parsing fails, e.g. due to
		/// a syntax error in the equation, an std::exception is thrown.
		/// </summary>
		/// <param name="choiceEquation">Choice equation as a string.</param>
		void SetCondition(std::string choiceEquation) noexcept
		{
			expression::ExpressionParser parser;
			SetCondition(parser.Parse(choiceEquation, false, false));
		}
		
		virtual inline void AddIncreaseListener(StateListener stateListener) override
		{
			addListeners_.push_back(std::move(stateListener));
		}
		virtual inline void AddDecreaseListener(StateListener stateListener) override
		{
			// do nothing. The molecular number of choices is always zero, and thus the state cannot be decreased.
		}

	private:
		const std::string name_;
		ExpressionHolder choiceEquation_;
		std::vector<Product> elementsIfTrue_;
		std::vector<Product> elementsIfFalse_;
		std::list<StateListener> addListeners_;
	};
}
