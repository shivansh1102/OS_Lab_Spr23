#include "readPosts.hpp"

void readAllUpdates(const int &node, ofstream &outFile, const int &tidx)
{
    pthread_mutex_lock(&mutexFeedQueue[node]);
    int _priority;
    while(1)
    {
        if(nodes[node].feedQueue.empty())
        break;
        Action obj = nodes[node].feedQueue.top().second;
        _priority = nodes[node].feedQueue.top().first;
        nodes[node].feedQueue.pop();
        pthread_mutex_lock(&filelock);
        outFile << "Read-update by thread RP#" << tidx << " for node #" << node << " Action-> ";
        outFile << obj;
        if(_priority > 0) // if priority-based 
        outFile << " with priority - " << _priority;
        outFile << endl;
        pthread_mutex_unlock(&filelock);

        pthread_mutex_lock(&stdoutlock);
        cout << "Read-update by thread RP#" << tidx << " for node #" << node << " Action-> ";
        cout << obj;
        if(_priority > 0) // if priority-based 
        cout << " with priority - " << _priority;
        cout << endl;
        pthread_mutex_unlock(&stdoutlock);
    }
    pthread_mutex_unlock(&mutexFeedQueue[node]);
}

void* readPosts(void* param)
{
    int tidx = *static_cast<int*>(param);       // to store index of thread
    pthread_mutex_lock(&filelock);
    ofstream outFile;
    outFile.open("sns.log", ios::app);
    if(!outFile.is_open())
    {
        cerr << "Error in opening sns.log" << endl;
        exit(1);
    }
    pthread_mutex_unlock(&filelock);

    int node;
    while(1)
    {
        pthread_mutex_lock(&mutexUpdNodeFeed[tidx]);
        while(updNodeFeed[tidx].empty())
        {
            pthread_cond_wait(&condUpdNodeFeed[tidx], &mutexUpdNodeFeed[tidx]);
        }
        node = updNodeFeed[tidx].front();
        updNodeFeed[tidx].pop();
        pthread_mutex_unlock(&mutexUpdNodeFeed[tidx]);

        readAllUpdates(node, outFile, tidx);
    }

    outFile.close();
    pthread_exit(nullptr);
}