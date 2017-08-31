%stochSimulation Main class to create a stochastic simulation using matstochsim.
classdef stochSimulation < handle
    properties (SetAccess = private, GetAccess=private, Hidden = true)
        objectHandle; % Handle to the underlying C++ class instance
    end
    properties (Constant,GetAccess=private, Hidden = true)
        Prefix = 'Simulation::';
    end
    methods (Static,Access = private, Hidden = true)
        function qualifiedName = prefix(functionName)
            qualifiedName = [stochSimulation.Prefix, functionName];
        end
    end
    methods
        %% Constructor - Create a new stochsim Simulation
        % Usage:
        %   stochSimulation()
        %   stochSimulation(baseFolder)
        %   stochSimulation(baseFolder, logPeriod)
        %   stochSimulation(baseFolder, logPeriod, uniqueSubfolder)
        function this = stochSimulation(varargin)
            this.objectHandle = matstochsim('new');
            if nargin >= 1
                this.setBaseFolder(varargin{1});
            end
            if nargin >= 2
                this.setLogPeriod(varargin{2});
            end
            if nargin >= 3
                this.setUniqueSubfolder(varargin{3});
            else
                this.setUniqueSubfolder(false);
            end
        end
        
        %% Destructor - Destroy the stochsim Simulation 
        function delete(this)
			matstochsim('delete', this.objectHandle);
			this.objectHandle = 0;
        end

        %% createState - creates a state in the simulation
        function state = createState(this, name, initialCondition)
			state = stochState(this.objectHandle, this, matstochsim(stochSimulation.prefix('CreateState'), this.objectHandle, name, initialCondition));
        end
        
        function state = createComposedState(this, name, initialCondition, varargin)
            if nargin < 4
                state = stochState(this.objectHandle, this, matstochsim(stochSimulation.prefix('CreateComposedState'), this.objectHandle, name, initialCondition));
            else
                state = stochState(this.objectHandle, this, matstochsim(stochSimulation.prefix('CreateComposedState'), this.objectHandle, name, initialCondition, varargin{1}));
            end
        end
        %% createReaction - creates a reaction in the simulation
        function reaction = createPropensityReaction(this, name, rateConstant)
			reaction = stochPropensityReaction(this.objectHandle, this, matstochsim(stochSimulation.prefix('CreatePropensityReaction'), this.objectHandle, name, rateConstant));
        end
        function reaction = createDelayReaction(this, name, state, delay)
			reaction = stochDelayReaction(this.objectHandle, this, matstochsim(stochSimulation.prefix('CreateDelayReaction'), this.objectHandle, name, state.getStateHandle(), delay));
        end
        
        %% run - Executes the simulation for the given runtime
        function run(this, runtime)
			matstochsim(stochSimulation.prefix('Run'), this.objectHandle, runtime);
        end
        
        %% Configure save settings
        function setLogPeriod(this, logPeriod)
            matstochsim(stochSimulation.prefix('SetLogPeriod'), this.objectHandle, logPeriod);
        end
        
        function logPeriod = getLogPeriod(this)
            logPeriod = matstochsim(stochSimulation.prefix('GetLogPeriod'), this.objectHandle);
        end
        
        function setBaseFolder(this, baseFolder)
            matstochsim(stochSimulation.prefix('SetBaseFolder'), this.objectHandle, baseFolder);
        end
        
        function baseFolder = getBaseFolder(this)
            baseFolder = matstochsim(stochSimulation.prefix('GetBaseFolder'), this.objectHandle);
        end
        
        function setUniqueSubfolder(this, uniqueSubfolder)
            matstochsim(stochSimulation.prefix('SetUniqueSubfolder'), this.objectHandle, uniqueSubfolder);
        end
        
        function uniqueSubfolder = isUniqueSubfolder(this)
            uniqueSubfolder = matstochsim(stochSimulation.prefix('IsUniqueSubfolder'), this.objectHandle);
        end
        
    end
end