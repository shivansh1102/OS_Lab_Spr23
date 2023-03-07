#include "sns.hpp" 

Node::Node() : degree(0), typeFeed(rand()%2) {}

Node::~Node()
{
    neigh.clear();
    cntMutualFriends.clear();
    while(!feedQueue.empty())
    feedQueue.pop();
    while(!wallQueue.empty())
    wallQueue.pop();
}

void Node::addNeighbour(int node)
{
    neigh.push_back(node);
    degree++;
}

void Node::updateMutualFriendsNeigh()
{
    // Adding contribution of each node, ie
    // Consider ith node, for all pairs of its neighbours, (x,y)
    // We will add 1 to indicate both x & y have a mutual friend which is "i"
    for(int i = 0; i < neigh.size(); i++)
    {
        for(int j = i+1; j < neigh.size(); j++)
        {
            nodes[neigh[i]].cntMutualFriends[neigh[j]]++;
            nodes[neigh[j]].cntMutualFriends[neigh[i]]++;
        }
    }
}

Action::Action(int userID = 0, int actionID = 0, int actionType = 0) : user_id(userID), action_id(actionID), action_type(actionType)
{
    time(&timestamp);
}

Action::~Action()
{
    cout << "Deleting action - user_id:" << user_id << " action_id:" << action_id << " action_type:" << action_type << " timestamp:" << timestamp << endl;
}

int main()
{
    srand(time(nullptr));                  // seeding with current time
    
    string temp, temp2; 
    ifstream inFile("musae_git_edges.csv");

    if(!inFile.is_open())
    {
        cerr << "Error in opening csv file" << endl;
        exit(1);
    }

    inFile >> temp;                        // first row has coloums in csv

    int node1, node2;
    while(inFile >> temp)
    {
        temp2.clear();
        for(int i = 0; i < temp.size(); i++)
        {
            if(temp[i] == ',')
            {
                node1 = atoi(temp2.c_str());
                node2 = atoi(temp.substr(i+1).c_str());
                break;
            }
            temp2 += temp[i];
        }
        nodes[node1].addNeighbour(node2);
        nodes[node2].addNeighbour(node1);
    }   
    inFile.close();

    // auto startTime = chrono::high_resolution_clock().now();
    
    // // Now, computing mutual friends for all nodes
    // for(int i = 0; i < MAXNODES; i++)
    // nodes[i].updateMutualFriendsNeigh();

    // auto endTime = chrono::high_resolution_clock().now();
    // auto timeTaken = chrono::duration_cast<chrono::microseconds>(endTime-startTime);
    // cout << timeTaken.count() << endl;

    // for(int i = 0; i < MAXNODES; i++)
    // {
    //     for(int j = 0; j < MAXNODES; j++)
    //     cout << i << " " << j << " -> " << nodes[i].cntMutualFriends[j] << endl;
    // }

    pthread_t userSimTID, readPostTID[10], pushUpdTID[25];
    pthread_attr_t usattr, rpattr[10], pdattr[25];
    // Initialising thread attributes with default values
    pthread_attr_init(&usattr);
    for(int i = 0; i < 10; i++)
    pthread_attr_init(&rpattr[i]);
    for(int i = 0; i < 25; i++)
    pthread_attr_init(&pdattr[i]);

    // Creating threads
    pthread_create(&userSimTID, &usattr, userSimulator, nullptr);
    for(int i = 0; i < 10; i++)
    pthread_create(&readPostTID[i], &rpattr[i], readPosts, nullptr);
    for(int i = 0; i < 25; i++)
    pthread_create(&pushUpdTID[i], &pdattr[i], pushUpdates, nullptr);

    // Waiting for threads    
    pthread_join(userSimTID, nullptr);
    for(int i = 0; i < 10; i++)
    pthread_join(readPostTID[i], nullptr);
    for(int i = 0; i < 25; i++)
    pthread_join(pushUpdTID[i], nullptr);

    delete[] nodes;
    return 0;
}
