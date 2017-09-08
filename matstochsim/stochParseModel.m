function sim = stochParseModel(fileName, externalParameters)
%parseModel Parses the model saved in fileName (usually a CMDL file) and
%creates a simulation object in stochsim which can be run.

%% Setup
parseTree = struct();
% Struct p holds all declared parameters
parseTree.p = struct();
% hold all states which should be initialized
parseTree.states = cell(1,0);
% hold all reactions which should be initialized
parseTree.reactions = cell(1,0);
% externally provided parameters
if exist('externalParameters', 'var') && isstruct(externalParameters)
    parseTree.p_ext = externalParameters;
else
    parseTree.p_ext = struct();
end

%% Parse model and fill the parse tree
fid = fopen(fileName);
line = fgetl(fid);
lineID = 1;
while ischar(line)
    %% parse line
    try
        parseTree = parseLine(line, parseTree);
    catch e
        ME = MException(e.identifier, '%s\n\tin file %s, line %g:\n\t%s', e.message, fileName, lineID, line);
        ME=ME.addCause(e);
        throw(ME);
    end
    %% Read in next line
    line = fgetl(fid);
    lineID = lineID+1;
end
fclose(fid);

%% Create and initialize the simulation object
sim = createSimulation(parseTree);
end

%% Parse a line in the CMDL file
function parseTree = parseLine(line, parseTree)
% remove comments
idx = strfind(line, '//');
if ~isempty(idx)
    line = line(1:idx(1)-1);
end
% remove whitespace characters
line = strtrim(line);
if isempty(line)
    return;
end
if ~strcmp(line(end), ';')
    error('stochsim:mustEndWithSemicolon', 'Line does not end with a semicolon!');
end
% ignore empty commands
line = strtrim(line(1:end-1));
if isempty(line)
    return;
end
% check if variable
idx = strfind(line, '=');
if length(idx) > 1
    error('stochsim:multipleEqualSigns', 'Line does contain more than one equal sign ("=")!');
elseif length(idx) == 1
    parseTree = parseParameter(line, parseTree);
    return;
end
% check if reaction
idx = strfind(line, '->');
if length(idx) > 1
    error('stochsim:multipleReactionSigns', 'Line does contain more than one reaction sign ("->")!');
elseif length(idx) == 1
    parseTree = parseReaction(line, parseTree);
    return;
end
error('stochsim:notAssignmentNotReaction', 'Line is neither an assignment, nor a reaction!');
end

%% Create the simulatio from the parse tree
function sim = createSimulation(parseTree)
% Create simulation object
sim = stochSimulation();
% create states
for i=1:length(parseTree.states)
    IC = getParameter(parseTree, parseTree.states{i}.name);
    if parseTree.states{i}.composed
        sim.createComposedState(parseTree.states{i}.name, IC);
    else
        sim.createState(parseTree.states{i}.name, IC);
    end
end
% Create reactions
for i=1:length(parseTree.reactions)
    if isfield(parseTree.reactions{i}, 'rateConstant')
        reaction = sim.createPropensityReaction(parseTree.reactions{i}.name, parseTree.reactions{i}.rateConstant);
        for j=1:length(parseTree.reactions{i}.lhs_states)
            reaction.addReactant(parseTree.reactions{i}.lhs_states{j}, parseTree.reactions{i}.lhs_stochiometries(j));
        end
        for j=1:length(parseTree.reactions{i}.rhs_states)
            reaction.addProduct(parseTree.reactions{i}.rhs_states{j}, parseTree.reactions{i}.rhs_stochiometries(j));
        end
    else
        reaction = createDelayReaction(parseTree.reactions{i}.name, parseTree.reactions{i}.lhs_state, parseTree.reactions{i}.delay);
        for j=1:length(parseTree.reactions{i}.rhs_states)
            reaction.addProduct(parseTree.reactions{i}.rhs_states{j}, parseTree.reactions{i}.rhs_stochiometries(j));
        end
    end
end
end

%% Get a parameter value
function value = getParameter(parseTree, name)
% getParameter - Returns value of parameter.
% Returns the value of the parameter with the given name, or throws an
% error if no such parameter is defined.
% First, the externally provided parameters are searched, and only if not
% defined in there the parameter values already found in the model file are
% used.
%
% value = getParameter(parseTree, name)

if isfield(parseTree.p_ext, name)
    value = parseTree.p_ext.(name);
elseif isfield(parseTree.p, name)
    value = parseTree.p.(name);
else
    error('stochsim:parameterUndefined', 'Parameter %s not defined. Set a parameter e.g. by "%s = 0;"!', name, name);
end
end
%% Set a parameter value
function parseTree = setParameter(parseTree, name, value)
% setParameter - Sets the value of parameter and returns the new version of the parse tree.
%
% parseTree = setParameter(parseTree, name, value)
parseTree.p.(name) = value;
end

%% Set a species
function parseTree = setState(parseTree, name, composed)
% check if state already exists
stateIdx = find(cellfun(@(x)strcmp(x.name, name), parseTree.states), 1);
if isempty(stateIdx)
    parseTree.states{end+1} = struct('name', name, 'composed', composed);
elseif composed
    parseTree.states{stateIdx}.composed = composed;
end
end

