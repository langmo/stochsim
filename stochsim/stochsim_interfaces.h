#pragma once
#include <string>
#include <limits>
namespace stochsim
{
	/// <summary>
	///  Value used to indicate that delayed reaction should not fire at all (=fire at time infinity)
	/// </summary>
	constexpr double inf(std::numeric_limits<double>::max());
	typedef unsigned int Stochiometry;

	/// <summary>
	/// Provides information about the current global state of the simulation, e.g. the current simulation time.
	/// Also provides some helper functions to e.g. calculate random numbers. Random numbers should only be calculated given these numbers,
	/// which guarantees that a simulation will always run exactly the same way when the same seed is used (typically, a different seed is used for every simulation, but for debugging
	/// purposes it is often useful to be able to exactly reproduce a given situation).
	/// </summary>
	class ISimInfo
	{
	public:
		virtual ~ISimInfo() {};
		/// <summary>
		/// Returns the current simulation time.
		/// Should only be called while a simulation is running.
		/// </summary>
		/// <returns>Simulation time, starting at zero.</returns>
		virtual double SimTime() const = 0;
		/// <summary>
		/// Returns the runtime of the simulation, i.e. when the simulation stops.
		/// Should only be called while a simulation is running.
		/// </summary>
		/// <returns>Total runtime of simulation, in simulation time units.</returns>
		virtual double RunTime() const = 0;
		/// <summary>
		/// Returns the period for logging, e.g. "1.0" if every 1 units of simulation the current state is logged.
		/// </summary>
		/// <returns>Period of logging in simulation time units</returns>
		virtual double LogPeriod() const = 0;
		/// <summary>
		/// Generates a uniformly distributed random integer number between the lower and the upper bound. Both bounds are included.
		/// </summary>
		/// <param name="lower">lower value which can be generated (included).</param>
		/// <param name="upper">upper value which can be generated (included).</param>
		/// <returns></returns>
		virtual size_t Rand(size_t lower, size_t upper) = 0;
		/// <summary>
		/// Generates a uniformly distributed random double number between zero and one.
		/// </summary>
		/// <returns></returns>
		virtual double Rand() = 0;
	};

	/// <summary>
	/// Base class of all states (species) of a simulation. Every state has a name and an initial condition. How the value of the state is represented during runtime, however, is implementation dependent.
	/// </summary>
	class IState
	{
	public:
		virtual ~IState() {};
		/// <summary>
		/// Returns the current value of the state, i.e. the concentration times the volume.
		/// </summary>
		/// <returns>Number of elements the state represents/value of the state. At t=0, must be equal to the initial condition.</returns>
		virtual size_t Num() const = 0;
		/// <summary>
		/// Increases the value of the state by the given number. Typically called by the simulation as a result of a reaction firing, with this state being a product of the reaction.
		/// </summary>
		/// <param name="simInfo">Object providing context under which situation (e.g. when) the value of the state is increased.</param>
		/// <param name="num">Number by which the value of the state is increased. Must be &gt;0. </param>
		virtual void Add(ISimInfo& simInfo, size_t num = 1) = 0;
		/// <summary>
		/// Decreases the value of the state by the given number. Typically called by the simulation as a result of a reaction firing, with this state being a reactant of the reaction.
		/// </summary>
		/// <param name="simInfo">Object providing context under which situation (e.g. when) the value of the state is decreased.</param>
		/// <param name="num">Number by which the value of the state is decreased. Must be &gt;0. </param>
		virtual void Remove(ISimInfo& simInfo, size_t num = 1) = 0;
		/// <summary>
		/// Called when, as the result of a reaction, the number of molecules represented by this state should not increase or decrease, but instead some molecule should be modified.
		/// It is assumed that each molecule represented by the state is equally likely to get modified, i.e. the molecule which is actually modified should be chosen randomly.
		/// What exactly a modification could be is implementation dependent. For states which cannot be modified, this function can simply do nothing.
		/// </summary>
		/// <param name="simInfo">Object providing context under which situation (e.g. when) the modification takes place.</param>
		virtual void Modify(ISimInfo& simInfo) = 0;
		/// <summary>
		/// Called by the simulation before the simulation starts. Should ensure that e.g. the current value of the state equals the initial condition.
		/// </summary>
		/// <param name="simInfo">Simulation context</param>
		virtual void Initialize(ISimInfo& simInfo) = 0;
		/// <summary>
		/// Called by the simulation after the simulation finished. Can be used for cleanup.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		virtual void Uninitialize(ISimInfo& simInfo) = 0;
		/// <summary>
		/// Returns the name of the state/species.
		/// </summary>
		/// <returns>Name of the species/state.</returns>
		virtual std::string Name() const = 0;
		/// <summary>
		///  Returns the initial condition of the state. It must be guaranteed that at t=0, Num()==InitialCondition().
		/// </summary>
		/// <returns>Initial condition of the state.</returns>
		virtual size_t InitialCondition() const = 0;
	};

