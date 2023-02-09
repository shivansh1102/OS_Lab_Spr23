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
string currText = "";

historyCls history;
int i=history.history.size();

int moveUpArrow(int count, int key){
    
    if(i==0){
        return 0;
    }
    if(i==history.history.size()){
        currText = rl_line_buffer;
    }
    history.moveArrowUp(i);
    rl_replace_line(history.history[i].c_str(), 0);
    rl_redisplay();
    rl_end_of_line(count, key);
    return 0;
}

int moveDownArrow(int count, int key){
    int x = history.history.size();
    x -= 1;
    if(i==x){
        rl_replace_line(currText.c_str(), 0);
        rl_redisplay();
        rl_end_of_line(count, key);
        i++;
        return 0;
    }
    if(i>x){
        return 0;
    }

    history.moveArrowDown(i);
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
    rl_bind_keyseq("\001", rl_beg_of_line);
    rl_bind_keyseq("\005", rl_end_of_line);
    char *break_characters = strdup("\t\n\"\\'`@$><=;|&{(");
    rl_completer_word_break_characters = break_characters;



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
        {
            cout << "Bye bhai..." << endl;
            break;
        } 
        string command = input;
        if(command.empty())
        continue;
        Pipes* currpipe = new Pipes(command);
        currpipe->executePipes();
        i = history.history.size();
        free(input);

    }
}
