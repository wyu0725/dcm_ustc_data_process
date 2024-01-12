#include "dec_single_file.hh"

using namespace std;
dec_single_file::dec_single_file(string f_name_prefix, int c_loop_num, string o_filename_suffix, bool is_usb, string logFilename, bool is_write_txtfile)
{
	is_save_binary = is_write_txtfile;
	log_filename = logFilename;
	filename_prefix = f_name_prefix;
	current_file_loop_num = c_loop_num;
	out_filename_suffix = o_filename_suffix;

	save_filename = f_name_prefix + "-" + to_string(c_loop_num) + "_" + o_filename_suffix;
	init(save_filename);
	is_usb_data = is_usb;
}

dec_single_file::~dec_single_file()
{
	delete[] data_buffer;
	delete out_file;
	// data_tree->Delete();
}

void dec_single_file::init(string file_name)
{
	log_file.open(log_filename, ios::app);
	// Write current time to log file
	time_t now = time(0);
	char *dt = ctime(&now);
	log_file << "++++++++" << endl;
	log_file << "Start time: " << dt << endl;

	init_tree(file_name);
	if (is_save_binary)
	{
		init_bianry_file(file_name);
	}
}

void dec_single_file::init_tree(string file_name)
{
	data_file_name = file_name + ".root";
	// cout << file_name << endl;
	// data_file_name.replace(strlen(file_name) - 4, 4, ".root");
	out_file = new TFile(data_file_name.c_str(), "recreate");
	data_tree = new TTree("fec_origin_data", "FEC DATA");
	data_tree->SetAutoSave();
	data_tree->Branch("time_stamp", &time_stamp, "time_stamp/L");
	data_tree->Branch("trigger_id", &trigger_id, "trigger_id/I");
	data_tree->Branch("nhits", &nhits, "nhits/I");
	data_tree->Branch("chip_id", chip_id, "chip_id[nhits]/I");
	data_tree->Branch("chn_id", chn_id, "chn_id[nhits]/I");
	data_tree->Branch("adc_data", adc_data, "adc_data[nhits][512]/I");
	cout << "ROOT file: " << data_file_name << " created successfully" << endl;
	write_log("ROOT file: " + data_file_name + " created successfully");
}

void dec_single_file::init_bianry_file(string filename)
{
	binary_dout_filename = filename + "_binary.dat";
	binary_dout_file.open(binary_dout_filename, ios::binary);
	if (binary_dout_file.is_open())
	{
		cout << "Binary file: " << binary_dout_filename << " open successfully" << endl;
		write_log("Binary file: " + binary_dout_filename + " open successfully");
	}
	else
	{
		cout << cRED << "Error in create file: " << cYELLOW << binary_dout_filename << cRESET << endl;
	}
}

bool dec_single_file::init_data(string filename, bool skip_remain_data)
{
	data_file.open(filename, ifstream::binary);
	if (data_file.is_open())
	{

		is_file_exist = true;
		data_file.seekg(0, data_file.end);
		data_num_bytes = data_file.tellg();
		data_file.seekg(0, data_file.beg);
		data_buffer = new char[data_num_bytes];
		auto start = chrono::high_resolution_clock::now();
		data_file.read(data_buffer, data_num_bytes);
		auto finish = chrono::high_resolution_clock::now();

		auto microseconds = chrono::duration_cast<chrono::microseconds>(finish - start);
		double read_time_s = microseconds.count() / 1000. / 1000.;

		data_file.close();
		cout << "----------" << endl;
		cout << cCYAN << "File " << filename << " read successfully (" << read_time_s * 1000. << "ms). "
			 << "File size: "
			 << data_num_bytes / 1024. / 1024. << "MB"
			 << ", Speed: " << data_num_bytes / 1024. / 1024. / read_time_s << "MB/s"
			 << cRESET << endl;
		log_file << "----------" << endl;
		log_file << "File " << filename << " read successfully (" << read_time_s * 1000. << "ms). "
				 << "File size: "
				 << data_num_bytes / 1024. / 1024. << "MB"
				 << ", Speed: " << data_num_bytes / 1024. / 1024. / read_time_s << "MB/s" << endl;
		gbt_data_length = floor(data_num_bytes / 2);

		if (!is_usb_data)
		{
			raw_data = new unsigned short[gbt_data_length];
			for (int i = 0; i < gbt_data_length; i++)
			{
				raw_data[i] = (unsigned char)data_buffer[2 * i] * 0x100 + (unsigned char)data_buffer[2 * i + 1];
			}
			fec_data = new unsigned short[gbt_data_length];
			remove_flx_frame();
			last_pkg_ptr = end_ptr;
			remain_data_len = 0;
			delete raw_data;
		}
		else
		{
			if (!skip_remain_data && current_file_loop_num != 0)
			{
				load_last_remain_data();
			}
			else
			{
				cout << cYELLOW << "Skip remain data" << cRESET << endl;
				write_log("Skip remain data");
			}
			if (remain_data_len == 0)
			{
				cout << cYELLOW << cBOLDWHITE << "Warning: Skip last package remain data" << cRESET << endl;
				delete remain_data;
				remain_data_len = 0;
			}
			fec_data = new unsigned short[remain_data_len + gbt_data_length];
			for (int i = 0; i < remain_data_len; i++)
			{
				fec_data[i] = remain_data[i];
			}
			for (int i = 0; i < gbt_data_length; i++)
			{
				fec_data[i + remain_data_len] = (unsigned char)data_buffer[2 * i] * 0x100 + (unsigned char)data_buffer[2 * i + 1];
			}
			end_ptr = gbt_data_length + remain_data_len;
			if (remain_data_len != 0)
			{
				delete remain_data;
			}
			locate_last_pkg();
		}
		read_ptr = 0;
	}
	else
	{
		cout << cRED << "File " << filename << " not exist!" << cRESET << endl;
		is_file_exist = false;
		return false;
	}

	return true;
}

