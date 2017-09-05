classdef stochPropensityReaction < handle
    properties (SetAccess = private, GetAccess=private, Hidden = true)
        objectHandle; % Handle to the underlying C++ class instance
        simulationHandle; % Handle to simulation object to which this state belongs. Used for checking if the object handle is still valid.
        reactionHandle; % Handle to the state this object represents
    end
    methods (Access = private, Hidden = true)
        function varargout = call(this, functionName, varargin)
            assert(this.check(), 'Invalid object.');
            [varargout{1:nargout}] = matstochsim(['PropensityReaction::', functionName], this.objectHandle, this.reactionHandle, varargin{:});
        end
    end
    methods
        %% Constructor - Create a new stochsim State
        % Should only be called from stochSimulation class
        function this = stochPropensityReaction(objectHandle, simulationHandle, reactionHandle)
            this.objectHandle = objectHandle;
            this.simulationHandle = simulationHandle;
            this.reactionHandle = reactionHandle;
            assert(this.check(), 'Invalid object.');
        end
        
        %% Checks if the reaction points to a valid simulation object
        function valid = check(this)
            valid = isvalid(this) && isvalid(this.simulationHandle);
        end
        
        %% Returns name
        function name = name(this)
            name = this.call('Name');
        end
        
        function addProduct(this, state, varargin)
            if ~ischar(state)
                state = state.getStateHandle();
            end
            this.call('AddProduct', state, varargin{:});
        end
        
        function addReactant(this, state, varargin)
            if ~ischar(state)
                state = state.getStateHandle();
            end
            this.call('AddReactant', state, varargin{:});
        end
    end
end