#include <signal.h>
#include <readline/readline.h>
#include "history.hpp"
using namespace std;

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

    while(1){
        char *input = readline("rl> ");
        
        history.addHistory(input);
        if (strcmp(input, "exit")==0)
            break;
        i = history.history.size();
        free(input);
    }

    return 0;
}