%stochSimulation Main class to create a stochastic simulation using matstochsim.
classdef stochSimulation < handle
    properties (SetAccess = private, GetAccess=private, Hidden = true)
        objectHandle; % Handle to the underlying C++ class instance
    end
    methods (Access = private, Hidden = true)
        function varargout = call(this, functionName, varargin)
            assert(this.check(), 'Invalid object.');
            [varargout{1:nargout}] = matstochsim(['Simulation::', functionName], this.objectHandle, varargin{:});
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
        %% Checks if valid
        function valid = check(this)
            valid = isvalid(this);
        end
        %% createState - creates a state in the simulation
        function state = createState(this, name, initialCondition)
			state = stochState(this.objectHandle, this, this.call('CreateState', name, initialCondition));
        end
        
        function state = createComposedState(this, name, initialCondition, varargin)
            if nargin < 4
                state = stochComposedState(this.objectHandle, this, this.call('CreateComposedState', name, initialCondition));
            else
                state = stochComposedState(this.objectHandle, this, this.call('CreateComposedState', name, initialCondition, varargin{1}));
            end
        end
        %% createReaction - creates a reaction in the simulation
        function reaction = createPropensityReaction(this, name, rateConstant)
			reaction = stochPropensityReaction(this.objectHandle, this, this.call('CreatePropensityReaction', name, rateConstant));
        end
        function reaction = createDelayReaction(this, name, state, delay)
            if ~ischar(state)
                state = state.getStateHandle();
            end
			reaction = stochDelayReaction(this.objectHandle, this, this.call('CreateDelayReaction', name, state, delay));
        end
        function reaction = createTimerReaction(this, name, fireTime)
			reaction = stochTimerReaction(this.objectHandle, this, this.call('CreateTimerReaction', name, fireTime));
        end
        
        %% run - Executes the simulation for the given runtime
        function varargout = run(this, runtime)
			[varargout{1:nargout}] = this.call('Run', runtime);
        end
        
        %% Configure save settings
        function setLogPeriod(this, logPeriod)
            this.call('SetLogPeriod', logPeriod);
        end
        
        function logPeriod = getLogPeriod(this)
            logPeriod = this.call('GetLogPeriod');
        end
        
        function setBaseFolder(this, baseFolder)
            this.call('SetBaseFolder', baseFolder);
        end
        
        function baseFolder = getBaseFolder(this)
            baseFolder = this.call('GetBaseFolder');
        end
        
        function setUniqueSubfolder(this, uniqueSubfolder)
            this.call('SetUniqueSubfolder', uniqueSubfolder);
        end
        
        function uniqueSubfolder = isUniqueSubfolder(this)
            uniqueSubfolder = this.call('IsUniqueSubfolder');
        end
        
        function setLogConsole(this, logConsole)
            this.call('SetLogConsole', logConsole);
        end
        
        function logConsole = isLogConsole(this)
            logConsole = this.call('IsLogConsole');
        end
        
    end
end