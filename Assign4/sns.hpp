#ifndef __SNS_HPP
#define __SNS_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
#include <map>
#include <time.h>
#include <fstream>
#include <cstring>
#include <chrono>
using namespace std;

class Action
{
    public :
    int user_id;
    int action_id;
    int action_type;                    // 0 -> post, 1 -> comment, 2 -> like
    time_t timestamp;   
    Action(int userID, int actionID, int actionType);
    ~Action();
};  

class Node
{
    public :
    vector<int>neigh;
    int degree;
    queue<Action> feedQueue;
    queue<Action> wallQueue;
    bool typeFeed;                      // 0 - priority based, 1 - chronological
    map<int, int> cntMutualFriends;     // stores count of mutual friend of a node with current node
    Node();
    ~Node();
    void addNeighbour(int);
    void updateMutualFriendsNeigh(int);
}; 

extern const int MAXNODES, MAXEDGES;
extern Node* nodes;
extern queue<Action> updates;

extern void* userSimulator(void *);
extern void* pushUpdates(void *);
extern void* readPosts(void *);

#endif
