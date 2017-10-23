% This script exemplifies how to use stochsim to simulate the simple
% enzyme(E)-substrate(S) reaction network
% E + S -> ES       rate constant 1
% ES    -> E + S    rate constant 0.1
% ES    -> E + P    rate constant 0.01
% This network is constructed in two different ways: First by loading the
% corresponding CMDL file "Michaelis.cmdl", and then by
% manually/computationally constructing the network in Matlab. For the
% former, it is demonstrated how to modify the rate constant of the first
% reaction to 0.1, for example to allow for parameter searches.

%% Run enzyme(E)-substrate(S) reaction network via CMDL model file
% Load CMDL model
sim = stochSimulation('Michaelis.cmdl');
% Set time resolution of simulation result to store the state every 0.1
% time units. Note that this only affects how often the output is saved,
% not how the simulation is performed.
sim.logPeriod = 0.1;
% Run simulation for 200 time units.
[headers,data] = sim.run(200);
% Plot data.
% First column is time, other columns are molecule numbers.
figure('Name', 'CMDL model with original binding rate constant');
plot(data(:, 1), data(:, 2:end));
legend(headers(2:end));
xlabel('Time');
ylabel('Molecule numbers');

%% Run enzyme(E)-substrate(S) reaction network via CMDL model file, change parameters
% To change a parameter, simply construct a structure having a field with
% the same name as the parameter and its corresponding new value, and pass 
% it as an additional parameter when reading the CMDL file. 
% In the CMDL file, the parameter describing the rate constant for
% the first reaction is named k_binding
p = struct('k_binding', 0.1);
sim = stochSimulation('Michaelis.cmdl', p);
% Set time resolution of simulation result to store the state every 0.1
% time units. Note that this only affects how often the output is saved,
% not how the simulation is performed.
sim.logPeriod = 0.1;
% Run simulation for 200 time units, 500 times, and calculate mean.
% Deactivate console progress for this (slows down simulations if they are
% very simple).
sim.logConsole = false;
data = cell(1,500);
for i=1:length(data)
    [headers,data{i}] = sim.run(200);
end
% calculate mean
meanData = zeros(size(data{1}));
for i=1:length(data)
    meanData = meanData + data{i};
end
meanData = meanData/length(data);
% Plot data.
% First column is time, other columns are molecule numbers.
figure('Name', 'CMDL model with lower binding rate constant');
plot(meanData(:, 1), meanData(:, 2:end));
legend(headers(2:end));
xlabel('Time');
ylabel('Molecule numbers');

% Compare with rough analytic estimate.
hold on;
plot(meanData(:, 1), 100*(1-exp(-0.01*meanData(:, 1))), 'k-.');

%% Manually construct and run same model
% Create empty simulation
sim = stochSimulation();
% Add species E, S, P and ES, together with their initial conditions.
E = sim.createState('E', 100);
S = sim.createState('S', 100);
ES = sim.createState('ES', 0);
P = sim.createState('P', 0);
% Add reactions with corresponding rate constants
% Note: Every reaction must have a name for identification purposes.
% 1) E + S -> ES       rate constant 1
R1 = sim.createPropensityReaction('enzyme_substrate_combine', 1);
% Reactants and products can be added either by name or by reference.
R1.addReactant(E);
R1.addReactant('S');
R1.addProduct(ES);
% 2) ES    -> E + S    rate constant 0.1
% stochsim requires reversible reactions to be split into two
% non-reversible ones.
R2 = sim.createPropensityReaction('enzyme_substrate_separate', 0.1);
R2.addReactant('ES');
R2.addProduct(E);
R2.addProduct('S');
% 3) ES    -> E + P    rate constant 0.01
R3 = sim.createPropensityReaction('make_product', 0.01);
R3.addReactant('ES');
R3.addProduct(E);
R3.addProduct('P');
% Additionally to getting the data, save result in file simulation/data.csv
sim.baseFolder = 'simulation';
sim.resultFile = 'data1.csv';
sim.uniqueSubfolder = false;
sim.logPeriod = 0.1;
% Run simulation for 100 time units.
sim.logConsole = true;
[headers,data1] = sim.run(100);

% Set initial conditions to final values
for i=2:length(headers)
    state = sim.getState(headers{i});
    state.initialCondition = data1(end, i);
end
% Change propensity of first reaction to depend quadratically on substrate
% concentration
R1.rateEquation = '0.01*E*S*(S-1)';
% continue simulation for another 100 time units
sim.resultFile = 'data2.csv';
[~,data2] = sim.run(100);
% change t0 for simulation
data2(:, 1) = data2(:, 1) + data1(end, 1);
% combine simulations
data = [data1; data2];
% Plot data.
figure('Name', 'Custom model');
plot(data(:, 1), data(:, 2:end));
legend(headers(2:end));
xlabel('Time');
ylabel('Molecule numbers');