#ifndef __PIPELINE_H
#define __PIPELINE_H

#include "stringProcessing.hpp"
#include "signalHandler.hpp"

class Pipe
{
    public : 
    string fullCmd;                                 // corresponds to complete command
    vector<string> individualCmds;                  // corresponds to individual components commands
    int countActive;                                // stores the count of currently active process
    pid_t pgrpID;                                   // stores the process group id corresponding to individual commands
    bool isBackground;                              // if it is running in background

    Pipe(string &);
    void executePipe();
    void runIndividualCmd(vector<vector<string>>&, int, int, int);
    ~Pipe();
};

#endif