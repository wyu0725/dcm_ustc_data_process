function [brd_vth_data, file_name, brd_mean_data, brd_rms_data] = read_noise_file(sigma_level, filename)
if nargin < 1
    sigma_level = 3;
end
if nargin < 2
    if ispc
        [filename, filepath] = uigetfile('*.txt','PoCA file','E:\ExperimentsData\test');
    elseif isunix
        [filename, filepath] = uigetfile('*.txt','PoCA file','~/nas_mustc/MPGD');
    end
    file_name = [filepath filename];
else
    file_name = filename;
end

% read line from file
fid = fopen(file_name);

formatSpec = '%f ';
% for i = 1:68
%     formatSpec = [formatSpec '%f '];
% end
size_file = [68 Inf];
file_data = fscanf(fid,formatSpec, size_file);
fclose(fid);

brd_mean_data = [file_data(:,1)', file_data(:,2)', file_data(:,3)', file_data(:,4)'];
brd_rms_data = [file_data(:,5)', file_data(:,6)', file_data(:,7)', file_data(:,8)'];
brd_vth_data = brd_mean_data + sigma_level * brd_rms_data;
end