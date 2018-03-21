classdef stochPropensityReaction < stochSimulationComponent & matlab.mixin.CustomDisplay
% A propensity based ("normal") reaction. The
% propensity ("rate") of the reaction is calculated using mass
% action kinetics and can be considered to represent the
% average number of times the reaction fires during a certain
% time period.
% Can only be constructed via a stochSimulation object.
    methods(Static, Hidden = true)
        function className = getClassName()
            % Returns a unique name for this type of simulation component,
            % which is used in the communication with the C++ simulator to
            % identify the class of the component to which componentHandle is a handle.
            className = 'PropensityReaction';
        end
    end
    methods(Access = ?stochSimulation)
        %% Constructor
        function this = stochPropensityReaction(simulationHandle, reactionHandle)
            % Create a new propensity reaction.
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
        % Modifiers, that is, species which get neither produced nor
        % consumed by the reaction, but which modify the reaction rate
        % according to their stochiometry following mass action kinetics.
        % An example for modifiers are enzymes catalyzing a reaction.
        modifiers;
        % Stochiometries of modifiers.
        modifierStochiometries;
        % Property names of modifier molecules.
        modifierPropertyNames;
        % Transformees, that is, species which get neither produced nor
        % consumed by the reaction, but which modify the reaction rate
        % according to their stochiometry following mass action kinetics.
        % Additionally, when participating in a reaction, a transformee is
        % transformed. While in principle many ways of getting transformed
        % are possible, currently the only implemented transformation is a
        % simple counting of how often the respective molecule took part as
        % a transformee in a reaction.
        transformees;
        % Stochiometries of transformees.
        transformeeStochiometries;
        % Property names of transformee molecules.
        transformeePropertyNames;
         % Property expressions of product molecules.
        transformeePropertyExpressions;
    end
    properties(Dependent)
        % The rate constant of the reaction used to calculate the
        % propensity ("rate") of the reaction using mass action kinetics.
        % Only used to determine the reaction rate if no custom rate
        % equation is set.
        rateConstant;
        % A custom rate equation. When a custom rate equation is set, this
        % equation is used to determine the reaction rate dynamically
        % instead of calculating the rate using mass action kinetics.
        rateEquation;
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
                rateConstant = this.rateConstant;
                if rateConstant ~= -1
                    rateStr = mat2str(rateConstant);
                    rateName = 'Rate Constant:';
                else
                    rateStr = this.rateEquation;
                    rateName = 'Rate Equation:';
                end
                header = sprintf([...
                    'StochSim propensity reaction (%s):\n',...
                    '\t%-', int2str(nameLength), 's %s\n'...
                    '\t%-', int2str(nameLength), 's %s\n'...
                    '\nProperties:'...
                    ], className, ...
                    'Formula:', formula, ...
                    rateName, rateStr);
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
        function rateConstant = get.rateConstant(this)
            rateConstant = this.call('GetRateConstant');
        end
        function set.rateConstant(this, rateConstant)
            this.call('SetRateConstant', rateConstant);
        end
        
        function rateEquation = get.rateEquation(this)
            rateEquation = this.call('GetRateEquation');
            if isempty(rateEquation)
                rateEquation = [];
            end
        end
        function set.rateEquation(this, rateEquation)
            this.call('SetRateEquation', rateEquation);
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
        
        function modifiers = get.modifiers(this)
            [moifierRefs, ~] = this.call('GetModifiers');
            modifiers = cell(size(moifierRefs));
            for i=1:length(moifierRefs)
                modifiers{i} = this.simulationHandle.getState(moifierRefs{i});
            end
        end
        function modifierStochiometries = get.modifierStochiometries(this)
            [~, modifierStochiometries] = this.call('GetModifiers');
        end
        function modifierPropertyNames = get.modifierPropertyNames(this)
            [~, ~, modifierPropertyNames] = this.call('GetModifiers');
        end
        
        function transformees = get.transformees(this)
            [transformeeRefs, ~] = this.call('GetTransformees');
            transformees = cell(size(transformeeRefs));
            for i=1:length(transformeeRefs)
                transformees{i} = this.simulationHandle.getState(transformeeRefs{i});
            end
        end
        function transformeeStochiometries = get.transformeeStochiometries(this)
            [~, transformeeStochiometries] = this.call('GetTransformees');
        end
        function transformeePropertyNames = get.transformeePropertyNames(this)
            [~, ~,~,transformeePropertyNames] = this.call('GetTransformees');
        end
        function transformeePropertyExpressions = get.transformeePropertyExpressions(this)
            [~, ~,transformeePropertyExpressions] = this.call('GetTransformees');
        end
        
        function formula = formatFormula(this, format, numberFormat)
            % Displays the chemical formula of this reaction according to
            % the provided format string. In this format string, '%1$s'
            % corresponds to the reaction name, '%2$s' to the reactants,
            % '%3$s' to the products and '%4$s' to the rate constant or to
            % the rate equation. 
            % Usage: 
            %   formatFormula(this)
            %   formatFormula(this, format)
            %   formatFormula(this, format, numberFormat)
            % Parameters:
            %   format          The format string. See e.g. sprintf.
            %                   Default = '%2$s -> %3$s'.
            %   numberFormat    The format to convert numbers like the
            %                   reaction rate constant to strings.
            %                   Default = '%g';
            % Returns:
            %   A string describing the reaction formatted according to the
            %   prodided string.
            % Examples:
            %   this.formatFormula('%2$s -> %3$s')
            %       returns 'A + B -> C'
            %   this.formatFormula('%1$s, %2$s -> %3$s, %4$s')
            %       returns 'reaction1, A + B -> C, 3.141'  
            %   this.formatFormula('%2$s becomes %3$s in %1$s with rate %4$s')
            %       returns 'A + B becomes C in reaction1 with rate 3.141'  
            
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
            
            [modifierRefs, modifierStochiometries, modifierPropertyNames] = this.call('GetModifiers');
            modifiers = arrayfun(toString, modifierRefs, modifierStochiometries, modifierPropertyNames, 'UniformOutput', false);
            modifiers = cellfun(@(string)['$',string], modifiers, 'UniformOutput', false);
            
            [transformeeRefs, transformeeStochiometries, transformeePropertyExpressions, transformeePropertyNames] = this.call('GetTransformees');
            transformeesLeft = arrayfun(toString, transformeeRefs, transformeeStochiometries, transformeePropertyNames, 'UniformOutput', false);
            transformeesLeft = cellfun(@(string)['$',string], transformeesLeft, 'UniformOutput', false);
            transformeesRight = arrayfun(toString, transformeeRefs, transformeeStochiometries, transformeePropertyExpressions, 'UniformOutput', false);
            transformeesRight = cellfun(@(string)['$',string], transformeesRight, 'UniformOutput', false);
            
            rateConstant = this.rateConstant;
            if rateConstant ~=-1
                rateStr = sprintf(numberFormat, this.rateConstant);
            else
                rateStr = ['[', this.rateEquation, ']'];
            end
            
            formula = sprintf(format, ...
                this.name, ...
                strjoin([reactants, modifiers, transformeesLeft], ' + '), ...
                strjoin([products, transformeesRight], ' + '), ...
                rateStr);
        end
        function formula = getFormula(this)
            % Returns a string representation of the reaction, e.g. A -> 2 B.
            formula = this.formatFormula();
        end
        function cmdl = getCmdl(this)
            % Returns a string representing the cmdl command to
            % instantiate this reaction, e.g. 'reaction5, A -> 2 B, 7.1;'. 
            format = '%1$s, %2$s -> %3$s, %4$s;';
            cmdl = this.formatFormula(format);
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
                state = state.componentHandle;
            end
            if ~exist('stochiometry', 'var') || isempty(stochiometry)
                stochiometry = 1;
            end
            this.call('AddProduct', state, stochiometry);
        end
        
        function addReactant(this, state, stochiometry)
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
            this.call('AddReactant', state, stochiometry);
        end
        
        function addModifier(this, state, stochiometry)
            % Adds a species as a modifier of the reaction. Different to a
            % reactant or product of a reaction, the molecular number of a
            % modifier is neither increased nor decreased when the reaction
            % fires. However, a modifier still modifies the reaction rate
            % according to its stochiometry following mass action kinetics.
            % It can, thus, e.g. represent an enzyme which is neither
            % produced or consumed by the reaction but still has an
            % influence on the rate.
            % Usage:
            %   addModifier(this, state)
            %   addModifier(this, state, stochiometry)
            % Parameters:
            %   state           ...name or reference to state to add.
            %   stochiometry    ...number of molecules taking part in the
            %                      reaction. Default = 1. 
            if ~ischar(state)
                state = state.componentHandle;
            end
            if ~exist('stochiometry', 'var') || isempty(stochiometry)
                stochiometry = 1;
            end
            this.call('AddModifier', state, stochiometry);
        end
        function addTransformee(this, state, stochiometry)
            % Adds a species as a transformee of the reaction. A
            % transformee is similar to a modifier of a reaction, that is,
            % it is neither produced or consumed when the reaction fires
            % but still influences the reaction rate following mass action
            % kinetics. However, different to a modifier, the molecules of
            % a transformee taking part of the reaction "change" when the
            % reaction fires, that is, they get transformed in some sense.
            % A transformation can mean in principle various things, but
            % currently the only implemented molecule transformation is a
            % simple counting of how often the molecule was transformed.
            % This is e.g. useful when simply wanting to count how often a
            % molecule took part in a reaction.
            % Usage:
            %   addTransformee(this, state)
            %   addTransformee(this, state, stochiometry)
            % Parameters:
            %   state           ...name or reference to state to add.
            %   stochiometry    ...number of molecules taking part and
            %                      being transformed by the reaction.
            %                      Default = 1.  
            if ~ischar(state)
                state = state.componentHandle;
            end
            if ~exist('stochiometry', 'var') || isempty(stochiometry)
                stochiometry = 1;
            end
            this.call('AddTransformee', state, stochiometry);
        end
    end
end