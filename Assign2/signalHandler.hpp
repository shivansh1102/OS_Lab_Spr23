// header file for signalHandlers.cpp

#ifndef __SIGNAL_HANDLERS_H
#define __SIGNAL_HANDLERS_H

#include <sys/wait.h>
#include <map>
#include "pipeline.hpp"
#include <readline/readline.h>

void reapProcess(int sig);
void handle_ctrl_CZ(int sig) ;
void toggleSIGCHLDBlock(int how);
void blockSIGCHLD() ;
void unblockSIGCHLD();
void waitForForegroundProcess(pid_t pid);

extern const pid_t rootpid;
#endif
