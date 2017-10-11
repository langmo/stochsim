function simResults = stochReadStateFile(filename)

%% Open the file.
fileID = fopen(filename,'r');
%% Read header and find out number and names of columns
headerLine = fgetl(fileID);
headers = strsplit(headerLine, ',');
headers = cellfun(@(x)strtrim(x), headers, 'UniformOutput', false);
formatSpec = [repmat('%f', 1,length(headers)),'%[^\n\r]'];
%% Read data
dataArray = textscan(fileID, formatSpec, inf, 'Delimiter', ',', 'ReturnOnError', false);

%% Close the file.
fclose(fileID);

%% Prepare result struct
simResults = struct();
for i=1:length(headers)
    simResults.(headers{i}) = dataArray{:, i};
end