	/// <summary>
	/// Base class of all reactions which are propensity based, i.e. the probability that a given reaction fires in a given infinitisemal time period is proportional to the length of the time unit,
	/// and only depends on the current state. That is, the next firing time is exponentially distributed.
	/// </summary>
	class IPropensityReaction
	{
	public:
		virtual ~IPropensityReaction() {};
		/// <summary>
		/// Returns the current propensity/rate with which the reaction fires.
		/// </summary>
		/// <returns>Current rate/propensity.</returns>
		virtual double ComputeRate() const = 0;
		/// <summary>
		/// Called by the simulation when this reaction is chosen to fire next. Should modify the concentrations of all involved species according to their stochiometries.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		virtual void Fire(ISimInfo& simInfo) = 0;

		/// <summary>
		/// Returns the name of the reaction.
		/// </summary>
		/// <returns>Name of the reaction.</returns>
		virtual std::string Name() const = 0;
	};

	/// <summary>
	/// Base class of all reactions which fire at a given time. Thus, the duration when the reaction fires is not distributed according to a propensity, but the reaction is instead guaranteed to
	/// fire at a given time.
	/// </summary>
	class IDelayedReaction
	{
	public:
		virtual ~IDelayedReaction() {}
		/// <summary>
		/// Returns the simulation time when the reaction will fire next, or DelayedReaction::inf_ if the delayed reaction won't fire anymore, given the current simulation state.
		/// </summary>
		/// <param name="simInfo">Context of the simulation, e.g. the current simulation time.</param>
		/// <returns>Time when the reaction will fire next.</returns>
		virtual double NextReactionTime(ISimInfo& simInfo) const = 0;
		/// <summary>
		/// Called by the simulation when this reaction is due to fire.
		/// </summary>
		/// <param name="simInfo">Context of the simulation, e.g. the current simulation time.</param>
		virtual void Fire(ISimInfo& simInfo) = 0;
		/// <summary>
		/// Returns the name of the reaction.
		/// </summary>
		/// <returns>Name of the reaction.</returns>
		virtual std::string Name() const = 0;
	};

	/// <summary>
	/// Base class of all task which are executed when the state of the simulation should be logged, e.g. written to the disk or printed on the screen.
	/// </summary>
	class ILogger
	{
	public:
		virtual ~ILogger() {}
		/// <summary>
		/// Called when the task should write whatever information it is responsible to write to the disk/display on the screen.
		/// </summary>
		/// <param name="time">The current simulation time.</param>
		virtual void WriteLog(double time) = 0;
		/// <summary>
		/// Called before a new simulation is started.
		/// </summary>
		/// <param name="baseFolder">The folder where the results of the simulation should be stored on the disk, if this task actually stores anything on the disk.</param>
		/// <param name="simInfo">Simulation context.</param>
		virtual void Initialize(std::string baseFolder, ISimInfo& simInfo) = 0;
		/// <summary>
		/// Called after the simulation has finished. Perfect place to release e.g. file handles.
		/// </summary>
		virtual void Uninitialize() = 0;
		/// <summary>
		/// Should return true if the logger, in its current configuration, will write anything to the disk/file system, and false otherwise.
		/// </summary>
		/// <returns>True if anything is written to the disk.</returns>
		virtual bool WritesToDisk() const  = 0;
	};
}