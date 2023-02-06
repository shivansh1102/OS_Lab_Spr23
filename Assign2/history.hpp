#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <deque>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;


string trim_c(string &s);

class historyCls{
    
    int history_size = 1000;
    string filename;
    public:
        deque<string> history;
        historyCls(string filename);
        void addHistory(string line);
        void moveArrowUp(int &i);
        void moveArrowDown(int &i);
        ~historyCls();
};

#endif
