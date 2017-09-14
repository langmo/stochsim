classdef (Abstract) stochSimulationComponent < handle
% Base class of all elements of a stochsim simulation. Typically represents
% a state or a reaction.
    
    properties (Access = private, Hidden = true)
        simulationHandle; % Handle to simulation object to which this component belongs.
        componentHandle;  % Handle to the component this object represents.
    end
    methods(Static, Abstract, Hidden = true)
        % Returns a unique name for this type of simulation component,
        % which is used in the communication with the C++ simulator to
        % identify the class of the component to which componentHandle is a handle.
        className = getClassName();
    end
    methods(Static, Hidden=true, Access=private)
        function separator = getSeparator()
            separator = '::';
        end
    end
    methods (Access = protected, Hidden = true)
        function varargout = call(this, functionName, varargin)
            assert(this.check(), 'Invalid object.');
            [varargout{1:nargout}] = matstochsim([this.getClassName(), stochSimulationComponent.getSeparator(), functionName], this.simulationHandle.objectHandle, this.componentHandle, varargin{:});
        end
    end
    methods (Access = public, Hidden = true)
        function componentHandle = getComponentHandle(this)
            componentHandle = this.componentHandle;
        end
    end
    properties(SetAccess = private, GetAccess=public,Abstract)
        % Unique name of the component.
        name;
    end
    methods(Access = protected)
        %% Constructor
        function this = stochSimulationComponent(simulationHandle, componentHandle)
            % Create a new stochsim simulation component, e.g. a state or a
            % reaction. 
            % Should only be called from stochSimulation class.
            this.simulationHandle = simulationHandle;
            this.componentHandle = componentHandle;
            assert(this.check(), 'Invalid object.');
        end
    end
    methods  
        %% Validity
        function valid = check(this)
            % Checks if this component is in a valid state.
            % Usage:
            %   valid = check(this)
            % Returns:
            %   valid - True if this component is in a
            %           valid state and can be edited, false otherwise.
            valid = isvalid(this) && isvalid(this.simulationHandle);
        end
    end
end