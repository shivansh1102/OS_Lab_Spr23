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
// Using this graph representation.
struct edgeData
{
    int to;
    int nxt;
};

int *head;
edgeData* edges;

int currNodes; // to store count of current nodes
int currEdges; // to store count of current edges

extern void solveProducer();
extern void solveConsumer();