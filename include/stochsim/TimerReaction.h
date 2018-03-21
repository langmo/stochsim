#pragma once
#include <memory>
#include "stochsim_common.h"
#include <vector>
#include <string>
#include "ExpressionHolder.h"
namespace stochsim
{
	/// <summary>
	/// A reaction which fires once at a specific time (instead of having a propensity).
	/// Good to implement events like adding some substrate at a given time.
	/// </summary>
	class TimerReaction : public IEventReaction
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
	public:
		TimerReaction(std::string name, double fireTime_) : fireTime_(fireTime_), name_(std::move(name)), hasFired_(false)
		{
		}
		/// <summary>
		/// Returns all products of the reaction.
		/// </summary>
		/// <returns>Products of the reaction.</returns>
		stochsim::Collection<stochsim::ReactionElement> GetProducts() const
		{
			stochsim::Collection<stochsim::ReactionElement> returnVal;
			for (auto& product : products_)
			{
				returnVal.emplace_back(product.state_, product.stochiometry_);
			}
			return std::move(returnVal);
		}
		virtual double NextReactionTime(ISimInfo& simInfo) const override
		{
			return !hasFired_ ? fireTime_ : stochsim::inf;
		}
		virtual void Fire(ISimInfo& simInfo) override
		{
			for (auto& product : products_)
			{
				Molecule molecule = product(simInfo);
				for (size_t i = 0; i < product.stochiometry_; i++)
				{
					product.state_->Add(simInfo, molecule);
				}
			}
			hasFired_ = true;
		}
		virtual std::string GetName() const override
		{
			return name_;
		}
		virtual void Initialize(ISimInfo& simInfo) override
		{
			hasFired_ = false;
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
		/// Returns the time when the reaction fires.
		/// </summary>
		/// <returns>Time when reaction fires in simulation time units.</returns>
		double GetFireTime() const
		{
			return fireTime_;
		}
		/// <summary>
		/// Sets the time when the reaction fires.
		/// </summary>
		/// <param name="delay">ime when reaction fires in simulation time units.</param>
		void SetFireTime(double fireTime)
		{
			fireTime_ = fireTime;
		}

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
	private:
		bool hasFired_;
		double fireTime_;
		const std::string name_;
		std::vector<Product> products_;
	};
}
