classdef stochDelayReaction < handle
    properties (SetAccess = private, GetAccess=private, Hidden = true)
        objectHandle; % Handle to the underlying C++ class instance
        simulationHandle; % Handle to simulation object to which this state belongs. Used for checking if the object handle is still valid.
        reactionHandle; % Handle to the state this object represents
    end
    properties (Constant,GetAccess=private, Hidden = true)
        Prefix = 'DelayReaction::';
    end
    methods (Static,Access = private, Hidden = true)
        function qualifiedName = prefix(functionName)
            qualifiedName = [stochDelayReaction.Prefix, functionName];
        end
    end
    methods
        %% Constructor - Create a new delay reaction
        % Should only be called from stochSimulation class
        function this = stochDelayReaction(objectHandle, simulationHandle, reactionHandle)
            this.objectHandle = objectHandle;
            this.simulationHandle = simulationHandle;
            this.reactionHandle = reactionHandle;
            assert(this.check(), 'Invalid object.');
        end
        
        %% Checks if the state points to a valid simulation object
        function valid = check(this)
            valid = isvalid(this) && isvalid(this.simulationHandle);
        end
        
        %% Returns name
        function name = name(this)
            assert(this.check(), 'Invalid object.');
            name = matstochsim(stochDelayReaction.prefix('Name'), this.objectHandle, this.reactionHandle);
        end
        
        function addProduct(this, state, varargin)
            assert(this.check(), 'Invalid object.');
            matstochsim(stochDelayReaction.prefix('AddProduct'), this.objectHandle, this.reactionHandle, state.getStateHandle(), varargin{:});
        end
    end
end