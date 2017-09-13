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

%% Create the simulation from the parse tree
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
    def = parseTree.reactions{i};
    % Fuse multiply defined LHS states by simply adding up their
    % stochiometry.
    [reactants,idx_unique,idx] = unique(def.lhs_states);
    reactants_modified = def.lhs_modifiers(idx_unique);
    reactants_stochiometries = zeros(size(reactants));
    for k=1:length(idx)
        if def.lhs_modifiers(k) ~= reactants_modified(idx(k))
            error('stochsim:inconsistentReactants', 'State %s cannot take part in the reaction both as a modifier/transformee and as a normal reactant.', def.lhs_states{k});
        end
        reactants_stochiometries(idx(k)) = reactants_stochiometries(idx(k)) + def.lhs_stochiometries(k);
    end
    % Fuse multiply defined RHS states by simply adding up their
    % stochiometry.
    [products,idx_unique,idx] = unique(def.rhs_states);
    products_modified = def.rhs_modifiers(idx_unique);
    products_stochiometries = zeros(size(products));
    for k=1:length(idx)
        if def.rhs_modifiers(k) ~= products_modified(idx(k))
            error('stochsim:inconsistentProducts', 'State %s cannot take part in the reaction both as a modifier/transformee and as a normal product.', def.rhs_states{k});
        end
        products_stochiometries(idx(k)) = products_stochiometries(idx(k)) + def.rhs_stochiometries(k);
    end

    % remove LHS states with modifier flags which are also RHS states with
    % modifier flags, because these states are transformees.
    for k=1:length(products)
        if ~products_modified(k)
            continue;
        end
        idx = find(cellfun(@(x)strcmp(x,products{k}), reactants), 1);
        if isempty(idx)
            error('stochsim:invalidTransformee', 'State %s appears on the RHS of the reaction as a transformee, but does not appear on the LHS.', products{k});
        end
        if ~reactants_modified(idx)
            error('stochsim:invalidTransformee', 'State %s appears on the RHS of the reaction as a transformee, but as a normal reactant on the LHS.', products{k});
        end
        if products_stochiometries(k) > reactants_stochiometries(idx)
            error('stochsim:invalidTransformee', 'State %s appears on the RHS of the reaction as a transformee with a higher stochiometry than as it appears on the LHS.', products{k});
        end
        reactants_stochiometries(idx) = reactants_stochiometries(idx) - products_stochiometries(k);
    end

    % remove now "empty" states on LHS
    idx = reactants_stochiometries==0;
    reactants_stochiometries(idx)=[];
    reactants_modified(idx)=[];
    reactants(idx)=[];
    
    if isfield(def, 'rateConstant')
        reaction = sim.createPropensityReaction(def.name, def.rateConstant);
        for k=1:length(reactants)
            if reactants_modified(k)
                reaction.addModifier(reactants{k}, reactants_stochiometries(k));
            else
                reaction.addReactant(reactants{k}, reactants_stochiometries(k));
            end
        end
        for k=1:length(products)
            if products_modified(k)
                reaction.addTransformee(products{k}, products_stochiometries(k));
            else
                reaction.addProduct(products{k}, products_stochiometries(k));
            end
        end
    else
        if any(reactants_modified)
            error('stochsim:modifiersNotAllowd', 'Modifiers are not allowed in delay reactions.');
        end
        if any(products_modified)
            error('stochsim:transformeesNotAllowd', 'Transformees are not allowed in delay reactions.');
        end
        if length(reactants)~=1
            error('stochsim:exactlyOneReactant', 'There must be exactly one reactant in delay reactions.');
        end
        if reactants_stochiometries(1)~=1
            error('stochsim:exactlyOneReactant', 'Stochiometry of the only reactant in a delay reaction must be one.');
        end
        
        reaction = sim.createDelayReaction(def.name, reactants{1}, def.delay);
        for k=1:length(products)
            reaction.addProduct(products{k}, products_stochiometries(k));
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
function parseTree = setPropensityReaction(parseTree, name, lhs_states, lhs_stochiometries, lhs_modifiers, rhs_states, rhs_stochiometries, rhs_modifiers, rateConstant)
reaction = struct();
reaction.name = name;
reaction.lhs_states = lhs_states;
reaction.lhs_stochiometries = lhs_stochiometries;
reaction.lhs_modifiers = lhs_modifiers;
reaction.rhs_states = rhs_states;
reaction.rhs_stochiometries = rhs_stochiometries;
reaction.rhs_modifiers = rhs_modifiers;
reaction.rateConstant = rateConstant;

