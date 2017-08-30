#pragma once
#include <vector>
#include <memory>
#include <random>
#include "types.h"
namespace stochsim
{
	class Logger;
	/// <summary>
	/// Main class to run simulations.
	/// The idea is to construct a simulation by adding reactions and states to an object of this class. Once done, the simulation can be run using Simulation::run.
	/// </summary>
	class Simulation
	{
	public:
		explicit Simulation();
		~Simulation();
		/// <summary>
		/// Runs the simulation for maxTime time units.
		/// </summary>
		/// <param name="maxTime">Simulation time when simulation should stop. Simulation starts at simulation time zero.</param>
		void Run(double maxTime);

		/// <summary>
		/// Returns an object to configure when and what is logged during a simulation.
		/// </summary>
		/// <returns>Object to configure logging mechanism.</returns>
		Logger& GetLogger();

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
		void AddReaction(std::shared_ptr<PropensityReaction> reaction);
		/// <summary>
		/// Adds a delayed reaction externally created (i.e. not with Simulation::CreateReaction) to be managed by the simulation.
		/// Only reactions managed by the simulation will fire when the simulation runs. Also, all states belonging to the reaction must also be managed by the simulation.
		/// </summary>
		/// <param name="reaction">Reaction to add.</param>
		void AddReaction(std::shared_ptr<DelayedReaction> reaction);
		/// <summary>
		/// Adds a state externally created (i.e. not with Simulation::CreateState) to be managed by the simulation. Every state modified by any reaction in the simulation must also be managed by the simulation.
		/// </summary>
		/// <param name="reaction">State to add.</param>
		void AddState(std::shared_ptr<State> state);
		/// <summary>
		/// Returns the state with the given name, or nullptr if a state with the name is not yet defined in the simulation.
		/// </summary>
		/// <param name="name">name of state</param>
		/// <returns></returns>
		std::shared_ptr<State> GetState(const std::string& name);

		/// <summary>
		/// Returns the propensity reaction with the given name, or nullptr if a propensity reaction with the name is not yet defined in the simulation.
		/// </summary>
		/// <param name="name">name of reaction</param>
		/// <returns></returns>
		std::shared_ptr<PropensityReaction> GetPropensityReaction(const std::string& name);

	private:
		// Make this object be non-copyable
		Simulation(const Simulation&) = delete;
		Simulation& operator=(const Simulation&) = delete;

		class Impl;
		Impl *impl_;
	};

	class Logger
	{
	public:
		virtual ~Logger()
		{
		}
		/// <summary>
		/// Adds a task to the logger. This task is every time called the simulation time exceeds the log period.
		/// </summary>
		/// <param name="task">Task to add.</param>
		virtual void AddTask(std::shared_ptr<LoggerTask> task) = 0;
		/// <summary>
		/// Sets the time period of logging. Default = 0.1.
		/// </summary>
		/// <param name="logPeriod">Log period in simulation time units</param>
		virtual void SetLogPeriod(double logPeriod) = 0;
		/// <summary>
		/// Returns the time period of logging. Default = 0.1.
		/// </summary>
		/// <returns>Log period in simulation time units<returns>
		virtual double GetLogPeriod() const = 0;
		/// <summary>
		/// Sets the folder under which the results of the simulation should be saved. An additional sub-folder is created
		/// with the name indicating the current date and time to prevent overwriting old simulation results if IsUniqueSubfolder()==true.
		/// </summary>
		/// <param name="baseFolder">Base folder where simulation results are saved.</param>
		virtual void SetBaseFolder(std::string baseFolder) = 0;
		/// <summary>
		/// Returns the folder under which the results of the simulation should be saved. An additional sub-folder is created
		/// with the name indicating the current date and time to prevent overwriting old simulation results if IsUniqueSubfolder()==true.
		/// </summary>
		/// <returns>Base folder where simulation results are saved.</returns>
		virtual std::string GetBaseFolder() const = 0;
		/// <summary>
		/// Set to true to create an additional sub-folder under the base folder with the name indicating the current date and time to prevent overwriting old simulation results.
		/// </summary>
		/// <param name="uniqueSubFolder">True if sub-folder should be created, false if results should be saved directly in the base folder.</param>
		virtual void SetUniqueSubfolder(bool uniqueSubFolder) = 0;
		/// <summary>
		/// Returns true if an additional sub-folder under the base folder is created with the name indicating the current date and time to prevent overwriting old simulation results.
		/// </summary>
		/// <returns>True if sub-folder is created, false if results are saved directly in the base folder.</returns>
		virtual bool IsUniqueSubfolder() const = 0;
		/// <summary>
		/// Creates a logging task and adds it to this logger. Same as
		/// <code>
		/// Logger logger;
		/// //...
		/// shared_pointer&lt;TaskClass&gt; task = make_shared&lt;TaskClass&gt;(arguments...);
		/// logger.AddTask(task);
		/// </code>
		/// </summary>
		template<class TaskClass,
			class... ArgumentTypes> inline
			std::shared_ptr<TaskClass> CreateTask(ArgumentTypes&&... arguments)
		{
			std::shared_ptr<TaskClass> task = std::make_shared<TaskClass>(std::forward<ArgumentTypes>(arguments)...);
			AddTask(task);
			return task;
		}
	};
}