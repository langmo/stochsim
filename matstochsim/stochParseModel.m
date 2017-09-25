function sim = stochParseModel(fileName, externalParameters, sim)
% parseModel Parses the model saved in fileName (usually a CMDL file) and
% creates a simulation object in stochsim which can be run.
% Usage:
%   sim = stochParseModel(fileName)
%   sim = stochParseModel(fileName, externalParameters)
%   sim = stochParseModel(fileName, externalParameters, sim)
% Parameters:
%   fileName            Name of the file containing the model, e.g.
%                       'model.cmdl'.
%   externalParameters  Structure of additional parameters for the model.
%                       If a parameter is defined both in the model and in
%                       this structure, the definition in this structure
%                       has precedence, allowing e.g. to easily scan
%                       parameter regions. Parameters are defined as
%                       externalParameters.myParameter = myValue; Can be
%                       empty ("[]") to not predefine any parameters.
%   sim                 An already defined stochsim simulation object. If
%                       provided, this object is extended by the states and
%                       reactions defined in the model file. No states or
%                       reactions with the same name must be defined
%                       in the simulation object already and in the model
%                       file. If not provided or an empty array ("[]") is
%                       provided, a new simulation object is created.

%% Setup
parseTree = struct();
% Struct p holds all declared parameters
parseTree.p = struct();
% hold all states which should be initialized
parseTree.states = cell(1,0);
% hold all reactions which should be initialized
parseTree.reactions = cell(1,0);
% externally provided parameters
if nargin < 2 || isempty(externalParameters)
    parseTree.p_ext = struct();
else
    parseTree.p_ext = externalParameters;   
end
% externally provided, already existing simulation object
if nargin < 3 || isempty(sim)
    sim = [];
else
    sim.check();
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
        ME = MException('stochsim:modelParseError', 'Error while parsing model %s, line %g:\n\t%s', fileName, lineID, line);
        ME = ME.addCause(e);
        throw(ME);
    end
    %% Read in next line
    line = fgetl(fid);
    lineID = lineID+1;
end
fclose(fid);

%% Create and initialize the simulation object
try
    sim = createSimulation(sim, parseTree);
catch e
    ME = MException('stochsim:modelCreationError', 'Error while creating the model defined in %s.', fileName);
    ME = ME.addCause(e);
    throw(ME);
end
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
% ignore empty lines
if isempty(line)
    return;
end
% all other lines must be commands and thus be finished by a ;
if ~strcmp(line(end), ';')
    error('stochsim:mustEndWithSemicolon', 'Line does not end with a semicolon!');
end
% remove semicolon
line = strtrim(line(1:end-1));
% ignore empty commands
if isempty(line)
    return;
end
% check if assignment
idx = strfind(line, '=');
if length(idx) > 1
    error('stochsim:multipleEqualSigns', 'Line does contain more than one equal sign ("=")!');
elseif length(idx) == 1
    parseTree = parseAssignment(line, parseTree);
    return;
end
% check if invalid reaction
idx = strfind(line, '<->');
if ~isempty(idx)
    error('stochsim:bidirectionalReaction', 'Bidirectional reactions ("<->") are not allowed. Replace by two unidirectional ones ("->")!');
end
idx = strfind(line, '<-');
if ~isempty(idx)
    error('stochsim:backwardReaction', 'Backward reactions ("<-") are not allowed. Replace by a forward directional one ("->") by switching the left and right hand side!');
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
function sim = createSimulation(sim, parseTree)
% Create simulation object
if isempty(sim)
    sim = stochSimulation();
end

% Get all parameters. Parameters which are provided by the user have
% precedence over the parameters defined in the model file.
parameters = parseTree.p;
parameterNames = fieldnames(parseTree.p_ext);
for i=1:length(parameterNames)
    parameters.(parameterNames{i}) = parseTree.p_ext.(parameterNames{i});
end
% Get all parameters which are not initial conditions of states. These
% parameters must be filled into the lazy evaluated rate equations and
% similar, while the parameters corresponding to ICs of states mustn't.
parameterNames = setdiff(fieldnames(parameters), cellfun(@(x)x.name, parseTree.states, 'UniformOutput', false));
onlyParameters = struct();
for i=1:length(parameterNames)
    onlyParameters.(parameterNames{i}) = parameters.(parameterNames{i});