void dec_single_file::show_data(int start, int stop)
{
	if (start < 0 || stop < start || stop > gbt_data_length)
	{
		cout << "Show error" << endl;
		return;
	}
	int num = stop - start;
	ios_base::fmtflags f(cout.flags());
	cout << hex;
	// cout << showbase;
	for (int i = start; i < stop; i++)
	{
		cout << "0x" << setfill('0') << setw(4) << right << raw_data[i] << " ";
		if (i % 16 == 0 && i != 0)
		{
			cout << endl;
		}
	}
	cout << dec;
	cout << endl;
	cout.flags(f);
}

void dec_single_file::write_binary_file()
{
	binary_dout_file.write((char *)&header2write, sizeof(header2write));
	binary_dout_file.write((char *)&trigger_id2write, sizeof(trigger_id2write));
	nhits2write = my_bswap16(nhits);
	binary_dout_file.write((char *)&nhits2write, sizeof(nhits2write));
	for (int i = 0; i < nhits; i++)
	{
		binary_dout_file.write((char *)&chip_id2write[i], sizeof(unsigned short));
		binary_dout_file.write((char *)&chn_id2write[i], sizeof(unsigned short));
		binary_dout_file.write((char *)&adc_data2write[i], sizeof(unsigned short) * cSCA_NUM);
	}
}



void dec_single_file::write_log(string log)
{
	if (log_file.is_open())
		log_file << log << endl;
}

bool dec_single_file::data2tree()
{
	return data2tree(0);
}

