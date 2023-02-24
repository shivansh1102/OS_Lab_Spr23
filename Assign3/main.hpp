#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

// REF : https://codeforces.com/blog/entry/67883
// Using this graph representation with a modification of storing degree of each node also
struct edgeData
{
    int to;
    int nxt;
    edgeData()
    {
        to = -1;
        nxt = -1;
    }
};

struct nodeData
{
    int degree;
    int head;
    nodeData()
    {
        degree = 0;
        head = -1;
    }
};
nodeData *nodes;
edgeData* edges;

char* bufNode;  // pointer to shared memory storing nodes
char* bufEdge;  // pointer to shared memory storing edges

int currNodes; // to store count of current nodes
int currEdges; // to store count of current edges

extern void solveProducer();
extern void solveConsumer();
