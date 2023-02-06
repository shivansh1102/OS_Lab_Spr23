// #include <stdio.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <dirent.h>
// #include <string.h>
// #include <time.h>
// #include <stdlib.h>
// #include <iostream>
// using namespace std;

// #define MAX_CHILD_PROCESSES 100

// typedef struct Process
// {
//     pid_t pid;
//     pid_t ppid;
//     char name[100];
//     time_t start_time;
//     int num_child_processes;
// } Process;

// // Returns the number of child processes for a given process ID
// int get_num_child_processes(pid_t pid)
// {
//     int count = 0;
//     char path[100];
//     sprintf(path, "/proc/%d/task/%d/children", pid, pid);
//     FILE *fp = fopen(path, "r");
//     if (fp == NULL)
//     {
//         return -1;
//     }
//     int child_pid;
//     while (fscanf(fp, "%d", &child_pid) == 1)
//     {
//         count++;
//     }
//     fclose(fp);
//     return count;
// }

// // Returns the name of a process given its ID
// void get_process_name(pid_t pid, char *name)
// {
//     char path[100];
//     sprintf(path, "/proc/%d/comm", pid);
//     FILE *fp = fopen(path, "r");
//     if (fp == NULL)
//     {
//         strcpy(name, "");
//         return;
//     }
//     fscanf(fp, "%s", name);
//     fclose(fp);
// }

// // Returns the start time of a process given its ID
// time_t get_process_start_time(pid_t pid)
// {
//     char path[100];
//     sprintf(path, "/proc/%d/stat", pid);
//     FILE *fp = fopen(path, "r");
//     if (fp == NULL)
//     {
//         return 0;
//     }
    
//     time_t starttime;
    
//     int i=0;
//     while(!feof(fp)){
//         if(i==21)fscanf(fp, "%ld", &starttime);
//         else fscanf(fp, "%*s");
//     }
//     // fscanf(fp, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %ld %ld %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d",
//     //        &pid2, comm, &state, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags, &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime, &cutime, &cstime, &priority, &nice, &num_threads, &itrealvalue, &starttime, &vsize, &rss, &rsslim, &startcode, &endcode, &startstack, &kstkesp, &kstkeip, &signal, &blocked, &sigignore, &sigcatch, &wchan, &nswap, &cnswap, &exit_signal, &processor);
//     fclose(fp);
//     return starttime / sysconf(_SC_CLK_TCK);
// }

// // Recursively displays the parent, grandparent, ... of a given process
// void display_process_tree(pid_t pid, int depth)
// {
//     char indent[100] = "";
//     for (int i = 0; i < depth; i++)
//     {
//         strcat(indent, " ");
//     }
//     char name[100];
//     get_process_name(pid, name);
//     time_t start_time = get_process_start_time(pid);
//     int num_child_processes = get_num_child_processes(pid);
//     cout<<pid<<" "<<name<<" "<<start_time<<" "<<num_child_processes<<endl;
//     printf("%s%d %s %ld %d\n", indent, pid, name, start_time, num_child_processes);
//     char path[100];
//     sprintf(path, "/proc/%d/task/%d/children", pid, pid);
//     FILE *fp = fopen(path, "r");
//     if (fp == NULL)
//     {
//         return;
//     }
//     int child_pid;
//     while (fscanf(fp, "%d", &child_pid) == 1)
//     {
//         display_process_tree(child_pid, depth + 1);
//     }
//     fclose(fp);
// }

// int main(int argc, char *argv[])
// {
//     if (argc < 2)
//     {
//         printf("Usage: sb <pid> [-suggest]\n");
//         return 1;
//     }
//     pid_t pid = atoi(argv[1]);
//     printf("PID\tNAME\tSTART TIME\tNUM CHILD PROCESSES\n");
//     display_process_tree(pid, 0);
//     // if (argc >= 3 && strcmp(argv[2], "-suggest") == 0) {
//     //  Implement the heuristic to detect the root of all trouble
//     // }
//     return 0;
// }

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;


int main(){

    int x;
    scanf("%d", &x);
    ifstream file;
    string path;
    path = "/proc/"+to_string(x)+"/task/"+to_string(x)+"/children";
    file.open("/proc/");

}