bool dec_single_file::data2tree(int package_num)
{
	if (package_num == 0)
	{
		package_num = INT_MAX;
	}

	// 1+3+2+8=14
	int event_type;
	long time_stamp_a;
	int trigger_id_a;

	int chip_rd;
	int chn_rd;
	unsigned short *adc_data_tmp;

	long long total_sz = 0;
	int frame_type;
	bool is_reach_start = false;
	// Find the fist package, which starts with 0xAC0F 0x401C
	while (!is_reach_start)
	{
		while (read_frame_header(is_reach_start) != 1 && (read_ptr < end_ptr))
			;
		int code = read_frame_type(frame_type);
		if (code != 1)
		{
			cout << "Error while read frame type. Code: " << code << " read_ptr" << hex << read_ptr * 2 << " end_ptr: " << end_ptr * 2 << dec << endl;
			break;
		}
		if (frame_type == 1)
		{
			is_reach_start = true;
		}
	}
	bool is_event_end = false;
	int read_package_num = 0;
	int rd_code;
	while (read_ptr < last_pkg_ptr && read_package_num < package_num)
	{
		read_event_header(trigger_id, time_stamp);
		trigger_id2write = my_bswap32(trigger_id);
		is_event_end = false;
		rd_code = read_crc();
		if (rd_code != 1)
		{
			cout << "Reach to end while reading crc" << endl;
			if (rd_code == -1)
				break;
		}
		while (!is_event_end)
		{
			rd_code = read_frame_header(is_reach_start);
			if (rd_code != 1)
			{
				cout << "Read frame header error. read_ptr" << hex << read_ptr * 2 << " end_ptr: " << end_ptr * 2 << dec << endl;
				if (rd_code == -1)
					break;
			}
			rd_code = read_frame_type(frame_type);
			if (rd_code != 1)
			{
				cout << "Error while read frame type. Code: " << rd_code << " read_ptr" << hex << read_ptr * 2 << " end_ptr: " << end_ptr * 2 << dec << endl;
				if (rd_code == -1)
					break;
			}
			if (frame_type == 3 || nhits > cMAX_HITS)
			{
				if (nhits <= cMAX_HITS)
				{
					rd_code = read_event_tail();
					if (rd_code != 1)
					{
						cout << "Error while read event tail. Code: " << rd_code << " read_ptr" << hex << read_ptr * 2 << " end_ptr: " << end_ptr * 2 << dec << endl;
						if (rd_code == -1)
							break;
					}
					rd_code = read_crc();
					if (rd_code != 1)
					{
						cout << "Error while read crc. Code: " << rd_code << " read_ptr" << hex << read_ptr * 2 << " end_ptr: " << end_ptr * 2 << dec << endl;
						if (rd_code == -1)
							break;
					}

					if (is_save_binary)
					{
						write_binary_file();
					}
					int t = data_tree->Fill();
					total_sz += t;
				}
				is_event_end = true;
				nhits = 0;
				bool is_header_correct = false;
				while (!is_header_correct)
				{
					if (read_frame_header(is_reach_start) != 1)
					{
						cout << "Read frame header error. read_ptr" << hex << read_ptr << " end_ptr: " << end_ptr << dec << endl;
						if (rd_code == -1)
							break;
					}
					rd_code = read_frame_type(frame_type);
					if (rd_code != 1)
					{
						cout << "Error while read frame type. Code: " << rd_code << " read_ptr" << hex << read_ptr * 2 << " end_ptr: " << end_ptr * 2 << dec << endl;
						if (rd_code == -1)
							break;
					}
					if (frame_type != 1)
					{
						cout << "Frame type errorrrrrrrrr" << endl;
						is_header_correct = false;
					}
					else
					{
						is_header_correct = true;
					}
				}
			}
			else if (frame_type == 2)
			{
				rd_code = read_event_data(chip_rd, chn_rd, adc_data_tmp);
				if (rd_code != 1)
				{
					cout << cYELLOW << "Error while read event data. Code: " << rd_code << " read_ptr " << hex << read_ptr * 2 << " end_ptr: " << end_ptr * 2 << dec << cRESET << endl;
					if (rd_code == -1)
						break;
				}
				chip_id[nhits] = chip_rd;
				chn_id[nhits] = chn_rd;
				chip_id2write[nhits] = my_bswap16(chip_rd);
				chn_id2write[nhits] = my_bswap16(chn_rd);
				for (int sca_cnt = 0; sca_cnt < cSCA_NUM; sca_cnt++)
				{
					adc_data[nhits][sca_cnt] = adc_data_tmp[sca_cnt] & 0xFFF;
					adc_data2write[nhits][sca_cnt] = my_bswap16(adc_data_tmp[sca_cnt] & 0xFFF);
				}
				nhits++;
				rd_code = read_crc();
				if (rd_code != 1)
				{
					cout << "Error while read crc. Code: " << rd_code << " read_ptr " << hex << read_ptr * 2 << " end_ptr: " << end_ptr * 2 << dec << endl;
					if (rd_code == -1)
						break;
				}
			}
			else
			{
				cout << "frame_type error at " << hex << read_ptr * 2 << dec << endl;
				cout << "Total (MB): " << total_sz / 1024 / 1024 << endl;
				data_tree->Write("", TObject::kOverwrite);
				cout << "Total entries: " << data_tree->GetEntries() << endl;
				// out_file->Write();
				out_file->Close();
				cout << data_file_name << " write  done!" << endl;
				delete fec_data;
				return false;
			}
		}
	}
	cout << "Total (MB): " << total_sz / 1024.0 / 1024.0 << endl;
	data_tree->Write("", TObject::kOverwrite);
	cout << "Total entries: " << data_tree->GetEntries() << endl;
	// out_file->Write();
	delete fec_data;
	return true;
}

