classdef stochTimerReaction < stochSimulationComponent
% A reaction or event which fires a single time at exactly the
% simulation time specified. The reaction can only contain
% products but no reactans. Useful to e.g. simulate the
% addition of some ligands at a specific time.
% Can only be constructed via a stochSimulation object.
   methods(Static, Hidden = true)
        function className = getClassName()
            % Returns a unique name for this type of simulation component,
            % which is used in the communication with the C++ simulator to
            % identify the class of the component to which componentHandle is a handle.
            className = 'TimerReaction';
        end
    end
    methods(Access = ?stochSimulation)
        %% Constructor
        function this = stochTimerReaction(simulationHandle, reactionHandle)
            % Create a new timer reaction.
            % Should only be called from stochSimulation class.
            this = this@stochSimulationComponent(simulationHandle, reactionHandle);
        end
    end
     properties(SetAccess = private, GetAccess=public,Dependent)
        % Unique name of the reaction.
        name;
        % Products, that is, species which get produced by the reaction.
        products;
        % Stochiometries of products.
        productStochiometries;
        % String representation of the reaction, e.g. A -> 2 B.
        formula;
    end
    properties(Dependent)
        % Time, in sumulation time units, when the timer reaction should
        % fire.
        fireTime;
    end
    methods
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
        
        function products = get.products(this)
            [productRefs, ~] = this.call('GetProducts');
            products = cell(size(productRefs));
            for i=1:length(productRefs)
                products{i} = this.simulationHandle.getState(productRefs{i});
            end
        end
        function productStochiometries = get.productStochiometries(this)
            [~, productStochiometries] = this.call('GetProducts');
        end
        function formula = get.formula(this)
            iff = @(varargin) varargin{2 * find([varargin{1:2:end}], 1, 'first')}();
            toString = @(state, stoch) ...
                iff(stoch>1, @() sprintf('%g*%s', stoch, this.simulationHandle.getState(state{1}).name),...
                    true   , @()this.simulationHandle.getState(state{1}).name);
            
            [productRefs, productStochiometries] = this.call('GetProducts');
            products = arrayfun(toString, productRefs, productStochiometries, 'UniformOutput', false);
            
            formula = [' -> ', strjoin(products, ' + ')];
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