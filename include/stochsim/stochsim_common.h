#pragma once
#include <string>
#include <limits>
#include <vector>
#include <memory>
#include <utility>
#include <array>
#include <initializer_list>
#include <tuple>			
#include <functional>
#include "expression_common.h"
namespace stochsim
{
	/// <summary>
	///  Value used to indicate that delayed reaction should not fire at all (=fire at time infinity)
	/// </summary>
	constexpr double inf(std::numeric_limits<double>::max());
	/// <summary>
	/// Stochiometry of a reaction component. Must be a positive number.
	/// </summary>
	typedef unsigned int Stochiometry;
	/// <summary>
	/// Collection used to represent sets of states, reactions and similar in public interfaces.
	/// Note that the way these sets are saved internally can deviate from this definition.
	/// </summary>
	template <class T> using Collection = std::vector<T>;
	/// <summary>
	/// Name value pair, used to pass named variables having double values around.
	/// </summary>
	typedef std::pair<std::string, double> Variable;
	/// <summary>
	/// Array of name value pairs representing defined variables and their values.
	/// </summary>
	typedef std::vector<Variable> Variables;
	/// <summary>
	/// Creates a variable with the given name.
	/// </summary>
	constexpr auto make_variable = std::make_pair<std::string, double>;
	
	/// <summary>
	/// A molecule is one element of a state. In stochsim, each molecule itself can have an individuality, represented by a certain set of properties (double values).
	/// Note that not all states allow the properties of their molecules to be varied.
	/// </summary>
	struct Molecule
	{
	public:
		/// <summary>
		/// Number of different properties a molecule can have.
		/// </summary>
		static constexpr size_t size_ = 2;
		/// <summary>
		/// Returns number of different properties a molecule can have. Always same as variable size_.
		/// </summary>
		/// <returns>Number of different properties a molecule can have.</returns>
		inline size_t Size() const
		{
			return size_;
		}
		typedef std::array<double, size_> PropertyValues;
		typedef std::array<std::string, size_> PropertyNames;
		typedef std::array<std::unique_ptr<expression::IExpression>, size_> PropertyExpressions;
	private:
		std::array<double, size_> properties_;
	public: 
		Molecule() : properties_({ 0.,0.})
		{
		};
		Molecule(PropertyValues&& properties) : properties_(std::move(properties))
		{
		};
		Molecule(const PropertyValues& properties) : properties_(properties)
		{
		};
		/// <summary>
		/// Sets all properties of the molecule to zero.
		/// </summary>
		inline void Reset() noexcept
		{
			for (auto& val : properties_)
			{
				val = 0.0;
			}
		}

		inline double& operator[] (size_t i)
		{
			return properties_[i];
		}
		inline const double& operator[] (size_t i) const
		{
			return properties_[i];
		}
	};
	/// <summary>
	/// A default molecule with all properties set to zero.
	/// </summary>
	const Molecule defaultMolecule;