string dec_single_file::get_filename(string path_filename)
{
	char *path_name_c = new char[path_filename.length() + 1];
	strcpy(path_name_c, path_filename.c_str());
	char *name1;
	char *name2;
	name1 = strtok(path_name_c, "/");
	name2 = name1;
	while (name1 != NULL)
	{
		name2 = name1;
		name1 = strtok(NULL, "/");
	}

	delete[] path_name_c;
	return string(name2);
}

void dec_single_file::complete_adding()
{
	out_file->Close();
	cout << cCYAN << data_file_name << " write  done!" << cRESET << endl;
	if (is_save_binary)
	{
		for (int i = 0; i < 10; i++)
		{
			cout << "--";
		}
		cout << endl;
		binary_dout_file.flush();
		binary_dout_file.close();
		cout << cCYAN << binary_dout_filename << " write done!" << cRESET << endl;
		cout << cYELLOW << "Data format:" << endl
			 << " 16'hFF53 + Trigger ID (32bits) + nhit (16 bits) + chip_id (16 bits) + chn_id (16 bits) + 512*16 bits data" << endl
			 << " 16'hFF53 + Trigger ID (32bits) + nhit (16 bits) + chip_id (16 bits) + chn_id (16 bits) + 512*16 bits data" << endl
			 << "......"
			 << cRESET << endl;
		for (int i = 0; i < 10; i++)
		{
			cout << "--";
		}
		cout << endl;
	}
}

template <class T>
inline T dec_single_file::words2int_lsb_first(int *words, int len)
{
	T data = 0;
	for (int i = 0; i < len; i++)
	{
		data += (T)((words[i] & 0xFFFF)) << (16 * i);
	}
	return data;
}

void dec_single_file::remove_flx_frame()
{
	int data_idx = 0;
	int raw_idx = 0;
	bool is_frame_started = false;
	int data_cnt = 0;
	bool is_reach_tail = false;
	while (raw_idx < gbt_data_length)
	{
		if ((raw_idx % 0x200) == 0)
		{
			raw_idx += 2;
			continue;
		}
		if (raw_idx % 0x200 == (0x200 - 1))
		{
			raw_idx++;
			continue;
		}
		if (is_frame_started)
		{
			if (data_cnt == 256)
			{
				is_frame_started = false;
				data_cnt = 0;
				if (raw_data[raw_idx] != cUSER_TAIL)
				{
					cout << "\033[32m"
						 << "User frame tail error. Read value: " << hex << raw_data[raw_idx]
						 << ", setting value: " << cUSER_TAIL << "\033[0m" << dec << endl;
				}
				raw_idx++;
				is_reach_tail = true;
				continue;
			}
			else
			{
				fec_data[data_idx] = raw_data[raw_idx];
				data_cnt++;
				data_idx++;
				raw_idx++;
			}
		}
		else if (is_reach_tail)
		{
			raw_idx++;
			is_reach_tail = false;
			continue;
		}
		else if (raw_data[raw_idx] == cUSR_HEADER)
		{
			is_frame_started = true;
			raw_idx++;
			continue;
		}
		else
		{
			if (raw_data[raw_idx] != 0 && (raw_idx % 0x200) != 0x1FE)
			{
				cout << "\033[32m"
					 << "Spare data is not 0x0000, please check the data at " << hex << raw_idx * 2
					 << " data: " << raw_data[raw_idx] << "\033[0m" << dec << endl;
			}
			raw_idx++;
			continue;
		}
	}
	end_ptr = data_idx;
}

bool dec_single_file::read_chip_data(unsigned short *&a, int &len)
{
	if (read_ptr == end_ptr)
	{
		return false;
	}
	if ((end_ptr - read_ptr) < len)
	{
		len = (end_ptr - read_ptr);
	}
	a = fec_data + read_ptr;
	read_ptr += len;
	return true;
}

