// header file for stringProcessing.cpp

#ifndef STRINGPROCESSING_HPP
#define STRINGPROCESSING_HPP

#include <iostream>
#include <vector>
#include <glob.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
using namespace std;

// function to trim a string
string trim(string &s);

// function to get all pipes
vector<string> getAllPipes(string &command);

// function to handle * and ?
void handleRegex(vector<string>&cmd, string &temp);

// function to get all vectored tokens
vector<vector<string>> getAllVectoredTokens(string &command);

#endif
