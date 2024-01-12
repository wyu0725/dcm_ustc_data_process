sigma_level = 3;
[brd0_vth, filename, ~, brd0_rms]= read_noise_file(sigma_level);
c = strsplit(filename,'-');

% plot_rms(chn_data0);
file_pre=string(c(1));
for i = 2:length(c)-1
    file_pre = file_pre + '-' + string(c(i));
end

file_name = file_pre + '-1.txt';
[brd1_vth, ~, ~, brd1_rms] = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-2.txt';
[brd2_vth, ~, ~, brd2_rms] = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-3.txt';
[brd3_vth, ~, ~, brd3_rms] = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-4.txt';
[brd4_vth, ~, ~, brd4_rms] = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-5.txt';
[brd5_vth, ~, ~, brd5_rms] = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-6.txt';
[brd6_vth, ~, ~, brd6_rms] = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-7.txt';
[brd7_vth, ~, ~, brd7_rms] = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-8.txt';
[brd8_vth, ~, ~, brd8_rms] = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-9.txt';
[brd9_vth, ~, ~, brd9_rms] = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-10.txt';
[brd10_vth, ~, ~, brd10_rms] = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-11.txt';
brd11_vth = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-12.txt';
brd12_vth = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-13.txt';
brd13_vth = read_noise_file(sigma_level, file_name);
file_name = file_pre + '-14.txt';
brd14_vth = read_noise_file(sigma_level, file_name);