	// Forward declaration.
	class IState;

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
		virtual double GetSimTime() const = 0;
		/// <summary>
		/// Returns the runtime of the simulation, i.e. when the simulation stops.
		/// Should only be called while a simulation is running.
		/// </summary>
		/// <returns>Total runtime of simulation, in simulation time units.</returns>
		virtual double GetRunTime() const = 0;
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
		/// <summary>
		/// Returns the folder under which the results of the simulation should be saved.
		/// </summary>
		/// <returns>Folder where simulation results are saved.</returns>
		virtual std::string GetSaveFolder() const = 0;
		/// <summary>
		/// Returns the time period of logging. Default = 0.1.
		/// </summary>
		/// <returns>Log period in simulation time units</returns>
		virtual double GetLogPeriod() const = 0;
		/// <summary>
		/// Returns a collection of all states defined in the simulation.
		/// </summary>
		/// <returns>States defined in the simulation.</returns>
		virtual const Collection<std::shared_ptr<IState>> GetStates() const = 0;
	};

	/// <summary>
	/// Base class of all states (species) of a simulation. Every state has a name and an initial condition. How the value of the state is represented during runtime, however, is implementation dependent.
	/// </summary>
	class IState
	{
	public:
		/// <summary>
		/// Typedef for listener with which modifications of the state can be detected..
		/// </summary>
		typedef std::function<void(const Molecule& molecule, double time)> StateListener;
		/// <summary>
		/// Virtual destructor.
		/// </summary>
		virtual ~IState() {};
		/// <summary>
		/// Returns the current value of the state, i.e. the concentration times the volume.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		/// <returns>Number of elements the state represents/value of the state. At t=0, must be equal to the initial condition.</returns>
		virtual size_t Num(ISimInfo& simInfo) const = 0;
		/// <summary>
		/// Returns an arbitrary molecule (element) of this state.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		/// <returns>Arbitrary molecule.</returns>
		virtual const Molecule& Peak(ISimInfo& simInfo) const = 0;
		/// <summary>
		/// Increases the value of the state by the given number. Typically called by the simulation as a result of a reaction firing, with this state being a product of the reaction.
		/// </summary>
		/// <param name="simInfo">Object providing context under which situation (e.g. when) the value of the state is increased.</param>
		/// <param name="molecule">The molecule which should be added. Set to NULL to add a default molecule. </param>
		/// <param name="variables">Variables which are currently defined. Default is empty vector. </param>
		virtual void Add(ISimInfo& simInfo, const Molecule& molecule= defaultMolecule, const Variables& variables = {}) = 0;
		/// <summary>
		/// Decreases the value of the state by the given number. Typically called by the simulation as a result of a reaction firing, with this state being a reactant of the reaction.
		/// </summary>
		/// <param name="simInfo">Object providing context under which situation (e.g. when) the value of the state is decreased.</param>
		/// <param name="num">Number by which the value of the state is decreased. Must be &gt;0. </param>
		/// <param name="variables">Variables which are currently defined. Default is empty vector. </param>
		/// <returns>The molecule which was removed.</returns>
		virtual Molecule Remove(ISimInfo& simInfo, const Variables& variables = {}) = 0;
		/// <summary>
		/// Called when, as the result of a reaction, the number of molecules represented by this state should not increase or decrease, but instead some molecule should be transformed in some way.
		/// It is assumed that each molecule represented by the state is equally likely to get transformed, i.e. the molecule which is actually transformed should be chosen randomly.
		/// What exactly a transformation could be is implementation dependent. For states which cannot be transformed, this function can simply return a static reference.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		/// <returns>Molecule which can be transformed.</returns>
		virtual Molecule& Transform(ISimInfo& simInfo, const Variables& variables = {}) = 0;
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
		virtual std::string GetName() const noexcept= 0;
		/// <summary>
		/// Adds a listener which gets invoked every time the concentration of this state decreases.
		/// </summary>
		/// <param name="fireListener">Listener to add.</param>
		virtual void AddDecreaseListener(StateListener stateListener) = 0;
		/// <summary>
		/// Adds a listener which gets invoked every time the concentration of this state increases.
		/// </summary>
		/// <param name="fireListener">Listener to add.</param>
		virtual void AddIncreaseListener(StateListener stateListener) = 0;
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
		/// Called by the simulation before the simulation starts. Should ensure that the reaction is at a consistent state.
		/// </summary>
		/// <param name="simInfo">Simulation context</param>
		virtual void Initialize(ISimInfo& simInfo) = 0;
		/// <summary>
		/// Called by the simulation after the simulation finished. Can be used for cleanup.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		virtual void Uninitialize(ISimInfo& simInfo) = 0;
		/// <summary>
		/// Returns the current propensity/rate with which the reaction fires.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		/// <returns>Current rate/propensity.</returns>
		virtual double ComputeRate(ISimInfo& simInfo) const = 0;
		/// <summary>
		/// Called by the simulation when this reaction is chosen to fire next. Should modify the concentrations of all involved species according to their stochiometries.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		virtual void Fire(ISimInfo& simInfo) = 0;

		/// <summary>
		/// Returns the name of the reaction.
		/// </summary>
		/// <returns>Name of the reaction.</returns>
		virtual std::string GetName() const = 0;
	};

	/// <summary>
	/// Base class of all reactions which fire at a given time. Thus, the time when the reaction fires is not distributed according to a propensity, but the reaction is instead guaranteed to
	/// fire at a given time. This can be used to realize timers, delays or other kinds of events.
	/// </summary>
	class IEventReaction
	{
	public:
		virtual ~IEventReaction() {}
		/// <summary>
		/// Called by the simulation before the simulation starts. Should ensure that the reaction is at a consistent state.
		/// </summary>
		/// <param name="simInfo">Simulation context</param>
		virtual void Initialize(ISimInfo& simInfo) = 0;
		/// <summary>
		/// Called by the simulation after the simulation finished. Can be used for cleanup.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		virtual void Uninitialize(ISimInfo& simInfo) = 0;
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
		virtual std::string GetName() const = 0;
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
		/// <param name="simInfo">Simulation context.</param>
		/// <param name="time">Time of logging. Note that this can be different to the current simulation time.</param>
		virtual void WriteLog(ISimInfo& simInfo, double time) = 0;
		/// <summary>
		/// Called before a new simulation is started.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		virtual void Initialize(ISimInfo& simInfo) = 0;
		/// <summary>
		/// Called after the simulation has finished. Perfect place to release e.g. file handles.
		/// </summary>
		/// <param name="simInfo">Simulation context.</param>
		virtual void Uninitialize(ISimInfo& simInfo) = 0;
		/// <summary>
		/// Should return true if the logger, in its current configuration, will write anything to the disk/file system, and false otherwise.
		/// </summary>
		/// <returns>True if anything is written to the disk.</returns>
		virtual bool WritesToDisk() const  = 0;
	};

	/// <summary>
	/// A reaction left element is either a reactant or a modifier of a reaction, i.e. an element standing on the left of the reaction arrow.
	/// </summary>
	struct ReactionLeftElement
	{
	public:
		/// <summary>
		/// State representing the reactant or modifier.
		/// </summary>
		const std::shared_ptr<IState> state_;
		/// <summary>
		/// Stochiometry of the reaction element.
		/// </summary>
		const Stochiometry stochiometry_;
		/// <summary>
		/// Names of the reactant/modifier molecule properties.
		/// </summary>
		const Molecule::PropertyNames propertyNames_;
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="state">State representing the reactant or modifier.</param>
		/// <param name="stochiometry">Stochiometry of the reaction element.</param>
		ReactionLeftElement(std::shared_ptr<IState> state, Stochiometry stochiometry, Molecule::PropertyNames propertyNames) : state_(std::move(state)), stochiometry_(stochiometry), propertyNames_(std::move(propertyNames))
		{
		}
	};

	/// <summary>
	/// A reaction right element is a product of a reaction, i.e. an element standing on the right of the reaction arrow.
	/// </summary>
	struct ReactionRightElement
	{
	public:
		/// <summary>
		/// State representing the product.
		/// </summary>
		const std::shared_ptr<IState> state_;
		/// <summary>
		/// Stochiometry of the reaction element.
		/// </summary>
		const Stochiometry stochiometry_;
		/// <summary>
		/// Expressions of the product molecule properties.
		/// </summary>
		const Molecule::PropertyExpressions propertyExpressions_;
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="state">State representing the product.</param>
		/// <param name="stochiometry">Stochiometry of the reaction element.</param>
		ReactionRightElement(std::shared_ptr<IState> state, Stochiometry stochiometry, Molecule::PropertyExpressions propertyExpressions) : state_(std::move(state)), stochiometry_(stochiometry), propertyExpressions_(std::move(propertyExpressions))
		{
		}

		/// <summary>
		/// Copy constructor.
		/// </summary>
		/// <param name="other">Object to copy.</param>
		ReactionRightElement(const ReactionRightElement& other) : state_(other.state_), stochiometry_(other.stochiometry_), propertyExpressions_(cloneArray(other.propertyExpressions_))
		{
		}
	private:
		/// <summary>
		/// Helper function to deep-copy property expressions.
		/// Since we want to initialize in the copy constructor a constant array, we have to do that in the initialization list. This helper function is a workaround since the initialization list
		/// does not support complicated operations.
		/// </summary>
		/// <param name="other">property expressions to deep-copy.</param>
		/// <returns>A deep copy.</returns>
		static inline Molecule::PropertyExpressions cloneArray(const Molecule::PropertyExpressions& other)
		{
			Molecule::PropertyExpressions clone;
			for (size_t i = 0; i < other.size(); i++)
			{
				clone[i] = other[i] ? other[i]->Clone() : nullptr;
			}
			return std::move(clone);
		}
	};

	/// <summary>
	/// A reaction left/right element is a transformee of a reaction, i.e. an element standing on the left and right of the reaction arrow.
	/// </summary>
	struct ReactionLeftRightElement
	{
	public:
		/// <summary>
		/// State representing the transformee.
		/// </summary>
		const std::shared_ptr<IState> state_;
		/// <summary>
		/// Stochiometry of the reaction element.
		/// </summary>
		const Stochiometry stochiometry_;
		/// <summary>
		/// Names of the transformee molecule properties.
		/// </summary>
		const Molecule::PropertyNames propertyNames_;
		/// <summary>
		/// Expressions of the transformee molecule properties.
		/// </summary>
		const Molecule::PropertyExpressions propertyExpressions_;
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="state">State representing the transformee.</param>
		/// <param name="stochiometry">Stochiometry of the reaction element.</param>
		ReactionLeftRightElement(std::shared_ptr<IState> state, Stochiometry stochiometry, Molecule::PropertyExpressions propertyExpressions, Molecule::PropertyNames propertyNames) : state_(std::move(state)), stochiometry_(stochiometry), propertyExpressions_(std::move(propertyExpressions)), propertyNames_(std::move(propertyNames))
		{
		}
		/// <summary>
		/// Copy constructor.
		/// </summary>
		/// <param name="other">Object to copy.</param>
		ReactionLeftRightElement(const ReactionLeftRightElement& other) : state_(other.state_), stochiometry_(other.stochiometry_), propertyNames_(other.propertyNames_), propertyExpressions_(cloneArray(other.propertyExpressions_))
		{
		}
	private:
		/// <summary>
		/// Helper function to deep-copy property expressions.
		/// Since we want to initialize in the copy constructor a constant array, we have to do that in the initialization list. This helper function is a workaround since the initialization list
		/// does not support complicated operations.
		/// </summary>
		/// <param name="other">property expressions to deep-copy.</param>
		/// <returns>A deep copy.</returns>
		static inline Molecule::PropertyExpressions cloneArray(const Molecule::PropertyExpressions& other)
		{
			Molecule::PropertyExpressions clone;
			for (size_t i = 0; i < other.size(); i++)
			{
				clone[i] = other[i] ? other[i]->Clone() : nullptr;
			}
			return std::move(clone);
		}
	};
}