int dec_single_file::read_frame_header(bool is_started)
{
	int read_len = 1;
	// Package header ac0f 401c
	unsigned short *data;
	if (!read_chip_data(data, read_len))
	{
		cout << "Reach to data end while reading package header" << endl;
		write_log("Reach to data end while reading package header");
		return -1;
	}
	if (read_len != 1)
	{
		cout << "Data lenth shorter than the required length" << endl;
		write_log("Data lenth shorter than the required length");
		return 0;
	}
	if (data[0] != 0xac0f)
	{
		if (is_started && !is_header_err)
		{
			cout << cRED << "Package is not 0xAC0F. Read data is 0x" << hex << data[0] << " at position 0x" << read_ptr * 2 << dec << endl;
			cout << "Locate next 0xAC0F" << cRESET << endl;
			is_header_err = true;
		}
		read_frame_header(is_started);
	}
	if (is_header_err)
	{
		is_header_err = false;
		cout << cYELLOW << "Located 0xAC0F at 0x" << hex << read_ptr * 2 << dec << cRESET << endl;
	}

	return 1;
}

int dec_single_file::read_frame_type(int &frame_type)
{
	int read_len = 1;
	// Package header ac0f 401c
	unsigned short *data;
	if (!read_chip_data(data, read_len))
	{
		cout << "Reach to data end while reading package type" << endl;
		return -1;
	}
	if (read_len != 1)
	{
		cout << "Data lenth shorter than the required length" << endl;
		return 0;
	}
	if (data[0] == 0x401C)
	{
		frame_type = 1;
	}
	else if (data[0] == 0x0404)
	{
		frame_type = 2;
	}
	else if (data[0] == 0x2008)
	{
		frame_type = 3;
	}
	else
	{
		frame_type = -1;
	}
	return 1;
}

int dec_single_file::read_event_header(int &trigger_id, long &time_stamp)
{
	int read_len = 14;
	unsigned short *header_spare;
	if (!read_chip_data(header_spare, read_len))
	{
		cout << "Reach to data end while reading package header spare data." << endl;
		return -1;
	}
	if (read_len != 14)
	{
		cout << "Data lenth shorter than the required length" << endl;
		return 0;
	}
	trigger_id = header_spare[4] + (header_spare[5] << 16);
	unsigned long word3 = header_spare[3];
	time_stamp = header_spare[1] + (header_spare[2] << 16) + (word3 << 32);
	return 1;
}

int dec_single_file::read_event_data(int &chip, int &chn, unsigned short *&event_data)
{
	int read_len = 1;
	// 0xFFCC chn_chp data*cSCA_NUM_USB 0xEEDD 0xAABB
	unsigned short *data;
	if (!read_chip_data(data, read_len))
	{
		cout << "Reach to data end while reading package package data." << endl;
		return -1;
	}
	if (read_len != 1)
	{
		cout << "Data lenth shorter than the required length" << endl;
		return 0;
	}
	if ((data[0] & 0xF000) != 0xC000)
	{
		cout << "Data channel header error. Read: 0x" << hex << data[0] << ". Excepted value: 0xC***"
			 << "read ptr: 0x" << read_ptr * 2 << dec << endl;
		return -2;
	}
	unsigned short chip_chn = data[0];
	chn = data[0] & 0x7F;
	chip = (data[0] >> 7) & 0x3;
	read_len = cSCA_NUM;
	if (!read_chip_data(event_data, read_len))
	{
		cout << "Reach to data end while reading package package data." << endl;
		return -1;
	}
	if (read_len != cSCA_NUM)
	{
		cout << "Data lenth shorter than the required length" << endl;
		return 0;
	}
	read_len = 1;
	if (!read_chip_data(data, read_len))
	{
		cout << "Reach to data end while reading package package data." << endl;
		return -1;
	}
	if (read_len != 1)
	{
		cout << "Data lenth shorter than the required length" << endl;
		return 0;
	}

	// ����ע�ͣ������ǳ���Ҫ
	// �˴��߼��� bug������˵ FEC �巢�͵�512�� SCA �Ĳ����������Ჹ��һ��0x0000�������е�ʱ�����ֵ����0
	// �������һ�� SCA �ɼ�����������0���˴�����ʾ�ͷ���ֵҲ��û��������
	if (data[0] != 0x0000)
	{
		/*cout << cRED << "Data tail error. Read: 0x" << hex << data[0]
			<< ". Excepted value: 0x0000"
			<< " read ptr: 0x" << read_ptr * 2 << dec << cRESET << endl;
		cout <<hex<<chip_chn<<" " << "Chip " << chip << " chn: " << chn << endl;
		*/
		// return -2;
	}
	return 1;
}

int dec_single_file::read_crc()
{
	int read_len = 2;
	unsigned short *data;
	if (!read_chip_data(data, read_len))
	{
		cout << "Reach to data end while reading CRC data." << endl;
		return -1;
	}
	if (read_len != 2)
	{
		cout << "Data lenth shorter than the required length. Read len" << read_len << " excepted value 2" << endl;
		return 0;
	}
	return 1;
}