end

% create states
for i=1:length(parseTree.states)
    name = parseTree.states{i}.name;
    if ~isfield(parameters, name)
        error('stochsim:ICnotdefined', 'Initial condition for state %g not defined. Define initial conditions like normal variable assignments, e.g. "%s=10;" to assign an initial condition of 10 molecules to %s.', name, name, name);
    end
    IC = parameters.(name);
    if parseTree.states{i}.composed
        sim.createComposedState(name, IC);
    else
        sim.createState(name, IC);
    end
end


% Create reactions
for i=1:length(parseTree.reactions)
    def = parseTree.reactions{i};
    if isempty(def.reactionName)
        def.reactionName = sprintf('reaction_%g', i);
    end
    % Fuse multiply defined LHS states by simply adding up their
    % stochiometry.
    [reactantsNames,idx_unique,idx] = unique(cellfun(@(x)x.name, def.lhsComponents, 'UniformOutput', false));
    reactantsModified = arrayfun(@(x)def.lhsComponents{x}.modifier, idx_unique);
    reactantsStochiometries = zeros(size(reactantsNames));
    for k=1:length(idx)
        if def.lhsComponents{k}.modifier ~= reactantsModified(idx(k))
            error('stochsim:inconsistentReactants', 'State %s cannot take part in the reaction both as a modifier/transformee and as a normal reactant.', reactantsNames{idx(k)});
        end
        reactantsStochiometries(idx(k)) = reactantsStochiometries(idx(k)) + evaluateFormula(def.lhsComponents{k}.stochiometry, parameters);
    end
    % Fuse multiply defined RHS states by simply adding up their
    % stochiometry.
    [productsNames,idx_unique,idx] = unique(cellfun(@(x)x.name, def.rhsComponents, 'UniformOutput', false));
    productsModified = arrayfun(@(x)def.rhsComponents{x}.modifier, idx_unique);
    productsStochiometries = zeros(size(productsNames));
    for k=1:length(idx)
        if def.rhsComponents{k}.modifier ~= productsModified(idx(k))
            error('stochsim:inconsistentProducts', 'State %s cannot take part in the reaction both as a modifier/transformee and as a normal product.', def.rhs_states{k});
        end
        productsStochiometries(idx(k)) = productsStochiometries(idx(k)) + evaluateFormula(def.rhsComponents{k}.stochiometry, parameters);
    end

    % remove LHS states with modifier flags which are also RHS states with
    % modifier flags, because these states are transformees.
    for k=1:length(productsNames)
        if ~productsModified(k)
            continue;
        end
        idx = find(cellfun(@(x)strcmp(x,productsNames{k}), reactantsNames), 1);
        if isempty(idx)
            error('stochsim:invalidTransformee', 'State %s appears on the RHS of the reaction as a transformee, but does not appear on the LHS.', productsNames{k});
        end
        if ~reactantsModified(idx)
            error('stochsim:invalidTransformee', 'State %s appears on the RHS of the reaction as a transformee, but as a normal reactant on the LHS.', productsNames{k});
        end
        if productsStochiometries(k) > reactantsStochiometries(idx)
            error('stochsim:invalidTransformee', 'State %s appears on the RHS of the reaction as a transformee with a higher stochiometry than as it appears on the LHS.', productsNames{k});
        end
        reactantsStochiometries(idx) = reactantsStochiometries(idx) - productsStochiometries(k);
    end

    % remove now "empty" states on LHS
    idx = reactantsStochiometries==0;
    reactantsStochiometries(idx)=[];
    reactantsModified(idx)=[];
    reactantsNames(idx)=[];
    
    if isfield(def, 'rate')
        reaction = sim.createPropensityReaction(def.reactionName, simplifyFormula(def.rate, onlyParameters));
        for k=1:length(reactantsNames)
            if reactantsModified(k)
                reaction.addModifier(reactantsNames{k}, reactantsStochiometries(k));
            else
                reaction.addReactant(reactantsNames{k}, reactantsStochiometries(k));
            end
        end
        for k=1:length(productsNames)
            if productsModified(k)
                reaction.addTransformee(productsNames{k}, productsStochiometries(k));
            else
                reaction.addProduct(productsNames{k}, productsStochiometries(k));
            end
        end
    elseif isfield(def, 'delay')
        if any(reactantsModified)
            error('stochsim:modifiersNotAllowd', 'Modifiers are not allowed in delay reactions.');
        end
        if any(productsModified)
            error('stochsim:transformeesNotAllowd', 'Transformees are not allowed in delay reactions.');
        end
        if length(reactantsNames)~=1
            error('stochsim:exactlyOneReactant', 'There must be exactly one reactant in delay reactions.');
        end
        if reactantsStochiometries(1)~=1
            error('stochsim:exactlyOneReactant', 'Stochiometry of the only reactant in a delay reaction must be one.');
        end
        
        reaction = sim.createDelayReaction(def.reactionName, reactantsNames{1}, evaluateFormula(def.delay, parameters));
        for k=1:length(productsNames)
            reaction.addProduct(productsNames{k}, productsStochiometries(k));
        end
    else
        error('stochsim:invalidReaction', 'Neither a rate constant or equation, nor a delay was specified for the reaction.');
    end
