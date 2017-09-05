function sim = stochParseModel( fileName )
%parseModel Parses the model saved in fileName (usually a CMDL file) and
%creates a simulation object in stochsim which can be run.

%% Setup
parseTree = struct();
% Struct p holds all declared parameters
parseTree.p = struct();
parseTree.states = cell(1,0);

% Create simulation object
sim = stochSimulation('sim_results', 1, false);

%% Iterate over model lines
fid = fopen(fileName);
line = fgetl(fid);
lineID = 1;
while ischar(line)
    %% parse line
    try
        [sim, parseTree] = parseLine(line, sim, parseTree);
    catch e
        error(e.identifier, '%s\n\tin file %s, line %g:\n\t%s', e.message, fileName, lineID, line);
        %rethrow(e);
    end
    %% Read in next line
    line = fgetl(fid);
    lineID = lineID+1;
end
fclose(fid);
end

%% Parse a line in the CMDL file
function [sim, parseTree] = parseLine(line, sim, parseTree)
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
     [sim, parseTree] = parseAssignment(line, sim, parseTree);
     return;
end
% check if reaction
idx = strfind(line, '->');
if length(idx) > 1
    error('stochsim:multipleReactionSigns', 'Line does contain more than one reaction sign ("->")!');
elseif length(idx) == 1
     [sim, parseTree] = parseReaction(line, sim, parseTree);
     return;
end
error('stochsim:notAssignmentNotReaction', 'Line is neither an assignment, nor a reaction!');
end

%% Parse assignment
function [sim, parseTree] = parseAssignment(assignment, sim, parseTree)
idx = strfind(assignment, '=');
name = strtrim(assignment(1:idx-1));
valueStr = strtrim(assignment(idx+1:end));
% check name
invalidIdx = regexp(name, '\W');
if ~isempty(invalidIdx)
    error('stochsim:invalidVariableName', 'Variable name "%s" is invalid since it contains the character ''%c''!', name, name(invalidIdx(1)));
end
% rewrite value to match parameter structure
extendedValueStr = extendParameterExpression(valueStr);
try
    value = eval(extendedValueStr);
catch e
    error('stochsim:parseError', 'Could not parse value string "%s"!', valueStr);
end
% set value
parseTree.p.(name) = value;
end

%% Parse reaction
function [sim, parseTree] = parseReaction(reactionStr, sim, parseTree)

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

% get rate constant
extendedRateStr = extendParameterExpression(rateStr);
try
    rateConstant = eval(extendedRateStr);
catch e
    error('stochsim:parseError', 'Could not parse value of rate constant "%s"!', rateStr);
end
[lhs_speciesNames, lhs_stochiometries] = parseReactants(lhs);
[rhs_speciesNames, rhs_stochiometries] = parseReactants(rhs);

% Create species which occur in reaction but are yet not initialized.
for speciesNameCell = [lhs_speciesNames,rhs_speciesNames]
    speciesName = speciesNameCell{1};
    if ~any(cellfun(@(x)strcmp(x, speciesName), parseTree.states))
        parseTree.states{end+1} = speciesName;
        if ~isfield(parseTree.p, speciesName)
            error('stochsim:noIC', 'Initial condition for state "%s" not set. Set IC for states in the same way as parameters, e.g. "%s = 0;"!', speciesName, speciesName);
        end
        sim.createState(speciesName, parseTree.p.(speciesName));
    end
end

% Create reaction
reaction = sim.createPropensityReaction([lhs,'->',rhs], rateConstant);
for i=1:length(lhs_speciesNames)
    reaction.addReactant(lhs_speciesNames{i}, lhs_stochiometries(i));
end
for i=1:length(rhs_speciesNames)
    reaction.addProduct(rhs_speciesNames{i}, rhs_stochiometries(i));
end
end
 %% extendParameterExpression
function parameterExpression = extendParameterExpression(parameterExpression)
    parameterExpression = regexprep(parameterExpression, '([a-zA-Z_]\w*)', 'parseTree.p.$1');
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