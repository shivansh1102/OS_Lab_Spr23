#ifndef __SIGNAL_HANDLERS_H
#define __SIGNAL_HANDLERS_H

#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "pipeline.hpp"

extern bool pressedCtrlC, pressedCtrlZ;
extern pid_t foregroundPID;
extern vector<Pipe*> allPipes;
extern map<pid_t, int*>pipeIndexMap;

void reapProcess(int sig);
void handle_ctrl_CZ(int sig) ;
void toggleSIGCHLDBlock(int how);
void blockSIGCHLD() ;
void unblockSIGCHLD();
void waitForForegroundProcess(pid_t pid);

#endif