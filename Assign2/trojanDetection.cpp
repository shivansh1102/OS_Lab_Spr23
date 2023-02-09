#include "trojanDetect.hpp"


// maps pid of a process to its cpu time
map<pid_t, long long>cpuTime;      
// maps pid of a process to count of children in subtree rooted at this node having cpu utilisation >= 95% of cpu util of suspected user process
map<pid_t, long long>cntTrojanSubtree; 

map<pid_t, double>cpuTime_;      
queue<pid_t>q;
map<pid_t, double> before, after;
set<pid_t> pids_;  

pid_t findPPid(pid_t pid)
{
    string word, lastword;
    pid_t ppid = -1;
    char* path = new char[100];
    sprintf(path, "/proc/%d/status", (int)pid);
    ifstream file(path);
    if(file.is_open())
    {
        while(file >> word)
        {
            if(lastword == "PPid:")
            {
                ppid = atoi(word.c_str());
                break;
            }
            lastword = word;
        }
    }
    else
    {
        cerr << "Error in opening /proc/" << pid << "/status" << endl;
        exit(1);
    }

    return ppid;
}
vector<pid_t> findAllChildren(pid_t pid)
{
    char* path = new char[100];
    snprintf(path, 100, "/proc/%d/task/%d/children", (int)pid, (int)pid);
    ifstream file(path);

    string word; vector<pid_t>childpid;
    if(file.is_open())
    {
        while(file >> word)
        {
            childpid.push_back(atoi(word.c_str()));
        }
        file.close();
    }
    else
    {
        cerr << "Error in creating children file" << endl;
        exit(1);
    }
    delete path;
    // cout << childpid.size() << endl;
    return childpid;
}

long long findCPUtime(pid_t pid)
{
    if(cpuTime.count(pid))
    return cpuTime[pid];

    char* path = new char[100];
    sprintf(path, "/proc/%d/stat", (int)pid);

    FILE *fp = fopen(path, "r");
    if(!fp)
    {
        cerr << "Error in opening " << path << endl;
        exit(1);
    }

    long long utime, stime, cutime, cstime, starttime;
    fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %llu %llu %llu %llu %*d %*d %*d %*d %*d %*u %*u %llu", &utime, &stime, &cutime, &cstime, &starttime);
    fclose(fp);
    long long total_time = utime + stime + cutime + cstime;
    return cpuTime[pid] = total_time;
}

long long findCntTrojanSubtree(pid_t parProcess,const pid_t &childDone, int depth)
{
    if(cntTrojanSubtree.count(parProcess))
    return cntTrojanSubtree[parProcess];

    long long cnt = (findCPUtime(parProcess) >= 0.95*findCPUtime(childDone));
    
    if(depth == 0)
    return cntTrojanSubtree[parProcess] = cnt;

    vector<pid_t> children = findAllChildren(parProcess);
    for(auto cpid : children)
    {
        if(cpid != childDone)
        cnt += findCntTrojanSubtree(cpid, childDone, depth-1);
    }
    return cntTrojanSubtree[parProcess] = cnt;
}

pid_t detectTrojan(pid_t suspectedUser)
{
    pid_t rootTrojan = suspectedUser, ppid;
    long long cnt, cntpar; int depth = 1;
    cnt = 1; cntTrojanSubtree[suspectedUser] = 1;
    while(rootTrojan != 1)
    {
        ppid = findPPid(rootTrojan);
        // cout << rootTrojan << " " << ppid << endl;
        cntpar = findCntTrojanSubtree(ppid, rootTrojan, depth) + cnt;
        // cout << cnt << " " << cntpar << endl;
        if(cntpar <= 1.05*cnt)
        break;
        cnt = cntpar;
        rootTrojan = ppid;
        depth++;
    }
    return rootTrojan;
}

void malwareDetect(int pid){
    pid_t rootTrojan = detectTrojan(pid);
    cout<<"Trojan process is: ";
    cout << rootTrojan << endl;
}


