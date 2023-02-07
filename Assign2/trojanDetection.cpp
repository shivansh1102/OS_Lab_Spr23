#include "trojanDetection.hpp"

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
        cout << rootTrojan << " " << ppid << endl;
        cntpar = findCntTrojanSubtree(ppid, rootTrojan, depth) + cnt;
        cout << cnt << " " << cntpar << endl;
        if(cntpar <= 1.05*cnt)
        break;
        cnt = cntpar;
        rootTrojan = ppid;
        depth++;
    }
    return rootTrojan;
}

int main(int argc, char* argv[])
{
    if(argc == 1)
    {
        cout << "pid daalo bsdk" << endl;
        exit(1);
    }
    int suspectedUser = atoi(argv[1]);
    cout << detectTrojan(suspectedUser) << endl;
    // cout << findPPid(suspectedUser) << endl;
    // findAllChildren(suspectedUser);

    return 0;
}