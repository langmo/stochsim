classdef stochComposedState < stochSimulationComponent & matlab.mixin.CustomDisplay
% A composed state is a state where each molecule represented by the
% composed state has an own identity. Specifically, for each molecule it is
% defined separately when it was created, and how often it was transformed.
% A composed state can thus be used in e.g. a delay reaction where each
% molecule should be consumed exactly a given time after it was produced.
% Can only be constructed via a stochSimulation object.
    methods(Static, Hidden = true)
        function className = getClassName()
            % Returns a unique name for this type of simulation component,
            % which is used in the communication with the C++ simulator to
            % identify the class of the component to which componentHandle is a handle.
            className = 'ComposedState';
        end
    end
     properties(SetAccess = private, GetAccess=public,Dependent)
        % Unique name of the state.
        name;
    end
    properties(Dependent)
        % Initial condition of the state. Must be greater or equal to zero.
        initialCondition;
    end
    methods(Access = {?stochSimulation})
        %% Constructor
        function this = stochComposedState(simulationHandle, stateHandle)
            % Create a new stochsim composed state.
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
                    'StochSim composed state (%s):\n',...
                    '\t%-', int2str(nameLength), 's %s\n'...
                    '\t%-', int2str(nameLength), 's %g\n'...
                    '\nProperties:'...
                    ], className, ...
                    'Name:', this.name, ...
                    'Initial Condition:', this.initialCondition);
            end
        end
        
        function s = getFooter(this)
            s = matlab.mixin.CustomDisplay.getDetailedFooter(this);
        end
    end
    methods        
        %% Getters and setters for properties
        function ic = get.initialCondition(this)
            % Returns the initial condition (in molec) of this state.
            ic = this.call('GetInitialCondition');
        end
        
        function set.initialCondition(this, ic)
            % Sets the initial condition (in molec) of this state.
            this.call('SetInitialCondition', ic);
        end
        
        function name = get.name(this)
            % Returns the unique name of this state.
            name = this.call('GetName');
        end
        %% Methods
        function logDecreaseToFile(this, fileName, propertyIdx, maxValue)
            % Specifies the name of a file where the value of
            % a given property of each molecule represented by this state
            % is saved whenever the corresponding state is decreased.
            % Set the file name to an empty string to not save this
            % information. The file name has typically the ending CSV.
            % Usage:
            %   logDecreaseToFile(this, fileName)
			%   logDecreaseToFile(this, fileName, propertyIdx)
            %   logDecreaseToFile(this, fileName, propertyIdx, maxValue)
            % Parameters:
            %   fileName      - Name of the file to save number of
            %                   transformations, or empty string to not save
            %                   this information.
			%   propertyIdx   - ID of the molecule property (zero based). 
			%                   Default = 0.
            %   maxValue      - Estimate of the maximal value of the property. 
			%                   The maximal value is automatically increased
            %                   if a molecule has a higher property value.
            %                   Default=20.
            
            if isempty(fileName)
                fileName = '';
            end
			 if nargin < 3 || isempty(propertyIdx)
                propertyIdx = 0;
            end
            if nargin < 4 || isempty(maxValue)
                maxValue = 20;
            end
            this.call('LogDecreaseToFile', fileName, propertyIdx, maxValue);
        end
        function logIncreaseToFile(this, fileName, propertyIdx, maxValue)
            % Specifies the name of a file where the value of
            % a given property of each molecule represented by this state
            % is saved whenever the corresponding state is increased.
            % Set the file name to an empty string to not save this
            % information. The file name has typically the ending CSV.
            % Usage:
            %   logIncreaseToFile(this, fileName)
			%   logIncreaseToFile(this, fileName, propertyIdx)
            %   logIncreaseToFile(this, fileName, propertyIdx, maxValue)
            % Parameters:
            %   fileName      - Name of the file to save number of
            %                   transformations, or empty string to not save
            %                   this information.
			%   propertyIdx   - ID of the molecule property (zero based). 
			%                   Default = 0.
            %   maxValue      - Estimate of the maximal value of the property. 
			%                   The maximal value is automatically increased
            %                   if a molecule has a higher property value.
            %                   Default=20.
            
            if isempty(fileName)
                fileName = '';
            end
			 if nargin < 3 || isempty(propertyIdx)
                propertyIdx = 0;
            end
            if nargin < 4 || isempty(maxValue)
                maxValue = 20;
            end
            this.call('LogIncreaseToFile', fileName, propertyIdx, maxValue);
        end
        function cmdl = getCmdl(this)
            % Returns a string representing the cmdl command to
            % instantiate this state, e.g. 'A = 10;'. 
            cmdl = sprintf('%s = %g;', this.name, this.initialCondition);
        end
    end
end