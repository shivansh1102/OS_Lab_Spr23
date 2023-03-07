#include "sns.hpp"

extern pthread_mutex_t mutexfeedQueue[40000];
extern pthread_mutex_t mutexupdateQueue;
extern pthread_cond_t condfeedQueue;
extern pthread_cond_t condupdateQueue;
extern queue<Action> feedQueue[40000];
extern queue<int> updateQueue;
extern int fileCount;

void* readPosts(void *)
{
    /*
        if the updatequeue is locked, then wait for it to be unlocked
        else if the updatequeue is empty, wait for the broadcast
        else lock the updatequeue and read the update file until the updatequeue is empty and unlock the updatequeue
        if the feedqueue is locked, then wait for it to be unlocked
        else if the feedqueue is empty, wait for the broadcast
        else lock the feedqueue and write the action file until the feedqueue is empty and unlock the feedqueue
    */
    pthread_mutex_lock(&mutexupdateQueue);
    while (updateQueue.empty())
    {
        pthread_cond_wait(&condupdateQueue, &mutexupdateQueue);
    }
    int i = updateQueue.front();
    updateQueue.pop();
    pthread_mutex_unlock(&mutexupdateQueue);

    pthread_mutex_lock(&mutexfeedQueue[i]);
    
    while(1)
    {
        if (feedQueue[i].empty())
            break;

        Action action = feedQueue[i].front();
        feedQueue[i].pop();

        string write_file = "sns.log";

        ofstream outFile(write_file, ios::app);
        if(!outFile.is_open())
        {
            cerr << "Error in opening sns.log file" << endl;
            exit(1);
        }
        outFile << action.user_id << " " << action.action_id << " " << action.action_type << " " << action.timestamp << endl;
        outFile.close();
    }
    pthread_mutex_unlock(&mutexfeedQueue[i]);
}