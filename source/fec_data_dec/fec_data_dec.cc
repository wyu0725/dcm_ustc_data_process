// fec_data_dec.cpp : Defines the entry point for the application.
//

#include "fec_data_dec.hh"

using namespace std;

int main(int argc, char **argv)
{
	// -f Get filename
	string first_filename;
	// -L
	// int loop_number = 1;
	bool arg_hit = false;
	// -T -t
	bool is_save_binary = false;
	// -n
	string file_seprate_name = "_mt";
	// -u -U
	bool is_usb_data = true;
	// -o
	string out_filename_suffix = "out";
	// -S
	bool is_skip_remain_data_loop = false;
	while ((c = getopt(argc, argv, "f:l:L:tTn:uUgGo:S")) != EOF)
	{
		arg_hit = true;
		switch (c)
		{
		case 'f':
			first_filename = optarg;
			break;
		// case 'l':
		// case 'L':
		// 	loop_number = atoi(optarg);
		// 	break;
		case 't':
		case 'T':
			is_save_binary = true;
			break;
		case 'n':
			file_seprate_name = optarg;
			break;
		case 'g':
		case 'G':
			is_usb_data = false;
			break;
		case 'u':
		case 'U':
			is_usb_data = true;
			break;
		case 'o':
			out_filename_suffix = optarg;
			break;
		case 'S':
			is_skip_remain_data_loop = true;
			break;
		case '?':
			cout << cRED;
			cout << "<<---------------------->>" << endl;
			cout << "[Error]: unknow parameters" << endl;
			cout << "<<---------------------->>" << endl;
			cout << "Or forget break in the last case?" << endl;
			show_info(argv[0]);
			return -1;
			break;
		default:
			break;
		}
	}
	if (first_filename.empty() || !arg_hit)
	{
		cout << cRED << "[Error]: Filename must be specificed" << cRESET << endl;
		show_info(argv[0]);
		return -1;
	}
	// Check if first_filename contain ".dat"
	if (first_filename.find(".dat") == string::npos)
	{
		cout << cRED << "[Error]: Filename: " << cYELLOW << first_filename << cRED << " does not contain .dat. Please check the file" << cRESET << endl;
		return -1;
	}

	for (int i = 0; i < 20; i++)
	{
		cout << "==";
	}
	cout << endl;
	cout << "Begin process:" << endl;
	// bool single_file = false;
	// if (loop_number == 1)
	// {
	// 	single_file = true;
	// }
	// cout << file_seprate_name << endl;

	// 这一步是为了将文件名按照指定的 file_seprate_name 分割，默认的是 _mt
	// 通常我的文件名是 run_20231123-0_mt-0.dat run_20231123-0_mt-1.dat run_20231123-0_mt-2.dat
	// _mt 后面的的数字是板号，-0.dat 是第一块板子的数据，-1.dat 是第二块板子的数据，-2.dat 是第三块板子的数据
	// 以run_20231123-0_mt-0.dat文件名为例，经过拆分后 filename_with_loop_vec[0] = "_mt-0.dat" filename_with_loop_vec[1] = "run_20231123-0"
	vector<string> filename_with_loop_vec = split_string(first_filename, file_seprate_name);
	if (filename_with_loop_vec.size() != 2)
	{
		cout << cRED << "[Error]: Filename: " << cYELLOW << first_filename << cRED << " does not contain" << file_seprate_name << "Please check the file" << cRESET << endl;
		return -1;
	}
	// 经过上面的分解过程 file_name_suffix[1] = "run_20231123-0" 短横线后面的数字就是采数的循环编号
	// 通过下面的代码将 run_20231123-0 拆分成 run_20231123 和 0
	// 即 file_prefix_without_loop_vec[0] = "0" file_prefix_without_loop_vec[1] = "run_20231123"
	vector<string> file_prefix_without_loop_vec = split_string(filename_with_loop_vec[1], "-", 1);
	if (file_prefix_without_loop_vec.size() != 2)
	{
		cout << cYELLOW << "Filename: " << cRESET << first_filename << cYELLOW << " does not contain idx. Assume single file and not fill the previous remain data." << cRESET << endl;
		cout << cYELLOW << "Continue? [y/n]" << cRESET << endl;
		char c;
		cin >> c;
		if (c != 'y')
		{
			cout << cRED << "User abort!" << cRESET << endl;
			return -1;
		}
	}
	int current_file_loop_num;
	if (is_number(file_prefix_without_loop_vec[0])) // Check if the file_prefix[0] is a number
	{
		current_file_loop_num = stoi(file_prefix_without_loop_vec[0]);
	}
	else
	{
		cout << cRED << "Start idx: " << file_prefix_without_loop_vec[0] << " is not a integer number. Please check the filename: " << cYELLOW << first_filename << cRESET << endl;
		return -1;
	}

	string file_name_first = file_prefix_without_loop_vec[1];
	string process_filename;
	vector<string> _suffix = split_string(filename_with_loop_vec[0], ".");
	string save_filename;
	save_filename = file_prefix_without_loop_vec[1] + "_" + out_filename_suffix + _suffix[1];

	dec_single_file data_dec(file_prefix_without_loop_vec[1] , current_file_loop_num, out_filename_suffix + _suffix[1], is_usb_data, save_filename + "_log.txt", is_save_binary);

	process_filename = first_filename;
	data_dec.init_data(process_filename, is_skip_remain_data_loop);
	if (!data_dec.is_file_exist)
	{
		cout << cRED << "File: " << cYELLOW << process_filename << cRED << " does not exist. Please check the file" << cRESET << endl;
		return -1;
	}
	data_dec.data2tree();

	data_dec.complete_adding();
	cout << cGREEN << "Done!" << cRESET << endl
		 << endl;
	return 0;
}

void show_info(char *name)
{
	cout << cCYAN;
	for (int i = 0; i < 15; i++)
	{
		cout << "==";
	}
	cout << endl
		 << "Usage: " << name << " -[parameter] [options]" << endl
		 << "    -f [First data filename. If single file, specific the filename] (required)" << endl
		 << "    -l/-L [Loop number]" << endl
		 << "    -t/-T <Save finary files for not root process>" << endl
		 << "    -n [Filename separator. Default: _mt]" << endl
		 << "    -g/-G <Is data acquired by FELIX GBT board?>" << endl
		 << cRED << "        (Note: USB device is set as default, and the -u/-U args will overwrite this.)" << cRESET
		 << "    -o [Save filename suffix] (Default: out)"
		 << cRESET << endl;
}

vector<string> split_string(const string &str, string delims, int inc)
{
	std::vector<std::string> result;
	size_t i = str.rfind(delims, str.length());
	if (i == str.length() - 1 || i == string::npos)
	{
		result.push_back(str);
	}
	else
	{
		result.push_back(str.substr(i + inc, str.length() - i));
		result.push_back(str.substr(0, i));
	}
	return result;
}

bool is_number(const string &s)
{
	return !s.empty() && find_if(s.begin(),
								 s.end(), [](unsigned char c)
								 { return !isdigit(c); }) == s.end();
}
