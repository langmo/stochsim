#pragma once
#include <memory>
#include "stochsim_common.h"
namespace stochsim
{
	/// <summary>
	/// Main class to run simulations.
	/// The idea is to construct a simulation by adding reactions and states to an object of this class. Once done, the simulation can be run using Simulation::run.
	/// </summary>
	class Simulation
	{
	public:
		explicit Simulation();
		virtual ~Simulation();
		/// <summary>
		/// Runs the simulation for maxTime time units.
		/// </summary>
		/// <param name="maxTime">Simulation time when simulation should stop. Simulation starts at simulation time zero.</param>
		virtual void Run(double maxTime);

		/// <summary>
		/// Creates a state of the given type and adds it to the set of states managed by this simulation. Equivalent to
		/// <code>
		///		Simulation sim;
		///		// ...
		///		std::shared_ptr&lt;StateClass&gt; state = make_shared&lt;StateClass&gt;(arguments...);
		///		sim.AddState(state);
		/// </code>
		/// <returns>The created state.</returns>
		/// </summary>
		template<class StateClass,
			class... ArgumentTypes> inline
			std::shared_ptr<StateClass> CreateState(ArgumentTypes&&... arguments)
		{
			std::shared_ptr<StateClass> state = std::make_shared<StateClass>(std::forward<ArgumentTypes>(arguments)...);
			AddState(state);
			return state;
		}
		/// <summary>
		/// Creates a propensity or delayed reaction of the given type and adds it to the set of reactions managed by this simulation. Equivalent to
		/// <code>
		///		Simulation sim;
		///		// ...
		///		std::shared_ptr&lt;ReactionClass&gt; state = make_shared&lt;ReactionClass&gt;(arguments...);
		///		sim.AddReaction(state);
		/// </code>
		/// <returns>The created reaction.</returns>
		/// </summary>
		template<class ReactionClass,
			class... ArgumentTypes> inline
			std::shared_ptr<ReactionClass> CreateReaction(ArgumentTypes&&... arguments)
		{
			std::shared_ptr<ReactionClass> reaction = std::make_shared<ReactionClass>(std::forward<ArgumentTypes>(arguments)...);
			AddReaction(reaction);
			return reaction;
		}

		/// <summary>
		/// Adds a propensity reaction externally created (i.e. not with Simulation::CreateReaction) to be managed by the simulation.
		/// Only reactions managed by the simulation will fire when the simulation runs. Also, all states belonging to the reaction must also be managed by the simulation.
		/// </summary>
		/// <param name="reaction">Reaction to add.</param>
		virtual void AddReaction(std::shared_ptr<IPropensityReaction> reaction);
		/// <summary>
		/// Adds a delayed reaction externally created (i.e. not with Simulation::CreateReaction) to be managed by the simulation.
		/// Only reactions managed by the simulation will fire when the simulation runs. Also, all states belonging to the reaction must also be managed by the simulation.
		/// </summary>
		/// <param name="reaction">Reaction to add.</param>
		virtual void AddReaction(std::shared_ptr<IEventReaction> reaction);
		/// <summary>
		/// Adds a state externally created (i.e. not with Simulation::CreateState) to be managed by the simulation. Every state modified by any reaction in the simulation must also be managed by the simulation.
		/// </summary>
		/// <param name="reaction">State to add.</param>
		virtual void AddState(std::shared_ptr<IState> state);
		/// <summary>
		/// Returns the state with the given name, or nullptr if a state with the name is not yet defined in the simulation.
		/// </summary>
		/// <param name="name">name of state</param>
		/// <returns></returns>
		virtual const std::shared_ptr<IState> GetState(const std::string& name) const;

		/// <summary>
		/// Returns all states defined in this simulation.
		/// </summary>
		/// <returns>Collection of all states.</returns>
		virtual const Collection<std::shared_ptr<IState>> GetStates() const;

