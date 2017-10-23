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
        function saveFinalNumModificationsToFile(this, fileName, maxTransformed)
            % Specifies the name of a file where the number of
            % transformations of each molecule represented by this state
            % are saved whenever the corresponding molecule is consumed.
            % Set the file name to an empty string to not save this
            % information. The file name has typically the ending CSV.
            % Usage:
            %   saveFinalNumModificationsToFile(this, fileName)
            %   saveFinalNumModificationsToFile(this, fileName, maxTransformed)
            % Parameters:
            %   fileName      - Name of the file to save number of
            %                   transformations, or empty string to not save
            %                   this information.
            %   maxTransformed- Estimate of the maximal number of times a
            %                   molecule of this state can be transformed
            %                   before being consumed. The maximal number
            %                   is automatically increased if a molecule is
            %                   modified more often during a simulation,
            %                   but determines the initial column number of
            %                   the CSV file. Default=20.
            
            if isempty(fileName)
                fileName = '';
            end
            if nargin < 3 || isempty(maxTransformed)
                maxTransformed = 20;
            end
            this.call('SaveFinalNumModificationsToFile', fileName, maxTransformed);
        end
        
        function cmdl = getCmdl(this)
            % Returns a string representing the cmdl command to
            % instantiate this state, e.g. 'A = 10;'. 
            cmdl = sprintf('%s = %g;', this.name, this.initialCondition);
        end
    end
end