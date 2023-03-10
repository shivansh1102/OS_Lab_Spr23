#include "pushUpdates.hpp"

int findCntMutualFriend(Node* node1, Node* node2)
{
    int i = 0, j = 0, count = 0;
    while(i < node1->neigh.size() && j < node2->neigh.size()){
        if(node1->neigh[i] == node2->neigh[j]){
            count++;
            i++;
            j++;
        }
        else if(node1->neigh[i] < node2->neigh[j]){
            i++;
        }
        else{
            j++;
        }
    }
    return count;
}

void pushOneToFeed(const Action &obj, ofstream& outFile, const int& tidx)
{
    int _priority, quIndex;
    for(int currNeigh : nodes[obj.user_id].neigh)
    {
        if(nodes[currNeigh].typeFeed)           // because by default, priority queue is a max heap and we want
        _priority = -obj.global_action_id;      // action which occur later(high globalID) should have less priority
        else
        {
            if(nodes[obj.user_id].cntMutualFriends.count(currNeigh) == 0)
            {
                nodes[obj.user_id].cntMutualFriends[currNeigh] = findCntMutualFriend(&nodes[obj.user_id], &nodes[currNeigh]);
                nodes[currNeigh].cntMutualFriends[obj.user_id] = nodes[obj.user_id].cntMutualFriends[currNeigh];
            }
            _priority = nodes[currNeigh].cntMutualFriends[obj.user_id];
        }

        pthread_mutex_lock(&filelock);
        outFile << "Thread PU#" << tidx << ": Pushed action- ";
        outFile << obj;
        outFile << " to feedQueue of Node #" << currNeigh << endl;
        pthread_mutex_unlock(&filelock);

        pthread_mutex_lock(&stdoutlock);
        cout << "Thread PU#" << tidx << ": Pushed action- ";
        cout << obj;
        cout << " to feedQueue of Node #" << currNeigh << endl;
        pthread_mutex_unlock(&stdoutlock);

        pthread_mutex_lock(&mutexFeedQueue[currNeigh]);
        nodes[currNeigh].feedQueue.push({_priority, obj});
        pthread_mutex_unlock(&mutexFeedQueue[currNeigh]);

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

    pthread_mutex_lock(&filelock);
    ofstream outFile;
    outFile.open("sns.log", ios::app);

    if(!outFile.is_open())
    {
        cerr << "Error in opening sns.log" << endl;
        exit(1);
    }
    pthread_mutex_unlock(&filelock);

    while(1)
    {
        pthread_mutex_lock(&mutexUpdateQueue);  
        while(updates.empty())
        pthread_cond_wait(&condUpdateQueue, &mutexUpdateQueue);
        Action obj = updates.front();
        updates.pop();
        pthread_mutex_unlock(&mutexUpdateQueue);

        pthread_mutex_lock(&filelock);
        outFile << "Thread PU#" << tidx << ": Read action from updates queue- ";
        outFile << obj << endl;
        pthread_mutex_unlock(&filelock);

        pthread_mutex_lock(&stdoutlock);
        cout << "Thread PU#" << tidx << ": Read action from updates queue- ";
        cout << obj << endl;
        pthread_mutex_unlock(&stdoutlock);

        pushOneToFeed(obj, outFile, tidx);
    }

    outFile.close();
    pthread_exit(nullptr);
}