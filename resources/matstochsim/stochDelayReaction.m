classdef stochDelayReaction < stochSimulationComponent & matlab.mixin.CustomDisplay
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
        % Reactants, that is, species which get consumed by the reaction.
        reactants;
        % Stochiometries of reactants.
        reactantStochiometries;
        % Property names of reactant molecules.
        reactantPropertyNames;
        % Products, that is, species which get produced by the reaction.
        products;
        % Stochiometries of products.
        productStochiometries;
        % Property expressions of product molecules.
        productPropertyExpressions;
    end
    properties(Dependent)
        % The delay after the creation of a molecule of the composed state
        % when this reaction fires.
        delay;
    end
    methods(Access = protected)
        %% Display methods
        function header = getHeader(this)
            if ~isscalar(this)
                header = getHeader@matlab.mixin.CustomDisplay(this);
            else
                className = matlab.mixin.CustomDisplay.getClassNameForHeader(this);
                nameLength = 15;
                formula = this.getFormula();
                header = sprintf([...
                    'StochSim delay reaction (%s):\n',...
                    '\t%-', int2str(nameLength), 's %s\n'...
                    '\t%-', int2str(nameLength), 's %g\n'...
                    '\nProperties:'...
                    ], className, ...
                    'Formula:', formula, ...
                    'Delay:', this.delay);
            end
        end
        
        function s = getFooter(this)
            s = matlab.mixin.CustomDisplay.getDetailedFooter(this);
        end
    end
    methods        
        %% Getters and setters for properties
        function name = get.name(this)
            % Returns the unique name of this reaction.
            name = this.call('GetName');
        end
        function reactants = get.reactants(this)
            [reactantRefs, ~] = this.call('GetReactants');
            reactants = cell(size(reactantRefs));
            for i=1:length(reactantRefs)
                reactants{i} = this.simulationHandle.getState(reactantRefs{i});
            end
        end
        function reactantStochiometries = get.reactantStochiometries(this)
            [~, reactantStochiometries] = this.call('GetReactants');
        end
        function reactantPropertyNames = get.reactantPropertyNames(this)
            [~, ~, reactantPropertyNames] = this.call('GetReactants');
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
        function productPropertyExpressions = get.productPropertyExpressions(this)
            [~, ~, productPropertyExpressions] = this.call('GetProducts');
        end
        
        function delay = get.delay(this)
            delay = this.call('GetDelay');
        end
        function set.delay(this, delay)
            this.call('SetDelay', delay);
        end
        function formula = formatFormula(this, format, numberFormat)
            % Displays the chemical formula of this reaction according to
            % the provided format string. In this format string, '%1$s'
            % corresponds to the reaction name, '%2$s' to the reactants,
            % '%3$s' to the products and '%4$s' to the delay. 
            % Usage: 
            %   formatFormula(this)
            %   formatFormula(this, format)
            %   formatFormula(this, format, numberFormat)
            % Parameters:
            %   format          The format string. See e.g. sprintf.
            %   numberFormat    The format to convert numbers like the
            %                   delay to strings.
            %                   Default = '%g';
            % Returns:
            %   A string describing the reaction formatted according to the
            %   prodided string.
            % Examples:
            %   this.formatFormula('%2$s -> %3$s')
            %       returns 'A + B -> C'
            %   this.formatFormula('%1$s, %2$s -> %3$s, %4$s')
            %       returns 'reaction1, A + B -> C, 3.141'  
            %   this.formatFormula('%2$s becomes %3$s in %1$s with delay %4$s')
            %       returns 'A + B becomes C in reaction1 with delay 3.141'  
            
            if nargin < 2
                format = '%2$s -> %3$s';
            end
            if nargin < 3
                numberFormat = '%g';
            end
            iff = @(varargin) varargin{2 * find([varargin{1:2:end}], 1, 'first')}();
            toString = @(state, stoch, properties) ...
                iff(stoch>1, @() sprintf('%g*%s%s', stoch, this.simulationHandle.getState(state{1}).name, stochSimulationComponent.formatProperties(properties{1})),...
                    true   , @()[this.simulationHandle.getState(state{1}).name, stochSimulationComponent.formatProperties(properties{1})]);
            
            [reactantRefs, reactantStochiometries, reactantPropertyNames] = this.call('GetReactants');
            reactants = arrayfun(toString, reactantRefs, reactantStochiometries, reactantPropertyNames, 'UniformOutput', false);
            
            [productRefs, productStochiometries, productPropertyExpressions] = this.call('GetProducts');
            products = arrayfun(toString, productRefs, productStochiometries, productPropertyExpressions, 'UniformOutput', false);
            
            delayStr = sprintf(numberFormat, this.delay);
            
            formula = sprintf(format, ...
                this.name, ...
                strjoin(reactants, ' + '), ...
                strjoin(products, ' + '), ...
                delayStr);
        end
        function formula = getFormula(this)
            % Returns a string representation of the reaction, e.g. A -> 2 B.
            formula = this.formatFormula();
        end
        function cmdl = getCmdl(this)
            % Returns a string representing the cmdl command to
            % instantiate this reaction, 
            % e.g. 'reaction5, A -> 2 B, delay=5.3;'. 
            format = '%1$s, %2$s -> %3$s, delay:%4$s;';
            cmdl = this.formatFormula(format);
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
                state = state.componentHandle;
            end
            if ~exist('stochiometry', 'var') || isempty(stochiometry)
                stochiometry = 1;
            end
            this.call('AddProduct', state, stochiometry);
        end        
    end
end