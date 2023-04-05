%% Configuration
modelFile = 'PhageInfect.cmdl';
logPeriod = 1/60;
simTime = 12; % [h]
dateString = char(datetime('now','Format','yyyy-MM-dd_HH-mm-ss'));
saveFolder = fullfile('PhageInfect', dateString);
if ~exist(saveFolder, 'dir')
    mkdir(saveFolder);
end

%% Overwrite parameter values
% This struct is passed to stochsim when it is parsing the model file.
% All parameters defined here overwrite the corresponding parameters
% defined in the model file.
p = struct('V', 0.01, 'fatePeriod', 10/60); % Smaller volume decreases simulation runtime.

%% Parse/read in model file
sim = stochSimulation(modelFile, p);
sim.logPeriod = logPeriod;
sim.logConsole = true;
sim.baseFolder = saveFolder;
sim.uniqueSubfolder = false;
sim.resultFile = 'states.csv';
Bf = sim.getState('Bf');
Bf.logDecreaseToFile('epis.csv', 0:1, 20);

%% Run simulation
[header,data] = sim.run(simTime);

%% Read in simulation results
simResult = stochReadStateFile(fullfile(saveFolder, 'states.csv'));
EPIs = stochReadStateFile(fullfile(saveFolder, 'epis.csv'));
EPIs.Time = EPIs.Time-p.fatePeriod; % EPIs are recorded at the times the cell lyse. We want to plot them, however, according to the first infection of the cell

%% Calculate differential EPI, average EPI and distribution of EPIs
getEPIs = @(r) EPIs.(sprintf('value%g', r));
numEPIs = length(fieldnames(EPIs))-2;

% Rate of infections at a given time
infectionRate = sum(cell2mat(arrayfun(@(r)getEPIs(r), 1:numEPIs, 'UniformOutput', false)), 2);
% Distribution of EPIs, but only for those bacteria infected at the given
% time.
distriEPI_atTime = cell2mat(arrayfun(@(r) getEPIs(r)./max(1,infectionRate), 1:numEPIs, 'UniformOutput', false));
% differential EPI
differentialEPI = arrayfun(@(r) sum(distriEPI_atTime(r, :).*(1:numEPIs)), 1:size(distriEPI_atTime, 1))';
distriEPI = arrayfun(@(r)sum(getEPIs(r)), 1:numEPIs) / sum(infectionRate);
averageEPI = sum(distriEPI.*(1:numEPIs)); 

%% Plot results
figure('Units', 'centimeters', 'Position', [5,5,20,20], 'Color', ones(1,3));
xlims = simResult.Time([1,end]);

% Plot concentrations
subplot(3,1,1)
ylims = [1/p.V, max((simResult.P_i+simResult.P_s)/p.V)];
plot(simResult.Time, (simResult.Bm+simResult.Bf+simResult.Bl_i+simResult.Bl_s+simResult.Bp_i+simResult.Bp_s)/p.V, 'k');
hold all;
plot(simResult.Time, (simResult.P_i+simResult.P_s)/p.V, 'r');
plot(simResult.Time, (simResult.Bp_i+simResult.Bp_s)/p.V, 'r-.');
set(gca, 'YScale', 'log');
legend({'bacteria','phages', 'lysogens'}, 'FontSize', 8, 'Location', 'SouthEast');
legend boxoff;
set(gca(), 'YTick', 10.^(floor(log10(1/p.V)):ceil(log10(max(infectionRate/p.V/logPeriod*60)))));
xlim(xlims);
ylim(ylims);
xlabel('Time (h)');
ylabel('Concentration (ml^{-1})');
box off;

% infection rate and differential EPI
subplot(3,1,2);
[axHs,plotH1,plotH2] = plotyy(EPIs.Time, infectionRate/p.V/logPeriod/60, EPIs.Time, differentialEPI);

axes(axHs(1));
ylims = [10^floor(log10(min(infectionRate(infectionRate>0)/p.V/logPeriod/60))), 10^ceil(log10(max(infectionRate/p.V/logPeriod/60)))];
plotH1.Color = zeros(1,3);
plotH1.LineStyle = ':';
plotH1.LineWidth = 2;
axHs(1).YColor= plotH1.Color;
set(axHs(1), 'YScale', 'log');
set(axHs(1), 'YTick', 10.^(0:ceil(log10(max(infectionRate/p.V/logPeriod/60)))));
xlim(xlims);
ylim(axHs(1), ylims);
xlabel('Time (h)');
ylabel(axHs(1), 'Infection rate (ml^{-1}min^{-1})');
box off;

axes(axHs(2));
ylims = [0, ceil(max(differentialEPI))];
plotH2.Color = zeros(1,3);
axHs(2).YColor= plotH2.Color;
xlim(xlims);
ylim(axHs(2), ylims);
set(axHs(2), 'YTick', ylims(1):ylims(2));
ylabel(axHs(2), 'Differential <EPI|t>');
box off;

% distribution of EPIs
subplot(3,1,3);
maxEPI = 6;
bar(1:maxEPI, distriEPI(1:maxEPI)*100);
for i=1:maxEPI
    text(i, distriEPI(i)*100, sprintf('%d%%',round(100*distriEPI(i))), 'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'center', 'FontSize', 8);
end
text(6, 90, {sprintf('Initial phage/bacteria ratio: %2.2f', (simResult.P_i(1)+simResult.P_s(1))/(simResult.Bm(1)+simResult.Bf(1)+simResult.Bl_i(1)+simResult.Bl_s(1)+simResult.Bp_i(1)+simResult.Bp_s(1))), ...
    sprintf('Average <EPI>: %2.2f', averageEPI)}, 'FontSize', 12, 'VerticalAlignment', 'top', 'HorizontalAlignment', 'right');

ylabel('Probability');
xlabel('EPI');
xlim([0.5,maxEPI+0.5]);
ylim([0, 100]);
box off;
