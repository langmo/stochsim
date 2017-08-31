classdef stochState < handle
    properties (SetAccess = private, GetAccess=private, Hidden = true)
        objectHandle; % Handle to the underlying C++ class instance
        simulationHandle; % Handle to simulation object to which this state belongs. Used for checking if the object handle is still valid.
        stateHandle; % Handle to the state this object represents
    end
    methods (Access = protected, Hidden = true)
        function varargout = call(this, functionName, varargin)
            assert(this.check(), 'Invalid object.');
            [varargout{1:nargout}] = matstochsim(['State::', functionName], this.objectHandle, this.stateHandle, varargin{:});
        end
    end
    methods (Access = {?stochSimulation, ?stochPropensityReaction, ?stochDelayReaction}, Hidden = true)
        function stateHandle = getStateHandle(this)
            stateHandle = this.stateHandle;
        end
    end
    methods
        %% Constructor - Create a new stochsim State
        % Should only be called from stochSimulation class
        function this = stochState(objectHandle, simulationHandle, stateHandle)
            this.objectHandle = objectHandle;
            this.simulationHandle = simulationHandle;
            this.stateHandle = stateHandle;
            assert(this.check(), 'Invalid object.');
        end
        
        %% Checks if the state points to a valid simulation object
        function valid = check(this)
            valid = isvalid(this) && isvalid(this.simulationHandle);
        end
        %% Returns initial condition
        function ic = initialCondition(this)
            ic = this.call('InitialCondition');
        end
        
        %% Returns name
        function name = name(this)
            name = this.call('Name');
        end
    end
end