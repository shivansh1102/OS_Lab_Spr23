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
#include <algorithm>
#include <cassert>
using namespace std;

class Action
{
    public :
    int user_id;
    int action_id;                              // Action ID specific to this type of action
    int global_action_id;                       // Action ID equalivalent to all types of actions
    int action_type;                            // 0 -> post, 1 -> comment, 2 -> like
    time_t timestamp;   
    Action(int, int, int, int);
    ~Action();
    Action(const Action &);                     // Copy constructor
    Action& operator= (const Action&);    // Copy assignment operator
    bool operator< (const Action&) const;       // overloading < as it will be used by priority_queue
};  

class Node
{
    public :
    vector<int>neigh;
    int degree;
    priority_queue<pair<int, Action>> feedQueue;
    queue<Action> wallQueue;
    bool typeFeed;                      // 0 - priority based, 1 - chronological
    map<int, int> cntMutualFriends;     // stores count of mutual friend of a node with current node
    Node();
    ~Node();
    void addNeighbour(int);
}; 

extern const int MAXNODES, MAXEDGES;
extern Node* nodes;
extern queue<Action> updates;
extern queue<int> updNodeFeed[];
extern pthread_mutex_t mutexUpdateQueue, mutexFeedQueue[], mutexUpdNodeFeed[];
extern pthread_cond_t condUpdateQueue, condUpdNodeFeed[];
extern pthread_mutex_t filelock, stdoutlock;

extern void* userSimulator(void *);
extern void* pushUpdates(void *);
extern void* readPosts(void *);

#endif