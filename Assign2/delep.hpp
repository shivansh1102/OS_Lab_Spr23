// header file for delep.cpp

#ifndef DELEP_HPP
#define DELEP_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
using namespace std;

#define BUF_SIZE 20
#define REC_BUF_SIZE 2048
void util_delep(vector<vector<string> > &v);

void kill_given_process(pid_t pid);

void get_from_child(int f);

void signal_handler_for_delep(int signal_number);

void delep(vector<vector<string>> &cmd);

#endif