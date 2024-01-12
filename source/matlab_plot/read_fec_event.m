function [trigger_id, hit_idx, chn_data] = read_fec_event(initial_data)
    if nargin == 0
        initial_data = import_data();
    end
	rd_ptr = 1;
    while initial_data(rd_ptr) ~= 0xFF53
        rd_ptr = rd_ptr + 1;
    end
    trigger_id = zeros(1000, 1);
    hit_idx = zeros(1000, 272);
    chn_data = zeros(1000, 272,512);
    k = 1;
    while rd_ptr < length(initial_data) && k < 1000
        [trigger_id(k), hit_idx(k, :), chn_data(k,:,:), rd_ptr] = read_single_event(initial_data, rd_ptr);
        k = k + 1;
    end
    trigger_id(k:1000) = [];
    hit_idx(k : 1000,:) = [];
    chn_data(k : 1000,:,:) = [];
end