parseTree.reactions{end+1} = reaction;
end

function parseTree = setDelayReaction(parseTree, name, lhs_state, rhs_states, rhs_stochiometries, rhs_modifiers, delay)
reaction = struct();
reaction.name = name;
reaction.lhs_states = {lhs_state};
reaction.lhs_stochiometries = 1;
reaction.lhs_modifiers = false;
reaction.rhs_states = rhs_states;
reaction.rhs_stochiometries = rhs_stochiometries;
reaction.rhs_modifiers = rhs_modifiers;
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
[lhs_states, lhs_stochiometries, lhs_modifiers] = parseReactants(parseTree,lhs);
[rhs_states, rhs_stochiometries, rhs_modifiers] = parseReactants(parseTree,rhs);

% get rate constant
if ~isempty(strfind(rateStr, 'delay:'))
    %% Create a delay reaction
    if length(lhs_states) ~= 1
        error('stochsim:delayReactionRequiresOneReactant', 'A delay reaction must have exactly one reactant (but can have arbitrary many products)!');
    end
    if lhs_stochiometries(1) ~= 1
        error('stochsim:delayReactionRequiresReactantStochiometryOne', 'The stochiometry of the reactant of a delay reaction must be one!');
    end
    if lhs_modifiers(1)
        error('stochsim:delayReactionRequiresReactantNotModified', 'The reactant of a delay reaction must not be modified ("[]" not allowed)!');
    end
    % find out length of delay
    idx = strfind(rateStr, 'delay:');
    if idx ~= 1
        error('stochsim:delayReactionMustOnlyContainDelay', 'After the comma in a delayed reaction and before the "delay:" identifier, nothing else must occur!');
    end
    rateStr = strtrim(rateStr(7:end));
    delay = getValueOfExpression(parseTree, rateStr);
    % Create states which occur in reaction.
    parseTree = setState(parseTree, lhs_states{1}, true);
    for i=1:length(rhs_states)
        parseTree = setState(parseTree, rhs_states{i}, false);
    end
    
    % Create reaction
    parseTree = setDelayReaction(parseTree, reactionStr, lhs_states{1}, rhs_states, rhs_stochiometries, rhs_modifiers, delay);
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
        lhs_states, lhs_stochiometries, lhs_modifiers,...
        rhs_states, rhs_stochiometries, rhs_modifiers,...
        rateConstant);
end
end
%% evaluate value of an expression
function value = getValueOfExpression(parseTree, expression) %#ok<INUSL>
extExpression = regexprep(expression, '([a-zA-Z_]\w*)', 'getParameter(parseTree, ''$1'')');
try
    value = eval(extExpression);
catch e
    ME = MException('stochsim:parseError', 'Could not parse value of expression "%s"!', expression);
    ME=ME.addCause(e);
    throw(ME);
end
end
%% parseReactants
function [speciesNames, stochiometries, modifiers] = parseReactants(parseTree, reactantsStr)

if isempty(reactantsStr)
    speciesNames = cell(1, 0);
    stochiometries = ones(1, 0);
    modifiers = false(1, 0);
    return;
end
elements = strsplit(reactantsStr, '+');
speciesNames = cell(1, length(elements));
stochiometries = ones(1, length(elements));
modifiers = false(1, length(elements));
for i=1:length(elements)
    element=strtrim(elements{i});
    [tokens,~] = regexp(element,'(\<\w+\>)?\s*\*?\s*(\<[a-zA-Z]\w*\>)(\[\])?','tokens','match');
    if ~isempty(tokens{1}{1})
        stochiometries(i) = getValueOfExpression(parseTree, tokens{1}{1});
    end
    speciesNames{i} = tokens{1}{2};
    if ~isempty(tokens{1}{3})
        modifiers(i) = true;
    end
end
end