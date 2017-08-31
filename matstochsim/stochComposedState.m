classdef stochComposedState < stochState
    methods
        %% Constructor - Create a new stochsim ComposedState
        % Should only be called from stochSimulation class
        function this = stochComposedState(objectHandle, simulationHandle, stateHandle)
            this@stochState(objectHandle, simulationHandle, stateHandle); 
        end
        
        function saveFinalNumModificationsToFile(this, fileName, varargin)
            this.call('SaveFinalNumModificationsToFile', fileName, varargin{:});
        end
    end
end