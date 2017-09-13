classdef stochComposedState < stochState
% A composed state is a state where each molecule represented by the
% composed state has an own identity. Specifically, for each molecule it is
% defined separately when it was created, and how often it was transformed.
% A composed state can thus be used in e.g. a delay reaction where each
% molecule should be consumed exactly a given time after it was produced.
% Can only be constructed via a stochSimulation object.
    
    methods(Static, Hidden = true)
        function className = getClassName()
            className = 'ComposedState';
        end
    end
    methods(Access = ?stochSimulation)
        %% Constructor
        function this = stochComposedState(simulationHandle, stateHandle)
            % Create a new stochsim ComposedState
            % Should only be called from stochSimulation class
            this@stochState(simulationHandle, stateHandle); 
        end
    end
    methods        
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
    end
end