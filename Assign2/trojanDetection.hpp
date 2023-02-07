#include <bits/stdc++.h>
using namespace std;

// maps pid of a process to its cpu time
map<pid_t, long long>cpuTime;      
// maps pid of a process to count of children in subtree rooted at this node having cpu utilisation >= 95% of cpu util of suspected user process
map<pid_t, long long>cntTrojanSubtree;      

pid_t findPPid(pid_t pid);
vector<pid_t> findAllChildren(pid_t pid);
long long findCPUtime(pid_t pid);
long long findCntTrojanSubtree(pid_t parProcess,const pid_t &childDone, int depth);
pid_t detectTrojan(pid_t suspectedUser);
