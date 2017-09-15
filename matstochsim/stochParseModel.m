function sim = stochParseModel(fileName, externalParameters)
%parseModel Parses the model saved in fileName (usually a CMDL file) and
%creates a simulation object in stochsim which can be run.

%% Run unit tests to ensure that parser work as expected
unitTestRegexpParsers();

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
if ~exist('fileName', 'var') && isempty(fileName)
    fileName = 'model.cmdl';
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
%% Run unit tests for various parsers.
function unitTestRegexpParsers()
regexpParsers = getRegexParsers();
parserNames = fieldnames(regexpParsers);
for p=1:length(parserNames)
    parser = regexpParsers.(parserNames{p});
    for i=1:length(parser.unitTests)
        [match, names] = regexp(parser.unitTests{i,1}, parser.expression, 'match', 'names', 'once');
        if strcmp(match, parser.unitTests{i,1}) ~= parser.unitTests{i,2} 
            if isempty(match)
                match = '';
            end
            ME = MException('stochsim:parserUnitTestFailed', [...
                'Unit test %g of regexp parser %s failed!\n',...
                ' Regexp      :"%s"\n',...
                ' Input       :"%s"\n',...
                ' Should match:%g\n',...
                ' Match       :"%s"\n'],...
                i, parserNames{p}, parser.expression, parser.unitTests{i,1}, parser.unitTests{i,2}+0, match);
            throw(ME);
        end
    end
end
end

%% Various regular expressions to parse reactions or assignemnts
function result = evalRegexParser(parser, string)
[match, result] = regexp(string, parser.expression, 'match', 'names', 'once');
if ~strcmp(string, match)
    ME = MException('stochsim:parseFail', [...
                'Could not parse string!\n',...
                ' Input       :"%s"\n',...
                ' Match       :"%s"\n',...
                ' Regexp      :"%s"\n'],...
                string, match, parser.expression);
            throw(ME);
end
end

function regexParsers = getRegexParsers()
regexParsers = struct();

% Name of a state or variable.
regexParsers.varName = struct();
regexParsers.varName.expression = ...
    '\<[a-zA-Z_]\w*\>';
regexParsers.varName.unitTests = {...
    'a', true;...
    'A', true;...
    'Za', true;...
    'aBcF', true;...
    'T1a', true;...
    'p_Pi', true;...
    '_oo', true;...
    '_1', true;...
    '2', false;...
    '2a', false
    ' ', false;...
    ' A', false;...
    'A ', false;...
    'o!', false;...
    'a12[]', false};
regexParsers.varName.eval = @(string)evalRegexParser(regexParsers.varName, string);
%regexParsers.varName.eval = @(x)iff(strcmp(regexp(x, regexParsers.varName.expression, 'match', 'once'),x), regexp(x, regexParsers.varName.expression, 'math', 'once'), true, []);


% Stochiometry, name and mofidier of one species taking part in a reaction
regexParsers.reactionComponent = struct();
regexParsers.reactionComponent.expression = ...
    ['(?<stochiometry>\<\w+\>)?(?(stochiometry)\s*[\*\s]\s*)(?<state>', regexParsers.varName.expression, ')(?<modifier>\[\])?'];
regexParsers.reactionComponent.unitTests = {...
    '2 * aA_[]', true;...
    'A', true;...
    ' A', false;...
    'A ', false;...
    '*A', false;...
    'a*Aa', true;...
    '2 A', true;...
    '2*A', true;...
    '2 * A', true;...
    '2A', false;...
    'A[]', true;...
    'A[ ]', false};
regexParsers.reactionComponent.eval = @(string)evalRegexParser(regexParsers.reactionComponent, string);

% Everything in a reaction which is NOT used to split the reaction into its
% fundamental parts
regexParsers.reactionPart = struct();
regexParsers.reactionPart.expression = [...
    '(?:([^,\-<\s]|(?<!<)\-(?!>)|<(?!\-))'...
    '([^,\-<]|(?<!<)\-(?!>)|<(?!\-))*'...
    '([^,\-<\s]|(?<!<)\-(?!>)|<(?!\-))'...
    '|[^,\-\s<]?)'];
