#include <memory>

#include "example1.h"
#include "Parameters.h"

#include "State.h"
#include "ComposedState.h"
#include "PropensityReaction.h"
#include "Simulation.h"
#include "StateLogger.h"
#include "DelayReaction.h"
#include "ProgressLogger.h"
#include "CustomLogger.h"
#include <iostream>
// Anonymous namespace to avoid name collisions with other examples.
namespace
{
	using namespace stochsim;
	struct InfectedBacterium
	{
		unsigned long numInfections;
		double firstInfectionTime;
	};
	struct LysingBacterium
	{
		double fateDecisionTime;
	};
	/// <summary>
	/// Initial conditions.
	/// Note that these values are given in #molecules/ml, and have to be converted to absolute numbers
	/// by multiplying them with the volume. To emphasize this, values are given as doubles not as integers even in the cases
	/// where they could be integers.
	/// </summary>
	struct InitialConditions
	{
		/// <summary>
		/// RM-, non-lysogenic, not infected [ml^-1]
		/// </summary>
		static constexpr double B0m = 4e6;
		/// <summary>
		/// RM-, non-lysogenic, infected, fate not yet determined [ml^-1]
		/// </summary>
		static constexpr double B0i = 0;
		/// <summary>
		/// RM-, non-lysogenic, infected, fate decided to lyse [ml^-1]
		/// </summary>
		static constexpr double B0l = 0;
		/// <summary>
		/// RM-, lysogenic [ml^-1]
		/// </summary>
		static constexpr double B0p = 0;
		/// <summary>
		/// phage, non-modified [ml^-1]
		/// </summary>
		static constexpr double Pm = 1e5;
	};
	void run(std::string folder)
	{
		// Make parameters & IC easier to access.
		typedef Parameters p;
		typedef InitialConditions ic;
		typedef std::vector<unsigned long>::size_type MoiSize;

		// Volume
		constexpr double V = 1.; // [ml]
		// Simulation time 
		constexpr double runtime = 12; // [h]
		// Logging period of 1min
		constexpr double logPeriod = 1. / 60; // [h]
		// Estimated maximal MOI
		constexpr MoiSize maxMoi = 20;

		// Construct simulation
		Simulation sim;			
								
		// find out maximal capacity for complex states
		unsigned long initialCapcity = static_cast<size_t>(1e6);

		/*********
		* STATES *
		*********/
		// RM-, non-lysogenic, not infected
		std::shared_ptr<State> B0m = sim.CreateState<State>("B0m", static_cast<unsigned long>(std::round(ic::B0m*V)));

		// RM-, non-lysogenic, infected, fate not yet determined
		auto B0i_initializer = [](InfectedBacterium& bacterium, double time) {
			bacterium.numInfections = 1;
			bacterium.firstInfectionTime = time;
		};
		auto B0i_modifier = [](InfectedBacterium& bacterium, double time) {
			bacterium.numInfections++;;
		};
		std::shared_ptr<ComposedState<InfectedBacterium>> B0i = sim.CreateState<ComposedState<InfectedBacterium>>("B0i", static_cast<unsigned long>(std::round(ic::B0i*V)), B0i_initializer, B0i_modifier, initialCapcity);
		// RM-, non-lysogenic, infected, fate decided to lyse
		auto B0l_initializer = [](LysingBacterium& bacterium, double time)
		{
			bacterium.fateDecisionTime = time;
		};
		auto B0l_modifier = [](LysingBacterium& bacterium, double time) {
			// do nothing
		};
		std::shared_ptr<ComposedState<LysingBacterium>> B0l = sim.CreateState<ComposedState<LysingBacterium>>("B0l", static_cast<unsigned long>(std::round(ic::B0l*V)), B0l_initializer, B0l_modifier, initialCapcity);

		// RM-, lysogenic
		std::shared_ptr<State> B0p = sim.CreateState<State>("B0p", static_cast<unsigned long>(std::round(ic::B0p*V)));

		// phage, non - modified
		std::shared_ptr<State> Pm = sim.CreateState<State>("Pm", static_cast<unsigned long>(std::round(ic::Pm*V)));

		/************
		* REACTIONS *
		************/
		// B0m -> 2 B0m
		auto B0m_doubling = sim.CreateReaction<PropensityReaction>("B0m -> 2 B0m", p::v_max);
		B0m_doubling->AddModifier(B0m);
		B0m_doubling->AddProduct(B0m);
			
		// B0p -> 2 B0p
		auto B0p_doubling = sim.CreateReaction<PropensityReaction>("B0p -> 2 B0p", p::v_max);
		B0p_doubling->AddModifier(B0p, 1);
		B0p_doubling->AddProduct(B0p);
			

		// B0m + Pm -> B0i
		auto B0m_infection = sim.CreateReaction<PropensityReaction>("B0m + Pm -> B0i", p::delta / V);
		B0m_infection->AddReactant(B0m);
		B0m_infection->AddReactant(Pm);
		B0m_infection->AddProduct(B0i);

		// B0i[i] + Pm -> B0i[i+1]
		auto B0i_infection = sim.CreateReaction<PropensityReaction>("B0i[i] + Pm -> B0i[i+1]", p::delta / V);
		B0i_infection->AddTransformee(B0i);
		B0i_infection->AddReactant(Pm);
		
		// B0p + Pm -> B0p
		auto B0p_infection = sim.CreateReaction<PropensityReaction>("B0p + Pm -> B0p", p::delta / V);
		B0p_infection->AddModifier(B0p);
		B0p_infection->AddReactant(Pm);

		// B0l + Pm -> B0l
		auto B0l_infection = sim.CreateReaction<PropensityReaction>("B0l + Pm -> B0l", p::delta / V);
		B0l_infection->AddModifier(B0l);
		B0l_infection->AddReactant(Pm);

		// B0i -(t_moi)-> B0p||B0l
		std::vector<unsigned long> mois(maxMoi);
		auto B0i_fate_fireTime = [] (InfectedBacterium& bacterium)-> double
		{
			return bacterium.firstInfectionTime + Parameters::moiPeriod;
		};
		auto B0i_fate_fireAction = [B0i,B0l,B0p, &mois](InfectedBacterium& bacterium, ISimInfo& simInfo)
		{
			while (bacterium.numInfections > mois.size())
			{
				mois.resize(2 * mois.size());
			}
			mois[bacterium.numInfections - 1]++;

			B0i->Remove(simInfo);
			if (simInfo.Rand() < Parameters::alpha)
				B0p->Add(simInfo);
			else
				B0l->Add(simInfo);
		};
		auto B0i_fate = sim.CreateReaction<DelayReaction<InfectedBacterium>>("B0i -(t_moi)-> B0p||B0l", B0i, B0i_fate_fireTime, B0i_fate_fireAction);

		// B0l -(t_lag-t_moi)-> beta*Pm
		auto B0l_lysis_fireTime = [](LysingBacterium& bacterium)-> double
		{
			return bacterium.fateDecisionTime + Parameters::lysisPeriod;
		};
		auto B0l_lysis_fireAction = [B0l,Pm](LysingBacterium& bacterium, ISimInfo& simInfo)
		{
			B0l->Remove(simInfo);
			Pm->Add(simInfo, Parameters::beta);
		};
		auto B0l_lysis = sim.CreateReaction<DelayReaction<LysingBacterium>>("B0l -(t_lag-t_moi)-> beta*Pm", B0l, B0l_lysis_fireTime, B0l_lysis_fireAction);

		// B0p -> B0l
		auto B0p_fate = sim.CreateReaction<PropensityReaction>("B0p -> B0l", p::xi);
		B0p_fate->AddReactant(B0p);
		B0p_fate->AddProduct(B0l);

		/**********
		* LOGGING *
		**********/
		sim.SetBaseFolder(folder);
		sim.SetLogPeriod(logPeriod);
			
		// Logging state values
		sim.CreateLogger<StateLogger>("states.csv", B0m, B0i, B0l, B0p, Pm);

		// Logging MOI
		auto MOI_logFunc = [&mois](std::ostream& out, double time)
		{
			out << time;
			for (auto& moi : mois)
			{
				out << ", " << moi;
				moi = 0;
			}
			out << std::endl;
		};
		auto MOI_headerFunc = [&mois](std::ostream& out)
		{
			out << "Time";
			for (MoiSize i = 0; i < mois.size(); i++)
			{
				mois[i] = 0;
				out << ", MOI=" << (i+1);
			}
			out << std::endl;
		};
		sim.CreateLogger<CustomLogger>("mois.csv", MOI_headerFunc, MOI_logFunc);
			
		// Display simulation progress in console
		sim.CreateLogger<ProgressLogger>();

		sim.Run(runtime);
	}
}
Example example1()
{
	return { "batchinfect", "Infection of a bacterial batch culture with temperate phages", [](std::string folder) {run(folder); } };
}