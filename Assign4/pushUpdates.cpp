#include "pushUpdates.hpp"

void pushOneToFeed(const Action &obj, ofstream& outFile)
{
    int _priority, quIndex;
    for(int currNeigh : nodes[obj.user_id].neigh)
    {
        if(nodes[currNeigh].typeFeed)           // because by default, priority queue is a max heap and we want
        _priority = -obj.global_action_id;      // action which occur later(high globalID) should have less priority
        else
        _priority = nodes[currNeigh].cntMutualFriends[obj.user_id];

        pthread_mutex_lock(&mutexFeedQueue[currNeigh]);
        nodes[currNeigh].feedQueue.push({_priority, obj});
        pthread_cond_signal(&condFeedQueue[currNeigh]);
        pthread_mutex_unlock(&mutexFeedQueue[currNeigh]);

        outFile << "Pushed action- ";
        outFile << obj;
        outFile << " to feedQueue of Node #" << currNeigh << endl;

        quIndex = currNeigh/(MAXNODES/10);
        pthread_mutex_lock(&mutexUpdNodeFeed[quIndex]);
        updNodeFeed[quIndex].push(currNeigh);
        pthread_cond_signal(&condUpdNodeFeed[quIndex]);
        pthread_mutex_unlock(&mutexUpdNodeFeed[quIndex]);
    }                                
}

void* pushUpdates(void* param)    // tidx -> thread index
{
    int tidx = *static_cast<int*>(param);

    ofstream outFile("sns.log");

    if(!outFile.is_open())
    {
        cerr << "Error in opening sns.log" << endl;
        exit(1);
    }

    while(1)
    {
        pthread_mutex_lock(&mutexUpdateQueue);  
        while(updates.empty())
        pthread_cond_wait(&condUpdateQueue, &mutexUpdateQueue);
        Action obj = updates.front();
        updates.pop();
        pthread_mutex_unlock(&mutexUpdateQueue);

        outFile << "Read action from update queue- ";
        outFile << obj << endl;
        pushOneToFeed(obj, outFile);
    }

    pthread_exit(nullptr);
}
