if ispc
    [filename, filepath] = uigetfile('*.dat','PoCA file','E:\ExperimentsData\test');
elseif isunix
    [filename, filepath] = uigetfile('*.dat','PoCA file','~/nas_mustc/MPGD');
end
if (length(filename) == 1 && filename == 0) || (length(filepath) == 1 && filepath == 0)
    error("No file select")
end
file_name = [filepath filename];
c = strsplit(file_name,'-');
[trigger_id0, hit_idx0, chn_data0] = read_fec_event(import_data(file_name));
% plot_rms(chn_data0);
file_pre=string(c(1));
for i = 2:length(c)-1
    file_pre = file_pre + '-' + string(c(i));
end
file_name = file_pre + '-1_binary.dat';
[trigger_id1, hit_idx1, chn_data1] = read_fec_event(import_data(file_name));
% plot_rms(chn_data1);
file_name =file_pre + '-2_binary.dat';
[trigger_id2, hit_idx2, chn_data2] = read_fec_event(import_data(file_name));
% plot_rms(chn_data2);
file_name = file_pre + '-3_binary.dat';
[trigger_id3, hit_idx3, chn_data3] = read_fec_event(import_data(file_name));
% plot_rms(chn_data3);
file_name = file_pre + '-4_binary.dat';
[trigger_id4, hit_idx4, chn_data4] = read_fec_event(import_data(file_name));
% plot_rms(chn_data4);
file_name = file_pre + '-5_binary.dat';
[trigger_id5, hit_idx5, chn_data5] = read_fec_event(import_data(file_name));
% plot_rms(chn_data5);
file_name = file_pre + '-6_binary.dat';
[trigger_id6, hit_idx6, chn_data6] = read_fec_event(import_data(file_name));
% plot_rms(chn_data6);
file_name = file_pre + '-7_binary.dat';
[trigger_id7, hit_idx7, chn_data7] = read_fec_event(import_data(file_name));
% plot_rms(chn_data7);
file_name = file_pre + '-8_binary.dat';
[trigger_id8, hit_idx8, chn_data8] = read_fec_event(import_data(file_name));
% plot_rms(chn_data8);
file_name = file_pre + '-9_binary.dat';
[trigger_id9, hit_idx9, chn_data9] = read_fec_event(import_data(file_name));
% plot_rms(chn_data9);
file_name = file_pre + '-10_binary.dat';
[trigger_id10, hit_idx10, chn_data10] = read_fec_event(import_data(file_name));
% plot_rms(chn_data10);
file_name = file_pre + '-11_binary.dat';
[trigger_id11, hit_idx11, chn_data11] = read_fec_event(import_data(file_name));
% plot_rms(chn_data11);
file_name = file_pre + '-12_binary.dat';
[trigger_id12, hit_idx12, chn_data12] = read_fec_event(import_data(file_name));
% plot_rms(chn_data12);
file_name = file_pre + '-13_binary.dat';
[trigger_id13, hit_idx13, chn_data13] = read_fec_event(import_data(file_name));
% plot_rms(chn_data13);
file_name = file_pre + '-14_binary.dat';
[trigger_id14, hit_idx14, chn_data14] = read_fec_event(import_data(file_name));
% plot_rms(chn_data14);

disp(["Load file    " file_name " done"])
