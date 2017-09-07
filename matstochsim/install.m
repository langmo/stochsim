function install(compile)

if ~exist('compile', 'var') || isempty(compile)
    compile = false;
end

if compile
    %% Setup Mex Compiler Options
    defines = {'NDEBUG', '_WINDOWS', '_USRDLL', 'MATSTOCHSIM_EXPORTS', 'MATLAB_MEX_FILE', '_UNICODE', 'UNICODE'};
    definesString = cellfun(@(x)['-D',x], defines, 'UniformOutput', false);
    includeDirs ={'src/stochsim', 'src/matstochsim'}; 
    includeDirsString = cellfun(@(x)['-I',x], includeDirs, 'UniformOutput', false);
    files = {};

    for i=1:length(includeDirs)
        dirFiles = dir(includeDirs{i});
        for j=1:length(dirFiles)
            if ~isempty(strfind(dirFiles(j).name, '.cpp'))
                files{end+1} = fullfile(includeDirs{i}, dirFiles(j).name);
            end
        end
    end
    %% Run compiler
    mex('-largeArrayDims', definesString{:}, includeDirsString{:}, '-output', 'matstochsim', files{:});
end
%% Add path to search path
addpath(cd())  
savepath();
end