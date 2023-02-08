#include "history.hpp"

string trim_c(string &s){
    string temp = "";
    int i = 0;
    int j = s.size() - 1;
    for (i = 0; i < s.length(); ++i)
    {
        if (s[i] != ' ' && s[i]!=0x03 && s[i]!='\n')
            break;
    }
    for (j = s.length() - 1; j >= 0; --j)
    {
        if (s[j] != ' ' && s[j]!=0x03 && s[j]!='\n')
            break;
    }
    return s.substr(i, j - i + 1);
}

historyCls::historyCls(string filename):filename(filename){
    ifstream fin(filename);
    string line;
    while (getline(fin, line))
    {
        history.push_back(line);
    }
    fin.close();
}

historyCls::~historyCls(){
    ofstream fout(filename);
    for (auto line : history)
    {
        fout << line << endl;
    }
    fout.close();
}

void historyCls::addHistory(string line){
    if (history.size() == history_size)
    {
        history.pop_front();
    }
    if(trim_c(line)=="\n" || trim_c(line)==""){
        return;
    }
    history.push_back(line);
}

void historyCls::moveArrowUp(int &i){

    if(i<=0){
        return;
    }
    i--;
}

void historyCls::moveArrowDown(int &i){

    if(i>=history.size()-1){
        return;
    }
    i++;
}
