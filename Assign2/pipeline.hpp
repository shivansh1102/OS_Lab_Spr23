// header file for pipeline.cpp

#ifndef __PIPELINE_H
#define __PIPELINE_H

#include "stringProcessing.hpp"
#include "signalHandler.hpp"
#include "trojanDetection.hpp"
#include "delep.hpp"
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
 
class Pipes
{
    public : 
    string fullCmd;                                 // corresponds to complete command
    vector<string> individualCmds;                  // corresponds to individual components commands
    int countActive;                                // stores the count of currently active process
    pid_t pgrpID;                                   // stores the process group id corresponding to individual commands
    bool isBackground;                              // if it is running in background

    Pipes(string &);
    void executePipes();
    bool runIndividualCmd(vector<vector<string>>&, int, int, int);
    ~Pipes();
};

extern vector<Pipes*> allPipes;
extern map<pid_t, int>pipeIndexMap;
static pid_t foregroundPID = 0;

#endif
