#include <signal.h>
#include <readline/readline.h>
#include "history.hpp"
#include "pipeline.hpp"
#include <map>
#include <vector>

using namespace std;

vector<Pipes*> allPipes;
map<pid_t, int>pipeIndexMap;
const pid_t rootpid = getpid();

historyCls history(".history.txt");
int i=history.history.size();

int moveUpArrow(int count, int key){
    
    history.moveArrowUp(i);
    rl_replace_line(history.history[i].c_str(), 0);
    rl_redisplay();
    rl_end_of_line(count, key);
    return 0;
}

int moveDownArrow(int count, int key){
    if(i==history.history.size()-1){
        rl_replace_line("", 0);
        rl_redisplay();
        rl_end_of_line(count, key);
        return 0;
    }
    history.moveArrowDown(i);
    if(i>history.history.size()-1){
        rl_replace_line("", 0);
        rl_redisplay();
        rl_end_of_line(count, key);
        return 0;
    }
    rl_replace_line(history.history[i].c_str(), 0);
    rl_redisplay();
    rl_end_of_line(count, key);
    return 0;
}

void handleCtrlC(int sig) {
    printf("\n");
    rl_reset_line_state();
    rl_replace_line("",0);
    rl_redisplay();
}

int main(){

    rl_bind_keyseq("\\e[A", moveUpArrow);
    rl_bind_keyseq("\\e[B", moveDownArrow);
    signal(SIGINT, handleCtrlC);



    struct sigaction action;
    action.sa_handler = handle_ctrl_CZ;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTSTP, &action, NULL);

    // Reference: https://web.archive.org/web/20170701052127/https://www.usna.edu/Users/cs/aviv/classes/ic221/s16/lab/10/lab.html
    signal(SIGTTOU, SIG_IGN);

    // Reference: https://web.stanford.edu/class/archive/cs/cs110/cs110.1206/lectures/07-races-and-deadlock-slides.pdf
    signal(SIGCHLD, reapProcess);



    while(1){
        string pwd = get_current_dir_name();
        pwd+=" $ ";
        pwd = "BhaiShell:"+pwd;
        char *input = readline(pwd.c_str());
        history.addHistory(input);
        if (strcmp(input, "exit")==0)
            break;
        string command = input;
        if(command.empty())
        continue;
        Pipes* currpipe = new Pipes(command);
        currpipe->executePipes();
        i = history.history.size();
        free(input);

    }
}
