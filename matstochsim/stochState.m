classdef stochState < handle
    properties (SetAccess = private, GetAccess=private, Hidden = true)
        objectHandle; % Handle to the underlying C++ class instance
        simulationHandle; % Handle to simulation object to which this state belongs. Used for checking if the object handle is still valid.
        stateHandle; % Handle to the state this object represents
    end
    properties (Constant,GetAccess=private, Hidden = true)
        Prefix = 'State::';
    end
    methods (Static,Access = private)
        function qualifiedName = prefix(functionName)
            qualifiedName = [stochState.Prefix, functionName];
        end
    end
    methods (Access = {?stochReaction}, Hidden = true)
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
            assert(this.check(), 'Invalid object.');
            ic = matstochsim(stochState.prefix('InitialCondition'), this.objectHandle, this.stateHandle);
        end
        
        %% Returns name
        function name = name(this)
            assert(this.check(), 'Invalid object.');
            name = matstochsim(stochState.prefix('Name'), this.objectHandle, this.stateHandle);
        end
    end
end