classdef stochSimulation < handle & matlab.mixin.CustomDisplay
    % Main object representing a simulation in stochsim. Every simulation
    % in stochsim is created by first creating a stochSimulation class, and
    % then configuring this class by adding states and reactions to the
    % simulation, and similar.
    properties (SetAccess = private, GetAccess=public, Hidden = true, Transient=true)
        objectHandle = []; % Handle to the underlying C++ class instance
    end
     methods(Static, Hidden = true)
        function className = getClassName()
            className = 'Simulation';
        end
     end
    methods(Static, Hidden=true, Access=?stochSimulationComponent)
        function separator = getSeparator()
            separator = '::';
        end
    end
    methods (Access = private, Hidden = true)
        function varargout = call(this, functionName, varargin)
            assert(this.check(), 'Invalid object.');
            [varargout{1:nargout}] = ...
                matstochsim([stochSimulation.getClassName(), stochSimulation.getSeparator(), functionName], ...
                this.objectHandle, varargin{:});
        end
    end
    methods (Access = private, Hidden = true)
        %% Helper functions to map C++ state and reaction reference strings to Matlab classes
        function state = toState(this, stateRef)
            idx = strfind(stateRef, stochSimulation.getSeparator());
            if length(idx) ~= 1
                error('stochsim:invalidStateRef', 'Reference to state "%s" has invalid format!', stateRef);
            end
            className = stateRef(1:idx-1);
            stateName = stateRef(idx+length(stochSimulation.getSeparator()):end);
            if strcmp(className, stochState.getClassName())
                state = stochState(this, stateName);
            elseif strcmp(className, stochComposedState.getClassName())
                state = stochComposedState(this, stateName);
            elseif strcmp(className, stochChoice.getClassName())
                state = stochChoice(this, stateName);
            else
                error('stochsim:classUnknown', 'The state class %s of the state %s is unknown!', className, stateName);
            end
        end
        
        function reaction = toReaction(this, reactionRef)
            idx = strfind(reactionRef, stochSimulation.getSeparator());
            if length(idx) ~= 1
                error('stochsim:invalidReactionRef', 'Reference to reaction "%s" has invalid format!', reactionRef);
            end
            className = reactionRef(1:idx-1);
            reactionName = reactionRef(idx+length(stochSimulation.getSeparator()):end);
            if strcmp(className, stochPropensityReaction.getClassName())
                reaction = stochPropensityReaction(this, reactionName);
            elseif strcmp(className, stochDelayReaction.getClassName())
                reaction = stochDelayReaction(this, reactionName);
            elseif strcmp(className, stochTimerReaction.getClassName())
                reaction = stochTimerReaction(this, reactionName);
            else
                error('stochsim:classUnknown', 'the state class %s of the state %s is unknown!', className, reactionName);
            end
        end
    end
    properties(Dependent)
        % Simulation time period defining in which intervals the current
        % molecular numbers of the states of this simulation get logged,
        % e.g. to the disk, or returned when calling run(...)
        logPeriod;
        
        % The base folder is the directory under which all simulation data
        % which gets written to the hard drive is saved. The base folder is
        % automatically created when running the simulation given that at
        % least one file is written to the disk.
        baseFolder;
        
        % Name of the file where the state trajectories are saved to the
        % hard drive. If set to an empty string, the state trajectories are
        % not saved to disk. Typically has the file ending CSV.
        resultFile;
        
        % If true, a unique sub-folder is created under the base folder for
        % every run of the simulation. This, thus, allows to run the
        % simulation several times without accidentially overwriting old
        % simulation results. If false, no such sub-folder is created. If
        % created, the name of the sub-folder encodes the date and time
        % when the simulation was started.
        uniqueSubfolder;
        
        % If true, the current progress of the simulation is written to the
        % Matlab console while the simulation is running. Specifically, the
        % percentage of the simulation already accomplished is displayed.
        logConsole;
    end
    properties(SetAccess = private, GetAccess=public,Dependent)
        % Cell array holding references to all states defined in the
        % simulation.
        states;
        % Cell array holding references to all reactions defined in the
        % simulation.
        reactions;
    end
    
    methods(Access = protected)
        %% Display methods
        function header = getHeader(this)
            if ~isscalar(this)
                header = getHeader@matlab.mixin.CustomDisplay(this);
            else
                className = matlab.mixin.CustomDisplay.getClassNameForHeader(this);
                nameLength = 15;
                
                formatState = ['\t%-', int2str(nameLength), 's %d\n'];
                formatChoice= ['\t%-', int2str(nameLength), 's %s\n'];
                states = this.states;
                stateStrings = cell(size(states));
                for i=1:numel(states)
                    if isa(states{i}, 'stochState') || isa(states{i}, 'stochComposedState')
                        stateStrings{i} = sprintf(formatState, states{i}.name, states{i}.initialCondition);
                    elseif isa(states{i}, 'stochChoice')
                        stateStrings{i} = sprintf(formatChoice, states{i}.name, states{i}.formatFormula('%2$s ? %3$s : %4$s'));
                    end
                end
                
                reactions = this.reactions;
                formatPropensity = ['\t%1$-', int2str(nameLength), 's %2$-10s -> %3$-10s %4$s\n'];
                formatDelay = ['\t%1$-', int2str(nameLength), 's %2$-10s -> %3$-10s delay: %4$s\n'];
                formatTimer = ['\t%1$-', int2str(nameLength), 's %2$-10s -> %3$-10s time: %4$s\n'];
                formatElse = ['\t%1$-', int2str(nameLength), 's %2$-10s -> %3$-10s unknown: %4$s\n'];
                reactionStrings = cell(size(reactions));
                for i=1:numel(reactions)
                    if isa(reactions{i}, 'stochPropensityReaction')
                        reactionStrings{i} = reactions{i}.formatFormula(formatPropensity);
                    elseif isa(reactions{i}, 'stochDelayReaction')
                        reactionStrings{i} = reactions{i}.formatFormula(formatDelay);
                    elseif isa(reactions{i}, 'stochTimerReaction')
                        reactionStrings{i} = reactions{i}.formatFormula(formatTimer);
                    else
                        reactionStrings{i} = reactions{i}.formatFormula(formatElse);
                    end
                end
                header = sprintf([...
                    'StochSim simulation (%s):\n',...
                    '\t%-', int2str(nameLength), 's %s\n'...
                    '\t',repmat('=', 1, nameLength+1+length('Initial Condition')),'\n',...
                    '%s\n',...
                    '\t%-', int2str(nameLength), 's %-24s %s\n'...
                    '\t',repmat('=', 1, nameLength+1+24+1+length('Rate Eq./Const., Time or Delay')),'\n',...
                    '%s\n',...
                    'Properties:'...
                    ], className, ...
                    'State', 'Initial Condition', [stateStrings{:}], ...
                    'Reaction', 'Formula', 'Rate Eq./Const., Time or Delay', [reactionStrings{:}]);
            end
        end
        
        function s = getFooter(this)
            s = matlab.mixin.CustomDisplay.getDetailedFooter(this);
        end
   end
    
    methods
        %% Constructor
        function this = stochSimulation(fileName, externalParameters)
            % Creates a new stochsim Simulation.
            % Usage:
            %   stochSimulation()
            %   stochSimulation(fileName)
            %   stochSimulation(fileName, externalParameters)
            % Parameters:
            %   fileName            Name of the file containing the model, e.g.
            %                       'model.cmdl'.
            %   externalParameters  Structure of additional parameters for the model.
            %                       If a parameter is defined both in the model and in
            %                       this structure, the definition in this structure
            %                       has precedence, allowing e.g. to easily scan
            %                       parameter regions. Parameters are defined as
            %                       externalParameters.myParameter = myValue; Can be
            %                       empty ("[]") to not predefine any parameters.
            %
            % If no file name for the model is provided, an empty model is
            % constructed.
            this.objectHandle = [];
            if nargin >= 1 && ~isempty(fileName)
                if nargin >= 2
					this.objectHandle = matstochsim('new', fileName, externalParameters);
                else
					this.objectHandle = matstochsim('new', fileName);
				end
			else
				this.objectHandle = matstochsim('new');
            end
        end
        
        %% Destructor
        function delete(this)
            % Destroy the stochsim simulation, and all of its staes,
            % reactions and similar.
            % Usage:
            %  delete(this)
            
			matstochsim('delete', this.objectHandle);
			this.objectHandle = [];
        end
        %% Validity
        function valid = check(this)
            % Checks if the simulation is in a valid state, i.e. can be run
            % or edited.
            % Usage:
            %   valid = check(this)
            % Returns:
            %   valid - True if the simulation is in a valid state and can
            %           be run/edited, false otherwise.
            valid = isvalid(this) && ~isempty(this.objectHandle);
        end
        %% States
        function state = createState(this, name, initialCondition)
            % Creates a state with the given name in the simulation. The
            % state can subsequently e.g. be added as a reactant or product
            % of a reaction.
            % Usage:
            %   state = createState(this, name)
            %   state = createState(this, name, initialCondition)
            % Parameters:
            %   name             - Name of the state, e.g. 'GFP', used to
            %                      uniquely identify the state. 
            %   initialCondition - Initial condition of the state.
            %                      Default=0.
            % Returns:
            %   state            - The newly created state.
            if nargin <3 || isempty(initialCondition)
                initialCondition = 0;
            end
			state = this.toState(this.call('CreateState', name, initialCondition));
        end
        
        function choice = createChoice(this, name, choiceEquation)
            % Creates a choice with the given name in the choice equation. The
            % choice can subsequently e.g. be added as a product
            % of a reaction. The choice should never be added as a
            % reactant.
            % A choice is a special kind of state in stochsim. Different to most or all other states, its concentration is always zero. The concentration of a choice can thus not be decreased, and an
            % error is thrown when the concentration of the choice is tried to be decreased, e.g. when the choice is used as a reactant in a reaction with non-zero propensity. 
            % In contrary, the choice can (and is intended to) be used as a product of a reaction. However, when in this case the reaction fires, the concentration of the choice is not increased. 
            % Instead, the boolean formula
            % associated with this choice is evaluated. Depending on the outcome of this evaluation, either the concentrations of one or the other set of product states is increased according to their
            % stochiometry.
            % The typical use case is to use a Choice to implement conditionals for reactions. For example, a reaction might result in a product B with a certain probability, and in a product C with another, i.e.
            %      { B     if rand()>0.2
            % A -> {
            %      { C     otherwise.
            % This would then be implemented as a propensity reaction with A as the reactant, and a choice as the product. The boolean formula associated to the choice would be "rand()>0.2", the first set of
            % products of the choice would contain only B with stochiometry 1, and the second set only C with stochiometry 1.
            % The usage of a choice becomes specifically interesting when combined with reactions providing additional information (variables) which can be used in the boolean expression. For example, a delay
            % reaction passes the number of how often its (sole) reactant was transformed.
            % Usage:
            %   choice = createChoice(this, name)
            %   choice = createChoice(this, name, choiceEquation)
            % Parameters:
            %   name             - Name of the choice, e.g. 'doEitherThisOrThat', used to
            %                      uniquely identify the choice. Note that
            %                      due to implementation details, a choice
            %                      must also not have the same name as a
            %                      state.
            %   choiceEquation   - A boolean equation which gets evaluated
            %                      every time the choice is invoked.
            %                      Depending on the outcome of the
            %                      evaluation, the concentration of one of
            %                      the product sets of the choice is
            %                      increased.
            %                      Default='1'.
            % Returns:
            %   choice           - The newly created Choice.
            if nargin <3 || isempty(choiceEquation)
                choiceEquation = '1';
            end
			choice = this.toState(this.call('CreateChoice', name, choiceEquation));
        end
        
        function state = createComposedState(this, name, initialCondition, capacity)
            % Creates a composed state with the given name in the
            % simulation. Similar to a "normal" state, the state can
            % subsequently e.g. be added as a reactant or product of a
            % propensity reaction. It can additionally be used in delay
            % reactions, that is, reactions which fire after a given time a
            % molecular of the state was produced. Furthermore, a composed
            % state takes track how often it was transformed by a reaction,
            % that is, one can determine how often a molecule of the state
            % took part in a reaction which did not produce or consume it.
            % Usage:
            %   state = createComposedState(this, name)
            %   state = createComposedState(this, name, initialCondition)
            %   state = createComposedState(this, name, initialCondition, capacity)
            % Parameters:
            %   name             - Name of the state, e.g. 'GFP', used to
            %                      uniquely identify the state. 
            %   initialCondition - Initial condition of the state.
            %                      Default=0.
            %   capacity         - Estimate of an upper bound on how many
            %                      molecules the state can maximally
            %                      represent. If the number of molecules
            %                      increases over the capacity, the
            %                      capacity is automatically increased,
            %                      which however slows down the simulation.
            %                      Default=1000;
            % Returns:
            %   state            - The newly created composed state.
            if nargin < 3 || isempty(initialCondition)
                initialCondition = 0;
            end
            if nargin < 4 || isempty(capacity)
                capacity = 1000;
            end
            state = this.toState(this.call('CreateComposedState', name, initialCondition, capacity));
        end
        
        function state = getState(this, name)
            % Returns the state with the given name. Throws an exception if
            % the state does not exist.
            % Usage:
            %   state = getState(this, name)
            % Parameters:
            %   name - Name of the state.
            % Returns:
            %   state - object representing the state.
            if isempty(strfind(name, stochSimulation.getSeparator()))
                stateRef = this.call('GetState', name);
            else
                stateRef = name;
            end
            state = this.toState(stateRef);
        end
        %% Create Reactions
        function reaction = createPropensityReaction(this, name, rateConstant)
            % Creates a propensity based ("normal") reaction. The
            % propensity ("rate") of the reaction is calculated using mass
            % action kinetics and can be considered to represent the
            % average number of times the reaction fires during a certain
            % time period.
            % Usage:
            %   reaction = createPropensityReaction(this, name, rateConstant)
            % Parameters:
            %   name         - Name of the reaction, e.g. 'A->B' used to
            %                  uniquely identify the reaction.
            %   rateConstant - The rate constant of the reaction used to
            %                  calculate the propensity ("rate") of the
            %                  reaction using mass action kinetics.
			reaction = this.toReaction(this.call('CreatePropensityReaction', name, rateConstant));
        end
        function reaction = createDelayReaction(this, name, composedState, delay)
            % Creates a reaction which fires exactly after a given time a
            % new molecule of the composed state is produced. Note that
            % such delay reactions require the creation time of every
            % molecule represented by this state to be saved separately,
            % and can thus only accept composed states as their sole
            % reactant. When the reaction fires, the respective molecule of
            % the composed state is consumed.
            % Usage:
            %   reaction = createDelayReaction(this, name, composedState, delay)
            % Parameters:
            %   name         - Name of the reaction, e.g. 'A->B' used to
            %                  uniquely identify the reaction.
            %   composedState- The composed state whose creation time of
            %                  molecules determine when this reaction
            %                  fires, and whose molecule count is
            %                  correspondingly decreased when the reaction
            %                  fires.
            %   delay        - The delay after the creation of a molecule
            %                  of the composed state when this reaction
            %                  fires.
            if ~ischar(composedState)
                composedState = composedState.componentHandle;
            end
			reaction = this.toReaction(this.call('CreateDelayReaction', name, composedState, delay));
        end
        function reaction = createTimerReaction(this, name, fireTime)
            % A reaction or event which fires a single time at exactly the
            % simulation time specified. The reaction can only contain
            % products but no reactans. Useful to e.g. simulate the
            % addition of some ligands at a specific time.
            % Usage:
            %   reaction = createTimerReaction(this, name, fireTime)
            % Parameters:
            %   name         - Name of the reaction, e.g. 'A->B' used to
            %                  uniquely identify the reaction.
            %   fireTime     - Time, in sumulation time units, when the
            %                  timer reaction should fire.
			reaction = this.toReaction(this.call('CreateTimerReaction', name, fireTime));
        end
        function reaction = getReaction(this, name)
            % Returns the reaction with the given name. Throws an exception if
            % the reaction does not exist.
            % Usage:
            %   reaction = getReaction(this, name)
            % Parameters:
            %   name - Name of the reaction.
            % Returns:
            %   reaction - object representing the reaction.
            if isempty(strfind(name, stochSimulation.getSeparator()))
                reactionRef = this.call('GetReaction', name);
            else
                reactionRef = name;
            end
            reaction = this.toReaction(reactionRef);
        end
        %% Executes simulation
        function varargout = run(this, runtime)
            % Executes the simulation, as it is currently configured. The
            % simulation results are saved to disk if configured
            % accordingly, and/or returned to the caller.
            % Usage:
            %   run(this, runtime)
            %   data = run(this, runtime)
            %   [headers,data] = run(this, runtime)
            % Parameters:
            %   runtime     - Time duration, in simulation time units,
            %                 specifying how long the simulation should
            %                 run.
            % Returns:
            %   data        - An array containing the simulation results.
            %                 The first column represents the time, and
            %                 each subsequent column the molecular number
            %                 of a different species. Each row represents a
            %                 the state of the simulation at a specific
            %                 time when it was logged.
            %   headers     - Cell array containing strings with the names
            %                 of the columns in data.
			[varargout{1:nargout}] = this.call('Run', runtime);
        end
        
        %% Getters and setters for properties
        function states = get.states(this)
            stateRefs = this.call('GetStates');
            states = cell(size(stateRefs));
            for i=1:numel(stateRefs)
                states{i} = this.toState(stateRefs{i});
            end
        end     
        function reactions = get.reactions(this)
            reactionRefs = this.call('GetReactions');
            reactions = cell(size(reactionRefs));
            for i=1:numel(reactionRefs)
                reactions{i} = this.toReaction(reactionRefs{i});
            end
        end     
        function set.logPeriod(this, logPeriod)
            this.call('SetLogPeriod', logPeriod);
        end
        
        function logPeriod = get.logPeriod(this)
            logPeriod = this.call('GetLogPeriod');
        end
        
        function set.baseFolder(this, baseFolder)
            this.call('SetBaseFolder', baseFolder);
        end
        
        function baseFolder = get.baseFolder(this)
            baseFolder = this.call('GetBaseFolder');
        end
        function set.resultFile(this, resultFile)
            if isempty(resultFile)
                resultFile = [];
            end
            this.call('SetResultFile', resultFile);
        end
        function resultFile = get.resultFile(this)
            resultFile = this.call('GetResultFile');
            if isempty(resultFile)
                resultFile = [];
            end
        end
        function set.uniqueSubfolder(this, uniqueSubfolder)
            this.call('SetUniqueSubfolder', uniqueSubfolder);
        end
        
        function uniqueSubfolder = get.uniqueSubfolder(this)
            uniqueSubfolder = this.call('IsUniqueSubfolder');
        end
        
        function set.logConsole(this, logConsole)
            this.call('SetLogConsole', logConsole);
        end
        
        function logConsole = get.logConsole(this)
            logConsole = this.call('IsLogConsole');
        end
        
    end
end