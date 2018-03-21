classdef stochChoice < stochSimulationComponent & matlab.mixin.CustomDisplay
    % A choice is a special kind of state in stochsim. When a choice is a product of a reaction and the reaction fires, the concentration of the choice is not increased. 
    % Instead, the boolean formula (condition)
    % associated with this choice is evaluated. Depending on the outcome of this evaluation, either the concentrations of one or the other set of states (products) is increased according to their
    % stochiometry.
    % The typical use case is to use a Choice to implement conditionals for reactions. For example, a reaction might result in a product B with a certain probability, and in a product C with another, i.e.
    %      { B     if rand()>0.2
    % A -> {
    %      { C     otherwise.
    % This would then be implemented as a propensity reaction with A as the reactant, and a choice as the product. The boolean formula associated to the choice would be "rand()>0.2", the first set of
    % products of the choice would contain only B with stochiometry 1, and the second set only C with stochiometry 1.
    % The usage of a choice becomes specifically interesting when combined with reactions providing additional information (variables) which can be used in the boolean expression. For example, a delay
    % reaction passes the number of how often its (sole) reactant was transformed.
    % Can only be constructed via a stochSimulation object.
    methods(Static, Hidden = true)
        function className = getClassName()
            % Returns a unique name for this type of simulation component,
            % which is used in the communication with the C++ simulator to
            % identify the class of the component to which componentHandle is a handle.
            className = 'Choice';
        end
    end
    properties(SetAccess = private, GetAccess=public,Dependent)
        % Unique name of the choice.
        name;
        
        % Products of the choice if the choice equation evaluates to true.
        productsIfTrue;
        % Stochiometries of products if the choice equation evaluates to true.
        productStochiometriesIfTrue;
        % Property expressions of product molecules  if the choice equation evaluates to true.
        productPropertyExpressionsIfTrue;
        
        % Products of the choice if the choice equation evaluates to false.
        productsIfFalse;
        % Stochiometries of products if the choice equation evaluates to false.
        productStochiometriesIfFalse;
        % Property expressions of product molecules  if the choice equation evaluates to false.
        productPropertyExpressionsIfFalse;
    end
    properties(Dependent)
        % A string representing a boolean equation which gets evaluated
        % every time the choice is invoked (i.e. when the choice is a
        % product of a reaction and the reaction fires). Depending on if
        % this equation evaluates to true or false, either the
        % concentrations of the products in productsIfTrue or in
        % productsIfFalse is increased according to their stochiometry.
        condition;
    end
    methods(Access = {?stochSimulation})
        %% Constructor
        function this = stochChoice(simulationHandle, stateHandle)
            % Create a new stochsim choice.
            % Should only be called from stochSimulation class.
            this = this@stochSimulationComponent(simulationHandle, stateHandle);
        end
    end
    methods(Access = protected)
        %% Display methods
        function header = getHeader(this)
            if ~isscalar(this)
                header = getHeader@matlab.mixin.CustomDisplay(this);
            else
                className = matlab.mixin.CustomDisplay.getClassNameForHeader(this);
                nameLength = 18;
                header = sprintf([...
                    'StochSim choice (%s):\n',...
                    '\t%-', int2str(nameLength), 's %s\n'...
                    '\t%-', int2str(nameLength), 's %s\n'...
                    '\t%-', int2str(nameLength), 's %s\n'...
                    '\t%-', int2str(nameLength), 's %s\n'...
                    '\nProperties:'...
                    ], className, ...
                    'Name:', this.name, ...
                    'Choice Equation:', this.condition,...
                    'Products if True:', this.formatFormula('%3$s'),...
                    'Products if False:', this.formatFormula('%4$s'));
            end
        end
        
        function s = getFooter(this)
            s = matlab.mixin.CustomDisplay.getDetailedFooter(this);
        end
    end
    methods  
        %% Getters and setters for properties
        function name = get.name(this)
            % Returns the unique name of this state.
            name = this.call('GetName');
        end
        
        function products = get.productsIfTrue(this)
            [productRefs, ~] = this.call('GetProductsIfTrue');
            products = cell(size(productRefs));
            for i=1:length(productRefs)
                products{i} = this.simulationHandle.getState(productRefs{i});
            end
        end
        function productStochiometries = get.productStochiometriesIfTrue(this)
            [~, productStochiometries] = this.call('GetProductsIfTrue');
        end
        function productPropertyExpressions = get.productPropertyExpressionsIfTrue(this)
            [~, ~, productPropertyExpressions] = this.call('GetProductsIfTrue');
        end
        
        function products = get.productsIfFalse(this)
            [productRefs, ~] = this.call('GetProductsIfFalse');
            products = cell(size(productRefs));
            for i=1:length(productRefs)
                products{i} = this.simulationHandle.getState(productRefs{i});
            end
        end
        function productStochiometries = get.productStochiometriesIfFalse(this)
            [~, productStochiometries] = this.call('GetProductsIfFalse');
        end
        function productPropertyExpressions = get.productPropertyExpressionsIfFalse(this)
            [~, ~, productPropertyExpressions] = this.call('GetProductsIfFalse');
        end
        
        function condition = get.condition(this)
            condition = this.call('GetCondition');
        end
        function set.condition(this, condition)
            this.call('SetCondition', condition);
        end
        
        %% Products, reactants and similar.
        function addProductIfTrue(this, state, stochiometry)
            % Adds a species as a product of the choice, whose
            % concentration is increased according to its stochiometry if
            % the choice is invoked (a reaction where this choice is
            % assigned as a product fires), and if the boolean equation
            % associated to this choice evaluates to true.
            % Usage:
            %   addProductIfTrue(this, state)
            %   addProductIfTrue(this, state, stochiometry)
            % Parameters:
            %   state           ...name or reference to state to add.
            %   stochiometry    ...number of molecules assumed to be
            %                      produced. Default = 1.
            if ~ischar(state)
                state = state.componentHandle;
            end
            if ~exist('stochiometry', 'var') || isempty(stochiometry)
                stochiometry = 1;
            end
            this.call('AddProductIfTrue', state, stochiometry);
        end   
        
        function addProductIfFalse(this, state, stochiometry)
            % Adds a species as a product of the choice, whose
            % concentration is increased according to its stochiometry if
            % the choice is invoked (a reaction where this choice is
            % assigned as a product fires), and if the boolean equation
            % associated to this choice evaluates to false.
            % Usage:
            %   addProductIfFalse(this, state)
            %   addProductIfFalse(this, state, stochiometry)
            % Parameters:
            %   state           ...name or reference to state to add.
            %   stochiometry    ...number of molecules assumed to be
            %                      produced. Default = 1.
            if ~ischar(state)
                state = state.componentHandle;
            end
            if ~exist('stochiometry', 'var') || isempty(stochiometry)
                stochiometry = 1;
            end
            this.call('AddProductIfFalse', state, stochiometry);
        end   
        
        function formula = formatFormula(this, format, numberFormat) %#ok<INUSD>
            % Displays the formula of this choice according to
            % the provided format string. In this format string, 
            % '%1$s'corresponds to the name of the choice, 
            % '%2$s'corresponds to the choice equation, 
            % '%3$s' to the products if the choice equation evaluates to true, and 
            % '%4$s' to the products if the choice equation evaluates to false.
            % Usage: 
            %   formatFormula(this)
            %   formatFormula(this, format)
            %   formatFormula(this, format, numberFormat)
            % Parameters:
            %   format          The format string. See e.g. sprintf.
            %   numberFormat    The format to convert numbers.
            %                   Currently not used, but kept for
            %                   compatibility with similarly named
            %                   functions in reactions.
            %                   Default = '%g';
            % Returns:
            %   A string describing the choice formatted according to the
            %   prodided string.
            % Examples:
            %   this.formatFormula('%2$s ? %3$s : %4$s')
            %       returns 'rand()>0.2 ? 2A : A + B'
            %   this.formatFormula('%1$ = %2$s ? %3$s : %4$s')
            %       returns 'mychoice = rand()>0.2 ? 2A : A + B'
            if nargin < 2
                format = '%2$s ? %3$s : %4$s';
            end
            if nargin < 3
                numberFormat = '%g'; %#ok<NASGU>
            end
            iff = @(varargin) varargin{2 * find([varargin{1:2:end}], 1, 'first')}();
            toString = @(state, stoch, properties) ...
                iff(stoch>1, @() sprintf('%g*%s%s', stoch, this.simulationHandle.getState(state{1}).name, stochSimulationComponent.formatProperties(properties{1})),...
                    true   , @()[this.simulationHandle.getState(state{1}).name, stochSimulationComponent.formatProperties(properties{1})]);
            
            [productsIfTrueRefs, productStochiometriesIfTrue, productPropertyExpressionsIfTrue] = this.call('GetProductsIfTrue');
            productsIfTrue = arrayfun(toString, productsIfTrueRefs, productStochiometriesIfTrue, productPropertyExpressionsIfTrue, 'UniformOutput', false);
            
            [productsIfFalseRefs, productStochiometriesIfFalse, productPropertyExpressionsIfFalse] = this.call('GetProductsIfFalse');
            productsIfFalse = arrayfun(toString, productsIfFalseRefs, productStochiometriesIfFalse, productPropertyExpressionsIfFalse, 'UniformOutput', false);
            
            formula = sprintf(format, ...
                this.name, ...
                this.condition, ...
                strjoin(productsIfTrue, ' + '), ...
                strjoin(productsIfFalse, ' + '));
        end
        function cmdl = getCmdl(this)
            % Returns a string representing the cmdl command to
            % instantiate this reaction, 
            % e.g. 'reaction5, -> 2 B, time=5.3;'. 
            format = '%1$ = %2$s ? %3$s : %4$s;';
            cmdl = this.formatFormula(format);
        end
    end
end