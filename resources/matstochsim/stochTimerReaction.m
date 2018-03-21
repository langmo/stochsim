classdef stochTimerReaction < stochSimulationComponent & matlab.mixin.CustomDisplay
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
        % Property expressions of product molecules.
        productPropertyExpressions;
    end
    properties(Dependent)
        % Time, in sumulation time units, when the timer reaction should
        % fire.
        fireTime;
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
                    'Fire Time:', this.fireTime);
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
        function productPropertyExpressions = get.productPropertyExpressions(this)
            [~, ~, productPropertyExpressions] = this.call('GetProducts');
        end
        
        function formula = formatFormula(this, format, numberFormat)
            % Displays the chemical formula of this reaction according to
            % the provided format string. In this format string, '%1$s'
            % corresponds to the reaction name, '%2$s' to the reactants (in
            % this case, an empty string kept for compatibility with other
            % reaction types), '%3$s' to the products and '%4$s' to the
            % time when the reaction fires.
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
            %       returns 'A + B becomes C in reaction1 at time 3.141'  
            
            if nargin < 2
                format = ' -> %3$s';
            end
            if nargin < 3
                numberFormat = '%g';
            end
            iff = @(varargin) varargin{2 * find([varargin{1:2:end}], 1, 'first')}();
            toString = @(state, stoch, properties) ...
                iff(stoch>1, @() sprintf('%g*%s%s', stoch, this.simulationHandle.getState(state{1}).name, stochSimulationComponent.formatProperties(properties{1})),...
                    true   , @()[this.simulationHandle.getState(state{1}).name, stochSimulationComponent.formatProperties(properties{1})]);
            
            [productRefs, productStochiometries, productPropertyExpressions] = this.call('GetProducts');
            products = arrayfun(toString, productRefs, productStochiometries, productPropertyExpressions, 'UniformOutput', false);
            
            timeStr = sprintf(numberFormat, this.fireTime);
            
            formula = sprintf(format, ...
                this.name, ...
                '', ...
                strjoin(products, ' + '), ...
                timeStr);
        end
        function formula = getFormula(this)
            % Returns a string representation of the reaction, e.g. -> 2 B.
            formula = this.formatFormula();
        end
        function cmdl = getCmdl(this)
            % Returns a string representing the cmdl command to
            % instantiate this reaction, 
            % e.g. 'reaction5, -> 2 B, time=5.3;'. 
            format = '%1$s, -> %3$s, time:%4$s;';
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
    end
end