%% Set a reaction
function parseTree = setPropensityReaction(parseTree, name, lhs_states, lhs_stochiometries, rhs_states, rhs_stochiometries, rateConstant)
reaction = struct();
reaction.name = name;
reaction.lhs_states = lhs_states;
reaction.lhs_stochiometries = lhs_stochiometries;
reaction.rhs_states = rhs_states;
reaction.rhs_stochiometries = rhs_stochiometries;
reaction.rateConstant = rateConstant;

parseTree.reactions{end+1} = reaction;
end

function parseTree = setDelayReaction(parseTree, name, lhs_state, rhs_states, rhs_stochiometries, delay)
reaction = struct();
reaction.name = name;
reaction.lhs_state = lhs_state;
reaction.rhs_states = rhs_states;
reaction.rhs_stochiometries = rhs_stochiometries;
reaction.delay = delay;

parseTree.reactions{end+1} = reaction;
end

%% Parse parameter
function parseTree = parseParameter(assignment, parseTree)
idx = strfind(assignment, '=');
name = strtrim(assignment(1:idx-1));
valueStr = strtrim(assignment(idx+1:end));
% check name
invalidIdx = regexp(name, '\W');
if ~isempty(invalidIdx)
    error('stochsim:invalidVariableName', 'Variable name "%s" is invalid since it contains the character ''%c''!', name, name(invalidIdx(1)));
end
% parse value
value = getValueOfExpression(parseTree, valueStr);
% set value
parseTree = setParameter(parseTree, name, value);
end

%% Parse reaction
function parseTree = parseReaction(reactionStr, parseTree)

idx = strfind(reactionStr, '->');
lhs = strtrim(reactionStr(1:idx-1));
rhs = strtrim(reactionStr(idx+2:end));
idx = strfind(rhs, ',');
if length(idx) > 1
    error('stochsim:multipleRateConstant', 'Reaction does contain more than one rate constant specification (separated by commas at RHS of "->")!');
elseif length(idx) < 1
    error('stochsim:missingRateConstant', 'Reaction does not contain rate specifier (separated by comma at RHS of "->")!');
end
rateStr = strtrim(rhs(idx+1:end));
rhs = strtrim(rhs(1:idx-1));

% Reactants and products
[lhs_states, lhs_stochiometries] = parseReactants(lhs);
[rhs_states, rhs_stochiometries] = parseReactants(rhs);

% get rate constant
if ~isempty(strfind(rateStr, 'delay:'))
    %% Create a delay reaction
    if length(lhs_states) ~= 1
        error('stochsim:delayReactionRequiresOneReactant', 'A delay reaction must have exactly one reactant (but can have arbitrary many products)!');
    end
    if lhs_stochiometries(1) ~= 1
        error('stochsim:delayReactionRequiresReactantStochiometryOne', 'The stochiometry of the reactant of a delay reaction must be one!');
    end
    % find out length of delay
    idx = strfind(rateStr, 'delay:');
    if idx ~= 1
        error('stochsim:delayReactionMustOnlyContainDelay', 'After the comma in a delayed reaction and before the "delay:" identifier, nothing else must occur!');
    end
    rateStr = strtrim(rateStr(7));
    delay = getValueOfExpression(parseTree, rateStr);
    % Create states which occur in reaction.
    parseTree = setState(parseTree, lhs_states{1}, true);
    for i=1:length(rhs_states)
        parseTree = setState(parseTree, rhs_states{i}, false);
    end
    
    % Create reaction
    parseTree = setDelayReaction(parseTree, reactionStr, lhs_states{1}, rhs_states, rhs_stochiometries, delay);
else
    %% Create a propensity reaction
    % read in reaction rate constant
    rateConstant = getValueOfExpression(parseTree, rateStr);
    % Create states which occur in reaction.
    for i=1:length(lhs_states)
        parseTree = setState(parseTree, lhs_states{i}, false);
    end
    for i=1:length(rhs_states)
        parseTree = setState(parseTree, rhs_states{i}, false);
    end
    
    % Create reaction
    parseTree = setPropensityReaction(parseTree, reactionStr, ...
        lhs_states, lhs_stochiometries, ...
        rhs_states, rhs_stochiometries, ...
        rateConstant);
end
end
%% evaluate value of an expression
function value = getValueOfExpression(parseTree, expression) %#ok<INUSL>
extExpression = regexprep(expression, '([a-zA-Z_]\w*)', 'getParameter(parseTree, ''$1'')');
try
    value = eval(extExpression);
catch e
    error('stochsim:parseError', 'Could not parse value of expression "%s"!', extExpression);
end
end
%% parseReactants
function [speciesNames, stochiometries] = parseReactants(reactantsStr)

if isempty(reactantsStr)
    speciesNames = cell(1, 0);
    stochiometries = ones(1, 0);
    return;
end
elements = strsplit(reactantsStr, '+');
speciesNames = cell(1, length(elements));
stochiometries = ones(1, length(elements));
for i=1:length(elements)
    element=strtrim(elements{i});
    [tokens,~] = regexp(element,'(\d)?\s*\*?\s*([a-zA-Z]\w*)','tokens','match');
    if ~isempty(tokens{1}{1})
        stochiometries(i) = str2double(tokens{1}{1});
    end
    speciesNames{i} = tokens{1}{2};
end
end