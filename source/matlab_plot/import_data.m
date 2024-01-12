function [initial_data, file_name] = import_data(file_name)

    if nargin == 1
        [fid,~] = fopen(file_name,'r');
    else
        if ispc
            [filename, filepath] = uigetfile('*.dat','PoCA file','D:\ExperimentsData\test');
        elseif isunix
            [filename, filepath] = uigetfile('*.dat','PoCA file','/mnt/mustc_data');
        end
        file_name = [filepath filename];
        [fid,~] = fopen(file_name,'r');
    end
    if fid <= 0
        % There was an error--tell user
        str = ['File ' file_name ' could not be opened.'];
        initial_data = 0;
        dlg_title = 'File Open Faild';
        errordlg(str, dlg_title, 'modal');
    else
        %File opend successfully
        initial_data = fread(fid, 'uint16', 'ieee-be');%Big-endian ording
        %Size = length(importdata);
        fclose(fid);%close file
    end
end

