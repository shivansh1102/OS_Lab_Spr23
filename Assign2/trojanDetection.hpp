#ifndef __TROJANDETECTION_HPP
#define __TROJANDETECTION_HPP

#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <set>
#include <fstream>
#include <unistd.h>
using namespace std;


pid_t findPPid(pid_t pid);
vector<pid_t> findAllChildren(pid_t pid);
long long findCPUtime(pid_t pid);
long long findCntTrojanSubtree(pid_t parProcess,const pid_t &childDone, int depth);
pid_t detectTrojan(pid_t suspectedUser);
void malwareDetect(int pid);

vector<pid_t> findAllChildren_(pid_t pid);
long long findCPUtime_(pid_t pid);
void getAllParents(pid_t suspectedPid);
long long getCPUJiffles();
pid_t getTrojan(pid_t suspectedPid);
void fasterMalware(int pid, bool flag);

#endif