		/// <summary>
		/// Returns the propensity reaction with the given name, or nullptr if a propensity reaction with the name is not yet defined in the simulation.
		/// </summary>
		/// <param name="name">name of reaction</param>
		/// <returns></returns>
		virtual const std::shared_ptr<IPropensityReaction> GetPropensityReaction(const std::string& name) const;

		/// <summary>
		/// Returns all propensity reactions defined in this simulation.
		/// </summary>
		/// <returns>Collection of all propensity reactions.</returns>
		virtual const Collection<std::shared_ptr<IPropensityReaction>>  GetPropensityReactions() const;

		/// <summary>
		/// Returns the event reaction with the given name, or nullptr if an event reaction with the name is not yet defined in the simulation.
		/// </summary>
		/// <param name="name">name of reaction</param>
		/// <returns></returns>
		virtual const std::shared_ptr<IEventReaction> GetEventReaction(const std::string& name) const;

		/// <summary>
		/// Returns all event reactions defined in this simulation.
		/// </summary>
		/// <returns>Collection of all event reactions.</returns>
		virtual const Collection<std::shared_ptr<IEventReaction>>  GetEventReactions() const;


		/// <summary>
		/// Adds a logger to the simulation monitoring the progress of a simulation and e.g. writing it to a file. This logger is called every time the simulation time exceeds the log period.
		/// </summary>
		/// <param name="task">Logger to add.</param>
		virtual void AddLogger(std::shared_ptr<ILogger> logger);
		/// <summary>
		/// Sets the time period of logging. Default = 0.1.
		/// </summary>
		/// <param name="logPeriod">Log period in simulation time units</param>
		virtual void SetLogPeriod(double logPeriod);
		/// <summary>
		/// Returns the time period of logging. Default = 0.1.
		/// </summary>
		/// <returns>Log period in simulation time units</returns>
		virtual double GetLogPeriod() const;
		/// <summary>
		/// Sets the folder under which the results of the simulation should be saved. An additional sub-folder is created
		/// with the name indicating the current date and time to prevent overwriting old simulation results if IsUniqueSubfolder()==true.
		/// </summary>
		/// <param name="baseFolder">Base folder where simulation results are saved.</param>
		virtual void SetBaseFolder(std::string baseFolder);
		/// <summary>
		/// Returns the folder under which the results of the simulation should be saved. An additional sub-folder is created
		/// with the name indicating the current date and time to prevent overwriting old simulation results if IsUniqueSubfolder()==true.
		/// </summary>
		/// <returns>Base folder where simulation results are saved.</returns>
		virtual std::string GetBaseFolder() const;
		/// <summary>
		/// Set to true to create an additional sub-folder under the base folder with the name indicating the current date and time to prevent overwriting old simulation results.
		/// </summary>
		/// <param name="uniqueSubFolder">True if sub-folder should be created, false if results should be saved directly in the base folder.</param>
		virtual void SetUniqueSubfolder(bool uniqueSubFolder);
		/// <summary>
		/// Returns true if an additional sub-folder under the base folder is created with the name indicating the current date and time to prevent overwriting old simulation results.
		/// </summary>
		/// <returns>True if sub-folder is created, false if results are saved directly in the base folder.</returns>
		virtual bool IsUniqueSubfolder() const;
		/// <summary>
		/// Creates a logger monitoring the state of the simulation and adds it to this simulation. Same as
		/// <code>
		/// Simulation sim;
		/// //...
		/// shared_pointer&lt;TaskClass&gt; logger = make_shared&lt;TaskClass&gt;(arguments...);
		/// sim.AddLogger(logger);
		/// </code>
		/// </summary>
		template<class TaskClass,
			class... ArgumentTypes> inline
			std::shared_ptr<TaskClass> CreateLogger(ArgumentTypes&&... arguments)
		{
			std::shared_ptr<TaskClass> logger = std::make_shared<TaskClass>(std::forward<ArgumentTypes>(arguments)...);
			AddLogger(logger);
			return logger;
		}

	private:
		// Make this object be non-copyable
		Simulation(const Simulation&) = delete;
		Simulation& operator=(const Simulation&) = delete;

		class Impl;
		Impl* const impl_;
	};
}