vector<pid_t> findAllChildren_(pid_t pid)
{
    int count = 0;
    char path[100];
    vector<pid_t>child;
    sprintf(path, "/proc/%d/task/%d/children", pid, pid);
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
    {
        return {};
    }
    int x;

    while (fscanf(fp, "%d", &x) == 1)
    {
        child.push_back(x);
        count++;
    }
    fclose(fp);
    return child;
}


long long findCPUtime_(pid_t pid)
{
    char* path = new char[100];
    sprintf(path, "/proc/%d/stat", (int)pid);

    FILE *fp = fopen(path, "r");
    if(!fp)
    {
        cerr << "Error in opening " << path << endl;
        exit(1);
    }

    long long utime, stime, cutime, cstime, starttime;
    fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %llu %llu %llu %llu %*d %*d %*d %*d %*d %*u %*u %llu", &utime, &stime, &cutime, &cstime, &starttime);
    fclose(fp);
    long long total_time = utime + stime ;
    return total_time;
}

void getAllParents(pid_t suspectedPid){

    q.push(suspectedPid);
    pids_.insert(suspectedPid);
    pid_t ppid;
    // cout<<suspectedPid<<' ';
    cout<<"Parents of suspected process are:\n";
    while(1){
        ppid = findPPid(suspectedPid);
        if(ppid == 0) break;
        cout<<ppid<<'\n';
        q.push(ppid);
        pids_.insert(ppid);
        suspectedPid = ppid;
    }
    cout<<'\n';
}

long long getCPUJiffles(){
    FILE *fp = fopen("/proc/stat", "r");
    if(!fp)
    {
        cerr << "Error in opening /proc/stat" << endl;
        exit(1);
    }

    long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
    fclose(fp);
    long long total_time = user + nice + system + idle + iowait + irq + softirq + steal;
    return total_time;
}

pid_t getTrojan(pid_t suspectedPid){

    map<pid_t, double> lessTime;

    pid_t child_ = q.front();
    q.pop();

    while(!q.empty()){

        pid_t pid = q.front();
        q.pop();
        vector<pid_t> children = findAllChildren_(pid);
        double itsCpuTime = cpuTime_[pid];
        long cnt=0;
        // cout<<pid<<' '<<itsCpuTime<<' '<<cpuTime[child_]<<'\n';
        if(itsCpuTime <= 0.5*cpuTime_[child_]){
            for(auto child: children){
                double ff=cpuTime_[child];
                // cout<<"H "<<ff<<'\n';
                if(ff >= 0.5*cpuTime_[child_]){
                    cnt++;
                }
            }
        }
        lessTime[pid] = cnt;
    }

    long long maxCnt = 0;
    pid_t maxPid = suspectedPid;
    for(auto it: lessTime){
        if(it.second > maxCnt){
            maxCnt = it.second;
            maxPid = it.first;
        }
        // cout<<it.first<<' '<<it.second<<'\n';
    }

    return maxPid;
}

void getSuggestPID(int suspectedPid){
    for(auto x: pids_){
        before[x] = findCPUtime_(x);
        // cout<<before[x]<<' ';
    }
    // cout<<'\n';
    int total_before = getCPUJiffles();

    sleep(2);

    for(auto x: pids_){
        after[x] = findCPUtime_(x);
        // cout<<after[x]<<' ';
    }
    // cout<<'\n';

    int total_after = getCPUJiffles();
    // cout<<total_after - total_before<<'\n';

    for(auto x: pids_){
        cpuTime_[x] = ((after[x] - before[x])*100.0)/(total_after - total_before);
        // cout<<x<<' '<<cpuTime[x]<<'\n';
    }

    pid_t trojanPid = getTrojan(suspectedPid);
    cout<<"Trojan process is: "<<trojanPid<<'\n';
}

void fasterMalware(int pid, bool flag){

    pid_t suspectedPid = pid;
    getAllParents(suspectedPid);

    if(flag) getSuggestPID(suspectedPid);

    return;
}
