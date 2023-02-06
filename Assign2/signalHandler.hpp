#ifndef __SIGNAL_HANDLERS_H
#define __SIGNAL_HANDLERS_H

#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <map>
#include "pipeline.hpp"
#include <readline/readline.h>
// #include "bhaishell.cpp"

void reapProcess(int sig);
void handle_ctrl_CZ(int sig) ;
void toggleSIGCHLDBlock(int how);
void blockSIGCHLD() ;
void unblockSIGCHLD();
void waitForForegroundProcess(pid_t pid);

extern const pid_t rootpid;
#endif
