// fec_data_dec.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <TFile.h>
#include <fstream>

#include "dec_single_file.hh"
#include "list_file.hh"
#include "constant.hh"
#include "getopt.hh"

// TODO: Reference additional headers your program requires here.
int c = 0;
extern int optind, opterr, optopt;
extern char* optarg;
void show_info(char* name);
vector<string> split_string(const string& str, string delims, int inc = 0);
bool is_number(const string& s);