end
end

%% Parse an assignment
function parseTree = parseAssignment(assignment, parseTree)
% An assignment assignes either a value or a formula to a variable or
% state (for the latter, as the initial condition).

idx = strfind(assignment, '=');
nameStr = assignment(1:idx-1);
formulaStr = assignment(idx+1:end);
% Get name
nameExpr = ...
    '\s*(?<parameterName>\<[a-zA-Z_]\w*\>)\s*';
[match, namedTokens] = regexp(nameStr, nameExpr, 'match', 'names', 'once');
if ~strcmp(nameStr, match)
    error('stochsim:invalidVariableName', 'The string %s is not a valid variable or state name. Variable or state names must begin with a lower or upper case character ([a-z] or [A-Z]), or an underscore ("_"). After that, they can continue with upper or lower case characters ([a-z] or [A-Z]), underscores ("_") or numbers ([0-9])!', strtrim(nameStr));
end
name = namedTokens.parameterName;
% get value or formula for value
formula = parseFormula(formulaStr, parseTree);
% store parameter in parse tree
parseTree.p.(name) = formula;
end
%% Parse formula
function value = parseFormula(formula, parseTree, forceEvaluation)
% A formula is a mathematical expression which can represent a variable
% value, an initial condition for a state, a reaction rate constant, or
% similar. There are two kinds of formulas: formulas which are eager, and
% formulas which are lazy evaluated. Lazy evaluated formulas must begin
% with an "[" and end with an "]". They are only evaluated in the moment
% when the value they are represented is needed, and may e.g. be used as
% custom reaction rate equations which do not follow mass action kinetics.
% On the other hand, eager evaluated formulas do neither start with an "["
% or end with an "]", and are immediately evaluated when found.

if nargin < 3
    forceEvaluation = false;
end

formulaExpr = ...
    '\s*(?<modifier>\[)?+\s*(?<formula>.*\S)\s*(?(modifier)\])\s*';
[match, namedTokens] = regexp(formula, formulaExpr, 'match', 'names', 'once');
if ~strcmp(formula, match)
    error('stochsim:invalidFormula', 'The string %s is not a valid formula. Valid formulas might either be defined as lazy evaluation, then having to start with an "[" and end with an "]", or as eager evaluation, then neither starting with an "[" or ending with an "]".', formula);
end
formula = namedTokens.formula;

if isempty(namedTokens.modifier) || forceEvaluation
    % eager evaluation. Let's evaluate it now!
    parameters = parseTree.p;
    parameterNames = fieldnames(parseTree.p_ext);
    for i=1:length(parameterNames)
        parameters.(parameterNames{i}) = parseTree.p_ext.(parameterNames{i});
    end
    value = evaluateFormula(formula, parameters);
else
    % lazy evaluation. Just pass formula as string
    value = formula;
end
end

%% Evaluate formula
function value = evaluateFormula(formula, parameters)
% If formula is already evaluated, return simply its value
if ~ischar(formula)
    value = formula;
    return;
end
qualifiedFormula = formula;

% find all parts of a formula which might be a parameter name.
nameExpr = ...
    '(\<[a-zA-Z_]\w*\>)';
