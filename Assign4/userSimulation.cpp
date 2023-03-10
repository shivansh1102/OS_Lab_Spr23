#include "userSimulation.hpp"

void* userSimulator(void* param)
{
    pthread_mutex_lock(&filelock);

    ofstream outFile;
    outFile.open("sns.log", ios::app);

    if(!outFile.is_open())
    {
        cerr << "Error in opening sns.log" << endl;
        exit(1);
    }
    pthread_mutex_unlock(&filelock);
    int actionID[3] = {0, 0, 0}; // postID, commentID, likeID

    set<int>distinctNodes;
    int temp, cntActions, actType, iter = 1;
    vector<Action> actions;     // to store all actions generated in one iteration
    while(1)
    {
        pthread_mutex_lock(&filelock);
        outFile << '\n' << "----------------------------------ITERATION #" << iter << "--------------------------------" << '\n' << '\n';
        pthread_mutex_unlock(&filelock);

        pthread_mutex_lock(&stdoutlock);
        cout << '\n' << "----------------------------------ITERATION #" << iter << "--------------------------------" << '\n' << '\n';
        pthread_mutex_unlock(&stdoutlock);

        distinctNodes.clear();
        while(distinctNodes.size() < 100)
        {
            temp = rand()%MAXNODES;
            distinctNodes.insert(temp);
        }  

        actions.clear(); 
        actions.reserve(9000);          // No of actions generated is upper bounded by 9000.
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
            cntActions = 10*(1 + cntActions);

            pthread_mutex_lock(&filelock);
            outFile << '\n' << "For Node #" << node << " with degree = " << nodes[node].degree << ", " << cntActions << " actions generated." << '\n';
            pthread_mutex_unlock(&filelock);
            
            pthread_mutex_lock(&stdoutlock);
            cout << '\n' << "For Node #" << node << " with degree = " << nodes[node].degree << ", " << cntActions << " actions generated." << '\n';
            pthread_mutex_unlock(&stdoutlock);

            while(cntActions--)
            {
                actType = rand()%3;
                ++actionID[actType];
                Action obj(node, actionID[actType], actionID[0] + actionID[1] + actionID[2], actType);
                nodes[node].wallQueue.push(obj);
                actions.push_back(obj);

                pthread_mutex_lock(&filelock);
                outFile << "Generated Action- ";
                outFile << obj << '\n';
                pthread_mutex_unlock(&filelock);

                pthread_mutex_lock(&stdoutlock);
                cout << "Generated Action- ";
                cout << obj << '\n';
                pthread_mutex_unlock(&stdoutlock);
                
            }
        }
        // Now, adding all actions to updates queue
        pthread_mutex_lock(&mutexUpdateQueue);
        for(auto &action : actions)
        {
            updates.push(action);
        }
        pthread_cond_broadcast(&condUpdateQueue);
        pthread_mutex_unlock(&mutexUpdateQueue);
        ++iter;
        sleep(40);
    }

    outFile.close();
    pthread_exit(nullptr);
}