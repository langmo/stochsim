classdef stochState < handle
% A state of a stochsim, typically representing the molecular count of some
% species like proteins, mRNA, genes, inducers or similar.
% Can only be constructed via a stochSimulation object.
    
    properties (Access = private, Hidden = true)
        simulationHandle; % Handle to simulation object to which this state belongs.
        stateHandle; % Handle to the state this object represents
    end
    methods(Static, Hidden = true)
        function className = getClassName()
            className = 'State';
        end
    end
    methods (Access = protected, Hidden = true)
        function varargout = call(this, functionName, varargin)
            assert(this.check(), 'Invalid object.');
            [varargout{1:nargout}] = matstochsim([stochState.getClassName(), stochSimulation.getSeparator(), functionName], this.simulationHandle.objectHandle, this.stateHandle, varargin{:});
        end
    end
    methods (Access = {?stochSimulation, ?stochPropensityReaction, ?stochDelayReaction, ?stochTimerReaction}, Hidden = true)
        function stateHandle = getStateHandle(this)
            stateHandle = this.stateHandle;
        end
    end
    properties(Dependent)
        % Unique name of the state.
        name;
        % Initial condition of the state. Must be greater or equal to zero.
        initialCondition;
    end
    methods(Access = {?stochSimulation, ?stochComposedState})
        %% Constructor
        function this = stochState(simulationHandle, stateHandle)
            % Create a new stochsim state.
            % Should only be called from stochSimulation class.
            this.simulationHandle = simulationHandle;
            this.stateHandle = stateHandle;
            assert(this.check(), 'Invalid object.');
        end
    end
    methods  
        %% Validity
        function valid = check(this)
            % Checks if this state and the simulation are in a valid state.
            % Usage:
            %   valid = check(this)
            % Returns:
            %   valid - True if this state and the simulation are in a
            %           valid state and can be run/edited, false otherwise.
            valid = isvalid(this) && isvalid(this.simulationHandle);
        end
        %% Getters and setters for properties
        function ic = get.initialCondition(this)
            % Returns the initial condition (in molec) of this state.
            ic = this.call('GetInitialCondition');
        end
        
        function set.initialCondition(this, ic)
            % Sets the initial condition (in molec) of this state.
            this.call('SetInitialCondition', ic);
        end
        
        function name = get.name(this)
            % Returns the unique name of this state.
            name = this.call('GetName');
        end
    end
end