int dec_single_file::read_event_tail()
{
	int read_len = 4;
	// 0xFFCC chn_chp data*cSCA_NUM_USB 0xEEDD 0xAABB
	unsigned short *data;
	if (!read_chip_data(data, read_len))
	{
		cout << "Reach to data end while reading pkg_tail data." << endl;
		return -1;
	}
	if (read_len != 4)
	{
		cout << "Data lenth shorter than the required length" << endl;
		return 0;
	}

	return 1;
}

void dec_single_file::locate_last_pkg()
{
	for (int i = end_ptr - 8 - 1; i > 0; i--)
	{
		if (fec_data[i] == 0xAC0F && fec_data[i + 1] == 0x2008)
		{

			last_pkg_ptr = i + 8;
			remain_data_len = end_ptr - last_pkg_ptr;
			remain_data = new unsigned short[remain_data_len];
			cout << "Size of unsigned short: " << sizeof(unsigned short) << " bytes\n";
			for (int j = 0; j < remain_data_len; j++)
			{
				remain_data[j] = my_bswap16(fec_data[last_pkg_ptr + j]);
			}
			string remain_data2file_name = filename_prefix + "-" + to_string(current_file_loop_num) + "_" + out_filename_suffix + "_last_remain_data.dat";
			ofstream remain_data2file(remain_data2file_name, ios::binary);
			if (remain_data2file.is_open())
			{
				remain_data2file.write((char *)remain_data, remain_data_len * 2);
				remain_data2file.close();
				cout << cGREEN << "Save last remain data to file: " << remain_data2file_name << cRESET << endl;
				write_log("Save last remain data to file: " + remain_data2file_name);
			}
			else
			{
				cout << cRED << "Error in create file: " << cYELLOW << remain_data2file_name << cRESET << endl;
				write_log("Error in create file: " + remain_data2file_name);
			}

			cout << cCYAN << "Locate last pkg at 0x" << hex << i * 2
				 << " last_pkg_ptr: 0x" << last_pkg_ptr * 2 << "remain_data_len: 0x" << remain_data_len
				 << cRESET << dec << endl;
			break;
		}
	}
}

void dec_single_file::load_last_remain_data()
{
	string last_remain_data_filename = filename_prefix + "-" + to_string(current_file_loop_num - 1) + "_" + out_filename_suffix + "_last_remain_data.dat";
	cout << cYELLOW << "Load last remain data from file: " << last_remain_data_filename << cRESET << endl;
	write_log("Load last remain data from file: " + last_remain_data_filename);
	ifstream last_remain_data_file;	
	last_remain_data_file.open(last_remain_data_filename, ios::binary);
	if (last_remain_data_file.is_open())
	{
		last_remain_data_file.seekg(0, std::ios::end);
		long long last_remain_data_len = last_remain_data_file.tellg();
		last_remain_data_file.seekg(0);
		char* last_remain_data_buffer = new char[last_remain_data_len];
		last_remain_data_file.read(last_remain_data_buffer, last_remain_data_len);
		last_remain_data_file.close();

		remain_data_len = last_remain_data_len / 2;
		remain_data = new unsigned short[remain_data_len];

		for (int i = 0; i < remain_data_len; i++)
		{
			remain_data[i] = (unsigned char)last_remain_data_buffer[2 * i] * 0x100 + (unsigned char)last_remain_data_buffer[2 * i + 1];
		}

		delete[] last_remain_data_buffer;
		cout << cGREEN << "Load last remain data done!" << cRESET << endl;
		write_log("Load last remain data done!");
		// Delete last remain data file
		if (remove(last_remain_data_filename.c_str()) == 0)
		{
			cout << cGREEN << "Delete last remain data file: " << last_remain_data_filename << " done!" << cRESET << endl;
			write_log("Delete last remain data file: " + last_remain_data_filename + " done!");
		}
		else
		{
			cout << cRED << "Error in delete file: " << cYELLOW << last_remain_data_filename << cRESET << endl;
			write_log("Error in delete file: " + last_remain_data_filename);
		}
	}
	else
	{
		cout << cRED << "Error in open file: " << cYELLOW << last_remain_data_filename << cRESET << endl;
		write_log("Error in open file: " + last_remain_data_filename);
	}
}