regexParsers.reactionPart.unitTests = {...
    'sfsdlöfk sdöfksdfö ppop', true;...
    'A->B', false;...
    'A-->B', false;...
    'a,BBB', false;...
    'A-B', true};
regexParsers.reactionPart.eval = @(string)evalRegexParser(regexParsers.reactionPart, string);

% Only the formula of a reaction, without the reaction name or similar.
regexParsers.reactionFormula = struct();
regexParsers.reactionFormula.expression = [...
    '(?<LHS>', regexParsers.reactionPart.expression, ')',...
    '\s*->\s*'...
    '(?<RHS>', regexParsers.reactionPart.expression, ')'];
regexParsers.reactionFormula.unitTests = {...
    'A -> B', true;...
    'sdfsdfdsf->ztjhfg jhfgh', true;...
    'sdfsdfdsf -> ztjhfg jhfgh', true;...
    'sdfsdfdsf - > ztjhfg jhfgh', false;...
    ' sdfsdfdsf->ztjhfg jhfgh', false;...
    '1s+!"§$%&/()=?´df2 ***sdfdsf->ztjhfg 666jhfgh', true;...
    ',1sdf2 ***sdfdsf->ztjhfg 666jhfgh', false;...
    '   ,1sdf2 ***sdfdsf->ztjhfg 666jhfgh', false;...
    '1sdf2 ***sdfdsf->ztjhfg 666jhfgh,', false;...
    '1sdf2 ***sdfdsf->ztjhfg 666jhfgh,  ', false;...
    '1sdf2 ***sdfdsf-->ztjhfg 666jhfgh', true;...
    '1sdf2 ***sdfdsf<->ztjhfg 666jhfgh', false;...
    '1sdf2 ***sdfdsf<-ztjhfg 666jhfgh', false};
regexParsers.reactionFormula.eval = @(string)evalRegexParser(regexParsers.reactionFormula, string);

% Complete reaction definition
regexParsers.reaction = struct();
regexParsers.reaction.expression = [...
    '\s*(?<reactionName>', regexParsers.varName.expression,')?(?(reactionName)\s*,\s*)',...
    regexParsers.reactionFormula.expression,...
    '\s*,\s*',...
    '(?<arguments>', regexParsers.reactionPart.expression, '(\s*,\s*)?)+\s*'];
regexParsers.reaction.unitTests = {...
    'A -> B,1', true;...
    'huhu,  A -> B   , fdfsfsdf, test ', true};
regexParsers.reaction.eval = @(string)evalRegexParser(regexParsers.reaction, string);
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
    parseTree = parseParameter(line, parseTree);
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
    if isempty(def.name)
        def.name = sprintf('reaction_%g', i);
    end
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

if isempty(reactionComponents.reactionName)
    reaction.reactionName = [];
else
    [match, reactionName] = regexp(reactionComponents.reactionName, reactionNameExpr, 'match', 'names', 'once');
    if ~strcmp(reactionComponents.reactionName, match)
        error('stochsim:invalidReactionName', 'The string %s is not a valid name for a reaction!', reactionComponents.reactionName);
    end
    reaction.reactionName = reactionName.reactionName;
end


% Reactants and products
reaction.lhsComponents = parseReactants(reactionComponents.LHS);
reaction.rhsComponents = parseReactants(reactionComponents.RHS);

reactionPropertieStrs = strsplit(reactionComponents.arguments, ',');
for i=1:length(reactionPropertieStrs)
    [match, reactionProperty] = regexp(reactionPropertieStrs{i}, reactionPropertyExpr, 'match', 'names', 'once');
    if isempty(reactionProperty)
        reaction.rate = strtrim(reactionPropertieStrs{i});
    else
        reaction.(reactionProperty.propertyName) = reactionProperty.propertyValue;
    end
end
parseTree.reactions{end+1} = reaction;
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
function components = parseReactants(reactionComponentsStr)
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
        components{i}.stochiometry = '1';
    else
        components{i}.stochiometry = reactionComponent.stochiometry;
    end
    components{i}.state = reactionComponent.state;
    components{i}.modifier = ~isempty(reactionComponent.modifier);
end
end

function [speciesNames, stochiometries, modifiers] = parseReactantsOld(parseTree, reactantsStr)

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