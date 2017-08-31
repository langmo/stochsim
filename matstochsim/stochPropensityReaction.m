classdef stochPropensityReaction < handle
    properties (SetAccess = private, GetAccess=private, Hidden = true)
        objectHandle; % Handle to the underlying C++ class instance
        simulationHandle; % Handle to simulation object to which this state belongs. Used for checking if the object handle is still valid.
        reactionHandle; % Handle to the state this object represents
    end
    properties (Constant,GetAccess=private, Hidden = true)
        Prefix = 'PropensityReaction::';
    end
    methods (Static,Access = private, Hidden = true)
        function qualifiedName = prefix(functionName)
            qualifiedName = [stochPropensityReaction.Prefix, functionName];
        end
    end
    methods (Access = private, Hidden = true)
        function varargout = call(this, functionName, varargin)
            assert(this.check(), 'Invalid object.');
            [varargout{1:nargout}] = matstochsim([stochPropensityReaction.Prefix, functionName], this.objectHandle, this.reactionHandle, varargin{:});
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
        
        %% Checks if the state points to a valid simulation object
        function valid = check(this)
            valid = isvalid(this) && isvalid(this.simulationHandle);
        end
        
        %% Returns name
        function name = name(this)
            %assert(this.check(), 'Invalid object.');
            %name = matstochsim(stochPropensityReaction.prefix('Name'), this.objectHandle, this.reactionHandle);
            name = this.call('Name');
        end
        
        function addProduct(this, state, varargin)
            %assert(this.check(), 'Invalid object.');
            %matstochsim(stochPropensityReaction.prefix('AddProduct'), this.objectHandle, this.reactionHandle, state.getStateHandle(), varargin{:});
            this.call('AddProduct', state.getStateHandle(), varargin{:});
        end
        
        function addReactant(this, state, varargin)
            %assert(this.check(), 'Invalid object.');
            %matstochsim(stochPropensityReaction.prefix('AddReactant'), this.objectHandle, this.reactionHandle, state.getStateHandle(), varargin{:});
            this.call('AddReactant', state.getStateHandle(), varargin{:});
        end
    end
end