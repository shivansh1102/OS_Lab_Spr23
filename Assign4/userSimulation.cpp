#include "userSimulation.hpp"

void* userSimulator(void * param)
{
    ofstream outFile("sns.log");

    if(!outFile.is_open())
    {
        cerr << "Error in opening sns.log" << endl;
        exit(1);
    }

    static int actionID[3] = {0, 0, 0}; // postID, commentID, likeID

    set<int>distinctNodes;
    int temp, cntActions, actType;
    while(1)
    {
        distinctNodes.clear();
        while(distinctNodes.size() < 100)
        {
            temp = rand()%MAXNODES;
            distinctNodes.insert(temp);
        }    

        for(auto &node : distinctNodes)
        {
            // counting no of bits in degree to find log2(degree)
            cntActions = 0;
            temp = nodes[node].degree;
            while(temp > 1)         
            {
                cntActions++;
                temp >>= 1;
            }
            
            pthread_mutex_lock(&filelock);
            outFile << endl << "For Node #" << node << " with degree = " << nodes[node].degree << ", " << cntActions << " actions generated." << endl;
            pthread_mutex_unlock(&filelock);

            pthread_mutex_lock(&mutexUpdateQueue);
            while(cntActions--)
            {
                actType = rand()%3;
                ++actionID[actType];
                Action obj(node, actionID[actType], actionID[0] + actionID[1] + actionID[2], actType);
                nodes[node].wallQueue.push(obj);
                pthread_mutex_lock(&filelock);
                outFile << "Generated Action- ";
                outFile << obj << endl;
                pthread_mutex_unlock(&filelock);
                
                updates.push(obj);
            }
            pthread_cond_signal(&condUpdateQueue);
            pthread_mutex_unlock(&mutexUpdateQueue);
        }

        sleep(120);
    }

    outFile.close();
    pthread_exit(nullptr);
}