matches = unique(regexp(qualifiedFormula, nameExpr, 'match'));
% replace parts of the formula which actually are parameter names by their
% fully qualified names.
parameterValues = struct();
for i=1:length(matches)
    if isfield(parameters, matches{i})
        parameterValues.(matches{i}) = evaluateFormula(parameters.(matches{i}), parameters);
        qualifiedFormula = strrep(qualifiedFormula, matches{i}, sprintf('parameterValues.%s', matches{i}));
    end
end

% Evaluate formula
try
    value = eval(qualifiedFormula);
catch e
    ME = MException('stochsim:parseError', 'Could not parse value of eager evaluation formula "%s"!', formula);
    ME=ME.addCause(e);
    throw(ME);
end
    
end

%% Simplify formula
function formula = simplifyFormula(formula, parameters)

% If formula is already evaluated, return simply its value
if ~ischar(formula)    
    return;
end

% find all parts of a formula which might be a parameter name.
nameExpr = ...
    '(\<[a-zA-Z_]\w*\>)';
replacedString = true;
while replacedString
    replacedString = false;
    matches = unique(regexp(formula, nameExpr, 'match'));
    for i=1:length(matches)
        if isfield(parameters, matches{i})
            if ischar(parameters.(matches{i}))
                % if the parameter is also late evaluated, simply combine.
                formula = strrep(formula, matches{i}, ['(', parameters.(matches{i}), ')']);
                replacedString = true;
            else
                % Otherwise, print the parameter value into the formula.
                formula = strrep(formula, matches{i}, mat2str(parameters.(matches{i})));
            end

        end
    end
end
end

%% Parse reaction
function parseTree = parseReaction(reactionStr, parseTree)
% A reaction specifies how the reactants interact to form the products. Its
% main part is the reaction definition, consisting of a left hand side
% (LHS) and a right hand side (RHS) separated by an arrow ("->"). This main
% part can be optionally preceeded by a reaction name (e.g. "myreaction")
% followed by a comma. After the reaction name, there must be at least one
% reaction property specifier separated by a comma. This specifier can be
% either an eager evaluated formula, then indicating the reaction rate
% constant. It can be a lazy evaluated formula, then indicating the
% reaction rate equation. Furthermore, it can be an eager or lazy evaluated
% formula preceeded by the string "delay:", then indicating that the
% reaction is a fixed delay firing reaction.
% Both the LHS and the RHS consists of zero or more reaction components, separated
% by a plus ("+"). A reaction component consists of a state name (e.g. "A",
% "_mystate123"), which can be optionally preceeded by a formula defining
% its stochiometry (e.g. "1", "5", "3*myvarname"). If preceeded by a
% formula, the formula and the state name must be either separated by one
% or more blank characters (spaces, tabs), or by zero or more blank
% characters, a multiplication sign ("*"), and zero or more blank
% characters (together, e.g. "2 mystate", "2*mystate", "2 * mystate",
% "5*myvar* 2 mystate", or "5*myvar* 2   * mystate"). Optionally, the state
% name can be followed by an opening and closing square bracket ("[]",
% together e.g. "mystate[]"), indicating that the state acts as a modifier
% of the reaction. When on the LHS, this means that the state influences
% the reaction rate according to its stochiometry following mass action
% kinetics, but that its concentration does not decrease when the reaction
% fires (e.g. an enzyme catalyzing the reaction). On the RHS, a modifier
% indicates that the respective species concentration is not decreased, but
% that the respective molecules are modified. What this means is
% implementation dependent, but currently only a simple counter of how
% often a molecule was modified is implemented. Note that when a state
% appears as a modifier on the RHS, it must appear as a modifier also on
% the LHS with at least the same or a greater stochiometry.

% Everything which does not contain a comma or an arrow.
reactionPartExpr = [...
    '(?:([^,\-<\s]|(?<!<)\-(?!>)|<(?!\-))'...
    '([^,\-<]|(?<!<)\-(?!>)|<(?!\-))*'...
    '([^,\-<\s]|(?<!<)\-(?!>)|<(?!\-))'...
    '|[^,\-\s<]?)'];
% Only the formula, e.g. A -> B
reactionExpr = [...
    '\s*(?<reactionName>', reactionPartExpr,')?(?(reactionName)\s*,\s*)',...
    '(?<LHS>', reactionPartExpr, ')',...
    '\s*->\s*'...
    '(?<RHS>', reactionPartExpr, ')',...
    '\s*,\s*',...
    '(?<arguments>', reactionPartExpr, '(\s*,\s*)?)+\s*'];
