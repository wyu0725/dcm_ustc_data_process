#ifndef dec_single_file_hh
#define dec_single_file_hh 1

#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include "limits.h"

#include "constant.hh"

using namespace std;
class dec_single_file
{
public:
	dec_single_file(string file_name_prefix, int c_loop_num, string o_filename_suffix, bool is_usb_data = false, string logFilename = "./dec_single_file_log.txt", bool is_write_txtfile = false);
	~dec_single_file();
	bool init_data(string filename, bool skip_remain_data = false);
	
	void show_data(int start, int stop);
	
	bool data2tree();
	bool data2tree(int);
	int gbt_data_length;
	string data_file_name;
	string get_filename(string path_filename);
	bool is_file_exist = false;
	bool crc_check_en = false;

	void complete_adding();
	
private:
	string save_filename; // without file type suffix
	string filename_prefix;
	int current_file_loop_num;
	string out_filename_suffix;
	bool is_save_binary = false;
	char* data_buffer;
	
	unsigned read_ptr;
	unsigned int end_ptr;
	unsigned last_pkg_ptr;
	unsigned int remain_data_len = 0;
	unsigned short* raw_data;
	unsigned short* fec_data;
	unsigned short* remain_data;
	
	long data_num_bytes;
	ifstream data_file;

	// ROOT data
	TFile* out_file;
	TTree* data_tree;
	long time_stamp = 0;
	int trigger_id = 0;
	int nhits = 0;
	int chip_id[cMAX_HITS];
	int chn_id[cMAX_HITS];
	int adc_data[cMAX_HITS][cSCA_NUM];
	template <class T>
	T words2int_lsb_first(int* words, int len);
	void init(string);
	void init_tree(string);
	void init_bianry_file(string);
	string binary_dout_filename;
	ofstream binary_dout_file;
	unsigned short header2write = 0x53FF;
	int trigger_id2write = 0;
	unsigned short nhits2write = 0;
	unsigned short chip_id2write[cMAX_HITS];
	unsigned short chn_id2write[cMAX_HITS];
	unsigned short adc_data2write[cMAX_HITS][cSCA_NUM];

	void write_binary_file();
	void write_log(string log);
	ofstream log_file;
	string log_filename;

	bool is_usb_data = false;

	void remove_flx_frame();

	bool read_chip_data(unsigned short*& a, int& len);

	// return value:
	// 1: correct
	// 0: Shorter than the required length
	// -1: End of file
	// -2: Content error
	int read_frame_header(bool);
	int read_frame_type(int& frame_type);
	int read_event_header(int& trigger_id, long& time_stamp);
	int read_event_header(int& trigger_id, long& time_stamp, long& fec_dna);
	int read_event_data(int& chip, int& chn, unsigned short*& event_data);
	int read_crc();
	int read_event_tail();

	void locate_last_pkg();
	bool is_header_err = false;
	int header_err_position = 0;

	void load_last_remain_data();

	inline uint16_t my_bswap16(uint16_t x) {
		return (x << 8) | (x >> 8);
	}
	inline uint32_t my_bswap32(uint32_t x) {
		return ((x << 24) & 0xFF000000) |
			   ((x << 8) & 0x00FF0000) |
			   ((x >> 8) & 0x0000FF00) |
			   ((x >> 24) & 0x000000FF);
	}
};

#endif // !dec_single_file_hh
