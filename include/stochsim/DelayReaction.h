#pragma once
#include "ComposedState.h"
#include <memory>
#include "stochsim_common.h"
#include <vector>
#include <sstream>
#include "ExpressionHolder.h"
namespace stochsim
{
	/// <summary>
	/// A reaction which fires at a specific time (instead of having a propensity), with the time when the reaction fires next being determined by the properties of the first molecule of a ComplexState.
	/// Since the first molecule of a complex state is also the oldest molecule, this type of reaction typically represents a reaction firing a fixed delay after a molecule of a given species was created.
	/// </summary>
	class DelayReaction : public IEventReaction
	{
	private:
		class Reactant
		{
		public:
			const std::shared_ptr<ComposedState> state_;
			const Molecule::PropertyNames propertyNames_;
			Reactant(std::shared_ptr<ComposedState> state, Molecule::PropertyNames propertyNames) noexcept : state_(std::move(state)), propertyNames_(std::move(propertyNames))
			{
			}
			inline void Initialize(ISimInfo& simInfo)
			{
				// do nothing.
			}
			inline void Uninitialize(ISimInfo& simInfo)
			{
				// do nothing.
			}
		};
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
	public:
		DelayReaction(std::string name, double delay, std::shared_ptr<ComposedState> reactant, Molecule::PropertyNames propertyNames = Molecule::PropertyNames()) : reactant_(std::move(reactant), std::move(propertyNames)), delay_(delay), name_(std::move(name))
		{
		}
		virtual double NextReactionTime(ISimInfo& simInfo) const override
		{
			return reactant_.state_->Num(simInfo) > 0 ? reactant_.state_->PeakFirstCreationTime(simInfo) + delay_ : stochsim::inf;
		}
		virtual void Fire(ISimInfo& simInfo) override
		{
			Variables variables;
			Molecule molecule = reactant_.state_->RemoveFirst(simInfo);
			for (size_t p = 0; p < molecule.Size(); p++)
			{
				if (!reactant_.propertyNames_[p].empty())
				{
					variables.push_back(Variable(reactant_.propertyNames_[p], molecule[p]));

				}
			}
			for (const auto& product : products_)
			{
				molecule = product(simInfo, variables);
				for (size_t i = 0; i < product.stochiometry_; i++)
				{
					product.state_->Add(simInfo, molecule, variables);
				}
			}
		}
		virtual std::string GetName() const override
		{
			return name_;
		}
		virtual void Initialize(ISimInfo& simInfo) override
		{
			for (auto& product : products_)
			{
				product.Initialize(simInfo);
			}
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			for (auto& product : products_)
			{
				product.Uninitialize(simInfo);
			}
		}

		/// <summary>
		/// Returns the current delay of the reaction
		/// </summary>
		/// <returns>Current delay in simulation time units.</returns>
		double GetDelay() const
		{
			return delay_;
		}
		/// <summary>
		/// Sets the delay of the reaction.
		/// </summary>
		/// <param name="delay">Delay in simulation time units.</param>
		void SetDelay(double delay)
		{
			delay_ = delay;
		}


		/// <summary>
		/// Adds a species as a product of the reaction. When the reaction fires, its concentration is increased according to its stochiometry.
		/// </summary>
		/// <param name="state">Species to add as a product.</param>
		/// <param name="stochiometry">Number of molecules produced when the reaction fires.</param>
		/// <summary>
		/// Adds a species as a product of the reaction. When the reaction fires, its concentration is increased according to its stochiometry.
		/// </summary>
		/// <param name="state">Species to add as a product.</param>
		/// <param name="stochiometry">Number of molecules produced when the reaction fires.</param>
		void AddProduct(std::shared_ptr<IState> state, Stochiometry stochiometry = 1, Molecule::PropertyExpressions propertyExpressions = Molecule::PropertyExpressions())
		{
			for (auto& product : products_)
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
							errorMessage << "Property " << std::to_string(i) << " of product " << state->GetName() << " in reaction " << GetName() << " was already assigned to the expression " << expressionOld << ". Cannot re-assign it to the expression " << expressionNew << ".";
							throw std::exception(errorMessage.str().c_str());
						}
					}
					product.stochiometry_ += stochiometry;
					return;
				}
			}
			products_.emplace_back(state, stochiometry, std::move(propertyExpressions));
		}
		/// <summary>
		/// Returns all products of the reaction.
		/// </summary>
		/// <returns>Products of the reaction.</returns>
		stochsim::Collection<stochsim::ReactionRightElement> GetProducts() const
		{
			stochsim::Collection<stochsim::ReactionRightElement> returnVal;
			for (auto& product : products_)
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
		/// Returns the reactant of the reaction. A delayed reaction always has exactly one reactant with stochiometry one.
		/// </summary>
		/// <returns>Reactants of the reaction.</returns>
		stochsim::ReactionLeftElement GetReactant() const
		{
			Molecule::PropertyNames names;
			for (size_t i = 0; i < reactant_.propertyNames_.size(); i++)
			{
				names[i] = reactant_.propertyNames_[i];
			}
			return ReactionLeftElement(reactant_.state_, 1, std::move(names));
		}
		/// <summary>
		/// Returns all reactants of the reaction.  A delayed reaction always has exactly one reactant with stochiometry one.
		/// The return collection thus contains only one element, namely the one returned by GetReactant().
		/// This method is mainly intended to provide a common interface as compared to other reaction types.
		/// </summary>
		/// <returns>Reactants of reaction.</returns>
		stochsim::Collection<stochsim::ReactionLeftElement> GetReactants() const noexcept
		{
			stochsim::Collection<stochsim::ReactionLeftElement> returnVal;
			returnVal.push_back(GetReactant());
			return std::move(returnVal);
		}
	private:
		double delay_;
		Reactant reactant_;
		const std::string name_;
		std::vector<Product> products_;
	};
}

