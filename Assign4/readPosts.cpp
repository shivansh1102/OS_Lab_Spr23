#include "readPosts.hpp"

void readAllUpdates(const int &node, ofstream &outFile, const int &tidx)
{
    
    pthread_mutex_lock(&mutexFeedQueue[node]);
    while(1)
    {
        if(nodes[node].feedQueue.empty())
        return;

        Action obj = nodes[node].feedQueue.top().second;
        nodes[node].feedQueue.pop();
        pthread_mutex_lock(&filelock);
        outFile << "Read-update by thread #" << tidx << " Action-> ";
        outFile << obj << endl;
        pthread_mutex_unlock(&filelock);
    }
    pthread_mutex_unlock(&mutexFeedQueue[node]);
}

void* readPosts(void* param)
{
    int tidx = *static_cast<int*>(param);       // to store index of thread

    ofstream outFile("sns.log");
    if(!outFile.is_open())
    {
        cerr << "Error in opening sns.log" << endl;
        exit(1);
    }

    int node;
    while(1)
    {
        pthread_mutex_lock(&mutexUpdNodeFeed[tidx]);
        while(updNodeFeed[tidx].empty())
        pthread_cond_wait(&condUpdNodeFeed[tidx], &mutexUpdNodeFeed[tidx]);

        node = updNodeFeed[tidx].front();
        updNodeFeed[tidx].pop();
        pthread_mutex_unlock(&mutexUpdNodeFeed[tidx]);

        readAllUpdates(node, outFile, tidx);
    }
}
