classdef stochDelayReaction < stochSimulationComponent
% A reaction which fires exactly after a given time a
% new molecule of the composed state is produced. Note that
% such delay reactions require the creation time of every
% molecule represented by this state to be saved separately,
% and can thus only accept composed states as their sole
% reactant. When the reaction fires, the respective molecule of
% the composed state is consumed.
% Can only be constructed via a stochSimulation object.
    methods(Static, Hidden = true)
        function className = getClassName()
            % Returns a unique name for this type of simulation component,
            % which is used in the communication with the C++ simulator to
            % identify the class of the component to which componentHandle is a handle.
            className = 'DelayReaction';
        end
    end
    methods(Access = ?stochSimulation)
        %% Constructor
        function this = stochDelayReaction(simulationHandle, reactionHandle)
            % Create a new delay reaction.
            % Should only be called from stochSimulation class.
            this = this@stochSimulationComponent(simulationHandle, reactionHandle);
        end
    end
     properties(SetAccess = private, GetAccess=public,Dependent)
        % Unique name of the reaction.
        name;
    end
    properties(Dependent)
        % The delay after the creation of a molecule of the composed state
        % when this reaction fires.
        delay;
    end
    methods        
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