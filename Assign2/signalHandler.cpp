#include "signalHandler.hpp"
#include <signal.h>

using namespace std;

void reapProcess(int sig)
{
    // need to write this after pipeline has count of current process
    pid_t currpid; int status;
    while(true)
    {
        currpid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if(currpid <= 0)         
        break;
        Pipes* currpipe = allPipes[pipeIndexMap[currpid]];
        
        if(WIFSIGNALED(status) || WIFEXITED(status))  // Terminated due to interrupt or normal exit
        {
            currpipe->countActive--;
        }
        else if(WIFSTOPPED(status))
        {
            currpipe->countActive--;
        }
        else if(WIFCONTINUED(status))
        { 
            currpipe->countActive++;
        }
        if(currpipe->pgrpID == foregroundPID && !WIFCONTINUED(status) && currpipe->countActive == 0)
        {
            foregroundPID = 0;
            // To allow background process due to ctrl+Z to continue in background
            if(WIFSTOPPED(status))                   
            // killpg(currpipe->pgrpID, SIGCONT);
            currpipe->isBackground = true;
        }
    }
}

// Signal handler function for ctrl+C(SIGINT) & ctrl+Z(SIGTSTP)
void handle_ctrl_CZ(int sig)                            
{
    if(sig == SIGINT)
    {
        if(getpid() != rootpid)
        kill(-getpgrp(), SIGINT);
        else
        {
            printf("\n");
            rl_reset_line_state();
            rl_replace_line("",0);
            rl_redisplay();
        }
    }
    if(sig == SIGTSTP)
    {
        printf("\n");
        rl_reset_line_state();
        rl_replace_line("",0);
        rl_redisplay();
    }
}


// These helper functions handle race condition
// REF : https://web.stanford.edu/class/archive/cs/cs110/cs110.1206/lectures/06-signal-slides.pdf,
// https://web.stanford.edu/class/archive/cs/cs110/cs110.1206/lectures/07-races-and-deadlock-slides.pdf

void toggleSIGCHLDBlock(int how) 
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(how, &mask, NULL);
}


void blockSIGCHLD() 
{
    toggleSIGCHLDBlock(SIG_BLOCK);
}

void unblockSIGCHLD() 
{
    toggleSIGCHLDBlock(SIG_UNBLOCK);
}

// Ensures no race conditions for foreground processes
void waitForForegroundProcess(pid_t pid) 
{
    foregroundPID = pid;
    sigset_t empty;

    sigemptyset(&empty);
    while (foregroundPID == pid) 
    {
        sigsuspend(&empty);
    }
    unblockSIGCHLD();
}
