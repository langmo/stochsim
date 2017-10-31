classdef stochChoice < stochSimulationComponent & matlab.mixin.CustomDisplay
    % A choice is a special kind of state in stochsim. When a choice is a product of a reaction and the reaction fires, the concentration of the choice is not increased. 
    % Instead, the boolean formula (condition)
    % associated with this choice is evaluated. Depending on the outcome of this evaluation, either the concentrations of one or the other set of states (elements) is increased according to their
    % stochiometry.
    % The typical use case is to use a Choice to implement conditionals for reactions. For example, a reaction might result in a product B with a certain probability, and in a product C with another, i.e.
    %      { B     if rand()>0.2
    % A -> {
    %      { C     otherwise.
    % This would then be implemented as a propensity reaction with A as the reactant, and a choice as the product. The boolean formula associated to the choice would be "rand()>0.2", the first set of
    % elements of the choice would contain only B with stochiometry 1, and the second set only C with stochiometry 1.
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
        
        % Elements of the choice if the choice equation evaluates to true.
        elementsIfTrue;
        % Stochiometries of elements if the choice equation evaluates to true.
        elementStochiometriesIfTrue;
        
        % Elements of the choice if the choice equation evaluates to false.
        elementsIfFalse;
        % Stochiometries of elements if the choice equation evaluates to false.
        elementStochiometriesIfFalse;
    end
    properties(Dependent)
        % A string representing a boolean equation which gets evaluated
        % every time the choice is invoked (i.e. when the choice is a
        % product of a reaction and the reaction fires). Depending on if
        % this equation evaluates to true or false, either the
        % concentrations of the elements in elementsIfTrue or in
        % elementsIfFalse is increased according to their stochiometry.
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
                    'Elements if True:', this.formatFormula('%3$s'),...
                    'Elements if False:', this.formatFormula('%4$s'));
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
        
        function elements = get.elementsIfTrue(this)
            [elementRefs, ~] = this.call('GetElementsIfTrue');
            elements = cell(size(elementRefs));
            for i=1:length(elementRefs)
                elements{i} = this.simulationHandle.getState(elementRefs{i});
            end
        end
        function elementStochiometries = get.elementStochiometriesIfTrue(this)
            [~, elementStochiometries] = this.call('GetElementsIfTrue');
        end
        
        function elements = get.elementsIfFalse(this)
            [elementRefs, ~] = this.call('GetElementsIfFalse');
            elements = cell(size(elementRefs));
            for i=1:length(elementRefs)
                elements{i} = this.simulationHandle.getState(elementRefs{i});
            end
        end
        function elementStochiometries = get.elementStochiometriesIfFalse(this)
            [~, elementStochiometries] = this.call('GetElementsIfFalse');
        end
        
        function condition = get.condition(this)
            condition = this.call('GetCondition');
        end
        function set.condition(this, condition)
            this.call('SetCondition', condition);
        end
        
        %% Products, reactants and similar.
        function addElementIfTrue(this, state, stochiometry)
            % Adds a species as a element of the choice, whose
            % concentration is increased according to its stochiometry if
            % the choice is invoked (a reaction where this choice is
            % assigned as a product fires), and if the boolean equation
            % associated to this choice evaluates to true.
            % Usage:
            %   addElementIfTrue(this, state)
            %   addElementIfTrue(this, state, stochiometry)
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
            this.call('AddElementIfTrue', state, stochiometry);
        end   
        
        function addElementIfFalse(this, state, stochiometry)
            % Adds a species as a element of the choice, whose
            % concentration is increased according to its stochiometry if
            % the choice is invoked (a reaction where this choice is
            % assigned as a product fires), and if the boolean equation
            % associated to this choice evaluates to false.
            % Usage:
            %   addElementIfFalse(this, state)
            %   addElementIfFalse(this, state, stochiometry)
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
            this.call('AddElementIfFalse', state, stochiometry);
        end   
        
        function formula = formatFormula(this, format, numberFormat) %#ok<INUSD>
            % Displays the formula of this choice according to
            % the provided format string. In this format string, 
            % '%1$s'corresponds to the name of the choice, 
            % '%2$s'corresponds to the choice equation, 
            % '%3$s' to the elements if the choice equation evaluates to true, and 
            % '%4$s' to the elements if the choice equation evaluates to false.
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
            toString = @(state, stoch) ...
                iff(stoch>1, @() sprintf('%g*%s', stoch, this.simulationHandle.getState(state{1}).name),...
                    true   , @()this.simulationHandle.getState(state{1}).name);
            
            [elementsIfTrueRefs, elementStochiometriesIfTrue] = this.call('GetElementsIfTrue');
            elementsIfTrue = arrayfun(toString, elementsIfTrueRefs, elementStochiometriesIfTrue, 'UniformOutput', false);
            
            [elementsIfFalseRefs, elementStochiometriesIfFalse] = this.call('GetElementsIfFalse');
            elementsIfFalse = arrayfun(toString, elementsIfFalseRefs, elementStochiometriesIfFalse, 'UniformOutput', false);
            
            formula = sprintf(format, ...
                this.name, ...
                this.condition, ...
                strjoin(elementsIfTrue, ' + '), ...
                strjoin(elementsIfFalse, ' + '));
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