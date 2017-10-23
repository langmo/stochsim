% This script exemplifies how to use stochsim to simulate the GAL network.
% This example is more complex than the Michaelis example, and we only use
% the CMDL functionality of stochsim for this demonstration.
% Please refer to GAL.cmdl for a description of the model.
%% Load and run model
% Load CMDL model
sim = stochSimulation('GAL.cmdl');
% Set time resolution of simulation result to store the state every 0.1
% time units. Note that this only affects how often the output is saved,
% not how the simulation is performed.
sim.logPeriod = 0.1;
% Run simulation for 200 time units.
[headers,data] = sim.run(200);
% Plot data.
% First column is time, other columns are molecule numbers.
figure('Name', 'GAL network');
plot(data(:, 1), data(:, 2:end));
legend(headers(2:end), 'Interpreter', 'none');
xlabel('Time');
ylabel('Molecule numbers');