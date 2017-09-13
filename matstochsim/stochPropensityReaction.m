classdef stochPropensityReaction < handle
% A propensity based ("normal") reaction. The
% propensity ("rate") of the reaction is calculated using mass
% action kinetics and can be considered to represent the
% average number of times the reaction fires during a certain
% time period.
% Can only be constructed via a stochSimulation object.
    properties (SetAccess = private, GetAccess=private, Hidden = true)
        simulationHandle; % Handle to simulation object to which this state belongs. Used for checking if the object handle is still valid.
        reactionHandle; % Handle to the state this object represents
    end
    methods (Access = private, Hidden = true)
        function varargout = call(this, functionName, varargin)
            assert(this.check(), 'Invalid object.');
            [varargout{1:nargout}] = matstochsim(['PropensityReaction::', functionName], this.simulationHandle.objectHandle, this.reactionHandle, varargin{:});
        end
    end
    methods(Access = ?stochSimulation)
        %% Constructor
        function this = stochPropensityReaction(simulationHandle, reactionHandle)
            % Create a new stochsim State
            % Should only be called from stochSimulation class
            this.simulationHandle = simulationHandle;
            this.reactionHandle = reactionHandle;
            assert(this.check(), 'Invalid object.');
        end
    end
    properties(Dependent)
        % Unique name of the reaction.
        name;
        % The rate constant of the reaction used to calculate the
        % propensity ("rate") of the reaction using mass action kinetics.
        rateConstant;
    end
    methods
        %% Validity
        function valid = check(this)
            % Checks if this reaction and the simulation are in a valid state.
            % Usage:
            %   valid = check(this)
            % Returns:
            %   valid - True if this reaction and the simulation are in a
            %           valid state and can be run/edited, false otherwise.
            valid = isvalid(this) && isvalid(this.simulationHandle);
        end
        
        %% Getters and setters for properties
        function name = get.name(this)
            % Returns the unique name of this reaction.
            name = this.call('Name');
        end
        function rateConstant = get.rateConstant(this)
            rateConstant = this.call('GetRateConstant');
        end
        function set.rateConstant(this, rateConstant)
            this.call('SetRateConstant', rateConstant);
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
                state = state.getStateHandle();
            end
            if ~exist('stochiometry', 'var') || isempty(stochiometry)
                stochiometry = 1;
            end
            this.call('AddProduct', state, stochiometry);
        end
        
        function addReactant(this, state, stochiometry)
            % Adds a species as a product of the reaction. When the
            % reaction fires, its concentration is increased according to
            % its stochiometry.
            % Usage:
            %   addProduct(this, state)
            %   addProduct(this, state, stochiometry)
            % Parameters:
            %   state           ...name or reference to state to add.
            %   stochiometry    ...number of molecules assumed to be
            %                      produced by the reaction. Default = 1.
            if ~ischar(state)
                state = state.getStateHandle();
            end
            if ~exist('stochiometry', 'var') || isempty(stochiometry)
                stochiometry = 1;
            end
            this.call('AddReactant', state, stochiometry);
        end
        
        function addModifier(this, state, stochiometry)
            % Adds a species as a modifier of the reaction. Different to a
            % reactant or product of a reaction, the molecular number of a
            % modifier is neither increased nor decreased when the reaction
            % fires. However, a modifier still modifies the reaction rate
            % according to its stochiometry following mass action kinetics.
            % It can, thus, e.g. represent an enzyme which is neither
            % produced or consumed by the reaction but still has an
            % influence on the rate.
            % Usage:
            %   addModifier(this, state)
            %   addModifier(this, state, stochiometry)
            % Parameters:
            %   state           ...name or reference to state to add.
            %   stochiometry    ...number of molecules taking part in the
            %                      reaction. Default = 1. 
            if ~ischar(state)
                state = state.getStateHandle();
            end
            if ~exist('stochiometry', 'var') || isempty(stochiometry)
                stochiometry = 1;
            end
            this.call('AddModifier', state, stochiometry);
        end
        function addTransformee(this, state, stochiometry)
            % Adds a species as a transformee of the reaction. A
            % transformee is similar to a modifier of a reaction, that is,
            % it is neither produced or consumed when the reaction fires
            % but still influences the reaction rate following mass action
            % kinetics. However, different to a modifier, the molecules of
            % a transformee taking part of the reaction "change" when the
            % reaction fires, that is, they get transformed in some sense.
            % A transformation can mean in principle various things, but
            % currently the only implemented molecule transformation is a
            % simple counting of how often the molecule was transformed.
            % This is e.g. useful when simply wanting to count how often a
            % molecule took part in a reaction.
            % Usage:
            %   addTransformee(this, state)
            %   addTransformee(this, state, stochiometry)
            % Parameters:
            %   state           ...name or reference to state to add.
            %   stochiometry    ...number of molecules taking part and
            %                      being transformed by the reaction.
            %                      Default = 1.  
            if ~ischar(state)
                state = state.getStateHandle();
            end
            if ~exist('stochiometry', 'var') || isempty(stochiometry)
                stochiometry = 1;
            end
            this.call('AddTransformee', state, stochiometry);
        end
    end
end