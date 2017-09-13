classdef stochDelayReaction < handle
% A reaction which fires exactly after a given time a
% new molecule of the composed state is produced. Note that
% such delay reactions require the creation time of every
% molecule represented by this state to be saved separately,
% and can thus only accept composed states as their sole
% reactant. When the reaction fires, the respective molecule of
% the composed state is consumed.
% Can only be constructed via a stochSimulation object.
    properties (SetAccess = private, GetAccess=private, Hidden = true)
        simulationHandle; % Handle to simulation object to which this state belongs. Used for checking if the object handle is still valid.
        reactionHandle; % Handle to the state this object represents
    end
    methods (Access = private, Hidden = true)
        function varargout = call(this, functionName, varargin)
            assert(this.check(), 'Invalid object.');
            [varargout{1:nargout}] = matstochsim(['DelayReaction::', functionName], this.simulationHandle.objectHandle, this.reactionHandle, varargin{:});
        end
    end
    methods(Access = ?stochSimulation)
        %% Constructor - Create a new delay reaction
        % Should only be called from stochSimulation class
        function this = stochDelayReaction(simulationHandle, reactionHandle)
            this.simulationHandle = simulationHandle;
            this.reactionHandle = reactionHandle;
            assert(this.check(), 'Invalid object.');
        end
    end
    properties(Dependent)
        % Unique name of the reaction.
        name;
        % The delay after the creation of a molecule of the composed state
        % when this reaction fires.
        delay;
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
        
        function delay = get.delay(this)
            delay = this.call('GetDelay');
        end
        function set.delay(this, delay)
            this.call('SetDelay', delay);
        end
        %% Products, reactants and similar.
        function addProduct(this, state, stochiometry)
            % Adds a species as a product of the reaction. When the
            % reaction fires, its concentration is increased according to
            % its stochiometry.
            % Usage:
            %   addProduct(this, state)
            %   addProduct(this, state, stochiometry)
            % Parameters:
            %   state           ...name or reference to state to add.
            %   stochiometry    ...number of molecules assumed to be
            %                      produced by the reaction. Default = 1.
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