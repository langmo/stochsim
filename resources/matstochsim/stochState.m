classdef stochState < stochSimulationComponent & matlab.mixin.CustomDisplay
% A state of a stochsim, typically representing the molecular count of some
% species like proteins, mRNA, genes, inducers or similar.
% Can only be constructed via a stochSimulation object.
    methods(Static, Hidden = true)
        function className = getClassName()
            % Returns a unique name for this type of simulation component,
            % which is used in the communication with the C++ simulator to
            % identify the class of the component to which componentHandle is a handle.
            className = 'State';
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
        function this = stochState(simulationHandle, stateHandle)
            % Create a new stochsim state.
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
                nameLength = 15;
                header = sprintf([...
                    'StochSim state (%s):\n',...
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
        
        function cmdl = getCmdl(this)
            % Returns a string representing the cmdl command to
            % instantiate this state, e.g. 'A = 10;'. 
            cmdl = sprintf('%s = %g;', this.name, this.initialCondition);
        end
    end
end