% Name of a reaction
reactionNameExpr = ...
    '\s*(?<reactionName>\<[a-zA-Z_]\w*\>)\s*';
reactionPropertyExpr = ...
    '\s*(?<propertyName>\<[a-zA-Z_]\w*\>):(?<propertyValue>.*)\s*';

% Create structure to hold reaction information
reaction = struct();

% Split reaction into main components
[match, reactionComponents] = regexp(reactionStr, reactionExpr, 'match', 'names', 'once');
if ~strcmp(reactionStr, match)
    error('stochsim:notCorrectReactantStructure', 'The string %s does not have the right structure of a reaction!', reactionStr);
end

% Read in reaction name
if isempty(reactionComponents.reactionName)
    reaction.reactionName = [];
else
    [match, reactionName] = regexp(reactionComponents.reactionName, reactionNameExpr, 'match', 'names', 'once');
    if ~strcmp(reactionComponents.reactionName, match)
        error('stochsim:invalidReactionName', 'The string %s is not a valid name for a reaction!', reactionComponents.reactionName);
    end
    reaction.reactionName = reactionName.reactionName;
end

% Read in reaction properties, e.g. rate constant, rate equation, delay,
% ...
reactionPropertieStrs = strsplit(reactionComponents.arguments, ',');
for i=1:length(reactionPropertieStrs)
    reactionProperty = regexp(reactionPropertieStrs{i}, reactionPropertyExpr, 'names', 'once');
    if isempty(reactionProperty)
        reaction.rate = parseFormula(reactionPropertieStrs{i}, parseTree);
    else
        reaction.(reactionProperty.propertyName) = parseFormula(reactionProperty.propertyValue, parseTree);
    end
end

% Reactants and products
reaction.lhsComponents = parseReactionComponents(reactionComponents.LHS, parseTree);
reaction.rhsComponents = parseReactionComponents(reactionComponents.RHS, parseTree);
for i=1:length(reaction.lhsComponents)
    % check if state already exists
    stateIdx = find(cellfun(@(x)strcmp(x.name, reaction.lhsComponents{i}.name), parseTree.states), 1);
    if isempty(stateIdx)
        parseTree.states{end+1} = struct('name', reaction.lhsComponents{i}.name, 'composed', isfield(reaction, 'delay'));
    elseif isfield(reaction, 'delay')
        parseTree.states{stateIdx}.composed = true;
    end
end
for i=1:length(reaction.rhsComponents)
    % check if state already exists
    stateIdx = find(cellfun(@(x)strcmp(x.name, reaction.rhsComponents{i}.name), parseTree.states), 1);
    if isempty(stateIdx)
        parseTree.states{end+1} = struct('name', reaction.rhsComponents{i}.name, 'composed', reaction.rhsComponents{i}.modifier);
    elseif reaction.rhsComponents{i}.modifier
        parseTree.states{stateIdx}.composed = true;
    end
end

parseTree.reactions{end+1} = reaction;
end

%% parseReactants
function components = parseReactionComponents(reactionComponentsStr, parseTree)
if isempty(reactionComponentsStr)
    components = cell(1, 0);
    return;
end

% Name of a state
varNameExpr = ...
    '\<[a-zA-Z_]\w*\>';
reactionComponentExpr = ...
    ['\s*(?<stochiometry>\<\w+\>)?(?(stochiometry)\s*[\*\s]\s*)(?<state>', varNameExpr, ')(?<modifier>\[\])?\s*'];

reactionComponents = strsplit(reactionComponentsStr, '+');
components = cell(size(reactionComponents));
for i=1:length(reactionComponents)
    [match, reactionComponent] = regexp(reactionComponents{i}, reactionComponentExpr, 'match', 'names', 'once');
    if ~strcmp(reactionComponents{i}, match)
        error('stochsim:notCorrectReactionComponent', 'The string %s is not a valid reaction component definition!', reactionComponents{i});
    end
    components{i} = struct();
    if isempty(reactionComponent.stochiometry)
        components{i}.stochiometry = 1;
    else
        components{i}.stochiometry = parseFormula(reactionComponent.stochiometry, parseTree);
    end
    components{i}.name = reactionComponent.state;
    components{i}.modifier = ~isempty(reactionComponent.modifier);
end
end