#pragma once
#include "muParser.h"
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include "stochsim_interfaces.h"
namespace stochsim
{
	/// <summary>
	/// A choice is a special kind of state. Different to most or all other states, its concentration is always zero. The concentration of a choice can thus not be decreased, and an
	/// error is thrown if trying to call Remove. In contrary, Add can (and should) be called, but a call to Add does not increase the concentration of the choice. Instead, the boolean formula
	/// associated with this choice is evaluated. Depending on the outcome of this evaluation, either the concentrations of one or the other set of product states is increased according to their
	/// stochiometry.
	/// The typical use case is to use a Choice to implement conditionals for reactions. For example, a reaction might result in a product B with a certain probability, and in a product C with another, i.e.
	///      { B     if rand()>0.2
	/// A -> {
	///      { C     otherwise.
	/// This would then be implemented as a propensity reaction with A as the reactant, and a choice as the product. The boolean formula associated to the choice would be "rand()>0.2", the first set of
	/// products of the choice would contain only B with stochiometry 1, and the second set only C with stochiometry 1.
	/// The usage of a choice becomes specifically interesting when combined with reactions providing additional information (variables) which can be used in the boolean expression. For example, a delay
	/// reaction passes the number of how often its (sole) reactant was transformed.
	/// </summary>
	class Choice :
		public IState
	{
	private:
		/// <summary>
		/// Structure to store the information about the products of a Choice, as well as their stochiometries.
		/// </summary>
		struct ReactionElementWithModifiers
		{
		public:
			Stochiometry stochiometry_;
			const std::shared_ptr<IState> state_;
			ReactionElementWithModifiers(std::shared_ptr<IState> state, Stochiometry stochiometry) : stochiometry_(stochiometry), state_(std::move(state))
			{
			}
		};

		typedef std::unordered_map<std::string, std::unique_ptr<mu::value_type>> VariablesMap;
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of this choice. Since a choice is conveniently treated as a normal state to be able to make a choice whenever being able to make no choice (i.e. just increase the
		/// concentration of a given product), a choice also has to have a name which is not clashing with other state names.</param>
		/// <param name="choiceEquation">The boolean equation for this choice. To evaluate this equation we use muparser. Please see the documentation of muparser for the syntax of the expression. The expression
		/// can contain any number of arbitrary variable names. These variables are automatically initialized to be zero. This value is only changed before an evaluation of the expression if the reaction responsible
		/// for triggering the choice (the reaction invoking the Add method) passes a varibale with the same name.</param>
		Choice(std::string name, std::string choiceEquation) : name_(std::move(name)), choiceEquation_(choiceEquation), variables_(100)
		{
		}
		/// <summary>
		/// A choice is not really a state, but only implements the state interface such that it can be added as a product of any reaction which allows to add an arbitrary state as a product.
		/// Since, however, a choice does not really have a concentration, this function always returns zero. This prevents at least some possible exceptions, e.g. when (invalidly) adding a Choice as a
		/// reactant to a propensity reaction following mass action kinetics, this reaction will never fire since a zero concentration of the choice then implies zero propensity of the reaction.
		/// </summary>
		/// <returns>Returns always zero.</returns>
		virtual size_t Num() const override
		{
			// Special kind of state, which only forwards addition of molecules to one of two choices. Thus, its concentration is always zero.
			return 0;
		}
		virtual void Add(ISimInfo& simInfo, size_t num = 1, std::initializer_list<Variable> variables = {}) override
		{
			// Set parser variables
			for (auto& variable : variables)
			{
				auto& valuePtr = variables_[variable.first];
				if (!valuePtr)
				{
					valuePtr = std::make_unique<mu::value_type>(static_cast<mu::value_type>(variable.second));
				}
				else
				{
					*valuePtr = static_cast<mu::value_type>(variable.second);
				}
			}
			accessSimInfo(&simInfo);
			for (size_t i = 0; i < num; i++)
			{
				// Find out which choice was made by evaluating the formula with the current variable values.
				double choice;
				try
				{
					choice = static_cast<double>(parser_.Eval());
				}
				catch (mu::Parser::exception_type &e)
				{
					std::string message = transformString<mu::string_type, std::string>(e.GetMsg());

					std::stringstream errorMessage;
					errorMessage << "Could not evaluate expression \"" << choiceEquation_ << "\" for choice "<<name_<<": " << message;
					throw std::exception(errorMessage.str().c_str());
				}
				// Depnding of the choice, either increase one or the other sets of products.
				if (choice != 0)
				{
					for (const auto& product : productsIfTrue_)
					{
						product.state_->Add(simInfo, product.stochiometry_, variables);
					}
				}
				else
				{
					for (const auto& product : productsIfFalse_)
					{
						product.state_->Add(simInfo, product.stochiometry_, variables);
					}
				}
			}
		}
		virtual void Remove(ISimInfo& simInfo, size_t num = 1, std::initializer_list<Variable> variables = {}) override
		{
			throw std::exception("Trying to decrease the concentration of a Choice state. Choices must only be used on the RHS of a reaction, and their concentration must only be increased.");
		}
		virtual void Transform(ISimInfo& simInfo, size_t num = 1, std::initializer_list<Variable> variables = {}) override
		{
			// do nothing.
		}
		virtual void Initialize(ISimInfo& simInfo) override
		{
			variables_.clear();
			parser_.ClearVar();
			mu::string_type choiceEquationMu = transformString<std::string, mu::string_type>(choiceEquation_);
			try
			{
				accessSimInfo(&simInfo);
				parser_.DefineFun(transformString<std::string, mu::string_type>("rand"), Rand, false);
				parser_.SetVarFactory(CreateVariable, &variables_);
				parser_.SetExpr(choiceEquationMu);
			}
			catch (mu::Parser::exception_type &e)
			{
				std::string message = transformString<mu::string_type, std::string>(e.GetMsg());
				std::stringstream errorMessage;
				errorMessage << "Could not parse expression \""<< choiceEquation_ << "\" for choice " << name_ << ": " << message;
				throw std::exception(errorMessage.str().c_str());
			}
	
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			variables_.clear();
			parser_.ClearVar();
		}
		virtual std::string GetName() const override
		{
			return name_;
		}

		/// <summary>
		/// Adds a species as a product of the Choice, which gets increased according to its stochiometry when the Add method of the Choice is invoked, and if the boolean expression associated to this Choice evaluates to true.
		/// </summary>
		/// <param name="state">Species to add as a product when the boolean expression evaluates to true.</param>
		/// <param name="stochiometry">Number of molecules produced when the boolean expression evaluates to true.</param>
		void AddProductIfTrue(std::shared_ptr<IState> state, Stochiometry stochiometry = 1)
		{
			for (auto& product : productsIfTrue_)
			{
				if (state == product.state_)
				{
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			productsIfTrue_.emplace_back(state, stochiometry);
		}

		/// <summary>
		/// Adds a species as a product of the Choice, which gets increased according to its stochiometry when the Add method of the Choice is invoked, and if the boolean expression associated to this Choice evaluates to false.
		/// </summary>
		/// <param name="state">Species to add as a product when the boolean expression evaluates to false.</param>
		/// <param name="stochiometry">Number of molecules produced when the boolean expression evaluates to false.</param>
		void AddProductIfFalse(std::shared_ptr<IState> state, Stochiometry stochiometry = 1)
		{
			for (auto& product : productsIfFalse_)
			{
				if (state == product.state_)
				{
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			productsIfFalse_.emplace_back(state, stochiometry);
		}

		/// <summary>
		/// Returns all products and their stochiometries in the case the expression associated to this choice evaluates to true.
		/// </summary>
		/// <returns>Products of the reaction if expression evaluates to true.</returns>
		stochsim::Collection<stochsim::ReactionElement> GetProductsIfTrue() const
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& product : productsIfTrue_)
			{
				returnVal.emplace_back(product.state_, product.stochiometry_);
			}
			return std::move(returnVal);
		}

		/// <summary>
		/// Returns all products and their stochiometries in the case the expression associated to this choice evaluates to false.
		/// </summary>
		/// <returns>Products of the reaction if expression evaluates to false.</returns>
		stochsim::Collection<stochsim::ReactionElement> GetProductsIfFalse() const
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& product : productsIfFalse_)
			{
				returnVal.emplace_back(product.state_, product.stochiometry_);
			}
			return std::move(returnVal);
		}

		/// <summary>
		/// Returns the equation determining for which set of products the concentration is increased according to the stochiometry.
		/// </summary>
		/// <returns>Choice equation. For the syntax, see the documentation of muparser.</returns>
		std::string GetChoiceEquation() const
		{
			return choiceEquation_;
		}
		/// <summary>
		/// Sets the equation determining for which set of products the concentration is increased according to the stochiometry.
		/// </summary>
		/// <param name="choiceEquation">Choice equation. For the syntax, see the documentation of muparser.</param>
		void SetChoiceEquation(std::string choiceEquation)
		{
			choiceEquation_ = std::move(choiceEquation);
		}

	private:
		/// <summary>
		/// Callback function for the muparser, which is called when during parsing (not evaluating) an equation a variable name is discovered which is not yet declared. The task of this function is then to
		/// return a pointer to a double value representing the value of the variable. This pointer is then used later when evaluating the equation to determine the (current) value of the variable.
		/// Here, we simply save all found variables in an unordered map, with the parameter values initially initialized to zero. Before evaluation of the equation, we then set all variables in the map which
		/// we know of to their respective values. This implies that variables which appear in the formula but of which we have no clue that they exist (respectively, for which we don't have a current value)
		/// have a value of zero.
		/// </summary>
		/// <param name="a_szName">Name of the variable found by the muparser.</param>
		/// <param name="pUserData">User provided data. In our case, this is a pointer to an unordered map of name-value pairs (a pointer to a VariablesMap).</param>
		/// <returns>Pointer to a double holding the current value to the variable, which is initially zero.</returns>
		static mu::value_type* CreateVariable(const mu::char_type *a_szName, void *pUserData)
		{
			// We know that the user data is a pointer to VariablesMap, since we provide this in the call to SetVarFactory of the parser when we pass a reference to this function. 
			VariablesMap* map = static_cast<VariablesMap*>(pUserData);
			std::string varName = transformString<mu::string_type, std::string>(a_szName);
			// Initialize variable to zero.
			(*map)[varName] = std::make_unique<mu::value_type>(0);
			return ((*map)[varName]).get();
		}

		template<typename A, typename B> static B transformString(const A& orgString)
		{
			B returnVal;
			std::transform(orgString.begin(), orgString.end(), std::back_inserter(returnVal), [](A::value_type value) {return static_cast<B::value_type>(value); });
			return std::move(returnVal);
		}
		/// <summary>
		/// Implementation of a uniform random function used in muparser evaluations.
		/// Since this function is only called when muparser evaluates the expression, and since this only happens inside Add, and
		/// since then simInfoPtr_ is set, this always returnes a valid random number in the current implementation.
		/// We nevertheless check if simInfoPtr_ is a valid pointer to prevent errors occuring during refactoring.
		/// </summary>
		/// <returns>Uniformly distributed random number between zero and one.</returns>
		static mu::value_type Rand()
		{ 
			auto simInfoPtr = accessSimInfo();
			if (simInfoPtr)
				return static_cast<mu::value_type>(simInfoPtr->Rand());
			else
				throw mu::Parser::exception_type(transformString<std::string, mu::string_type>("accessSimInfo() points to null during calculation of a random number."));
		}

		/// <summary>
		/// Helper class to access simInfo in muparser functions. These functions have to be static, so we have to somewhere store simInfo. We do this by a static local variable in this static function.
		/// This circumvents the C++ restriction that static member functions must not be initialized in the header, thus avoinding to define a source file only for this one static variable...
		/// This is definitely ugly, but shouldn't be a problem when not used in a multithreaded environment. In a multithreaded environment, it might happen that the wrong simInfo is used to calculate
		/// a random variable. This might cause problems when the random number generator is seeded with a given value instead of a random one. Seeding the random number generator with a given value is,
		/// however, mainly useful for debugging. Thus, the cases when this induces an unexpected behavior (non-reproducibility of a realization even though seeding the random number generator with the same value)
		/// should be rather view.
		/// TODO: Check for a better multi-threaded solution...
		/// </summary>
		/// <param name="newVal">New value of simInfo.</param>
		/// <param name="set">Determines if the value of simInfo is set.</param>
		/// <returns>The current value of simInfo.</returns>
		static ISimInfo* accessSimInfo(ISimInfo* newVal = nullptr)
		{
			static ISimInfo* simInfo = nullptr;
			if (newVal)
			{
				simInfo = newVal;
			}
			return simInfo;
		}

	private:
		const std::string name_;
		std::string choiceEquation_;
		mu::Parser parser_;
		VariablesMap variables_;
		std::vector<ReactionElementWithModifiers> productsIfTrue_;
		std::vector<ReactionElementWithModifiers> productsIfFalse_;
	};
}
