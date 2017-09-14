classdef stochState < stochSimulationComponent
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
    end
end