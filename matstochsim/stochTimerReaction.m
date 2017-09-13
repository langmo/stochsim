classdef stochTimerReaction < handle
% A reaction or event which fires a single time at exactly the
% simulation time specified. The reaction can only contain
% products but no reactans. Useful to e.g. simulate the
% addition of some ligands at a specific time.
% Can only be constructed via a stochSimulation object.
    properties (SetAccess = private, GetAccess=private, Hidden = true)
        simulationHandle; % Handle to simulation object to which this state belongs. Used for checking if the object handle is still valid.
        reactionHandle; % Handle to the state this object represents
    end
    methods (Access = private, Hidden = true)
        function varargout = call(this, functionName, varargin)
            assert(this.check(), 'Invalid object.');
            [varargout{1:nargout}] = matstochsim(['TimerReaction::', functionName], this.simulationHandle.objectHandle, this.reactionHandle, varargin{:});
        end
    end
    methods(Access = ?stochSimulation)
        %% Constructor - Create a new timer reaction
        % Should only be called from stochSimulation class
        function this = stochTimerReaction(simulationHandle, reactionHandle)
            this.simulationHandle = simulationHandle;
            this.reactionHandle = reactionHandle;
            assert(this.check(), 'Invalid object.');
        end
    end
    properties(Dependent)
        % Unique name of the reaction.
        name;
        % Time, in sumulation time units, when the timer reaction should
        % fire.
        fireTime;
    end
    methods
        %% Validity
        function valid = check(this)
            % Checks if this reaction and the simulation are in a valid state.
            % Usage:
            %   valid = check(this)
            % Returns:
            %   valid - True if this reaction and the simulation are in a
            %           valid state and can be run/edited, false otherwise.
            valid = isvalid(this) && isvalid(this.simulationHandle);
        end
        
        %% Getters and setters for properties
        function name = get.name(this)
            % Returns the unique name of this reaction.
            name = this.call('Name');
        end
        function fireTime = get.fireTime(this)
            % Returns the time, in simulation time units, when the reaction
            % fires. The reaction fires only exactly once at the given
            % time.
            fireTime = this.call('GetFireTime');
        end
        function set.fireTime(this, fireTime)
            % Sets the time, in simulation time units, when the reaction
            % fires. The reaction fires only exactly once at the given
            % time.
            this.call('SetFireTime', fireTime);
        end
        
        %% Products, reactants and similar.
        function addProduct(this, state, stochiometry)
            % Adds a species as a reactant of the reaction. When the
            % reaction fires, its concentration is decreased according to
            % its stochiometry. Also, it's concentration modifies the
            % reaction rate according to its stochiometry following mass
            % action kinetics.
            % Usage:
            %   addProduct(this, state)
            %   addProduct(this, state, stochiometry)
            % Parameters:
            %   state           ...name or reference to state to add.
            %   stochiometry    ...number of molecules assumed to be
            %                      consumed by the reaction. Default = 1.
            if ~ischar(state)
                state = state.getStateHandle();
            end
            if ~exist('stochiometry', 'var') || isempty(stochiometry)
                stochiometry = 1;
            end
            this.call('AddProduct', state, stochiometry);
        end        
    end
end