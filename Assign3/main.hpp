#ifndef __MAIN_HPP
#define __MAIN_HPP

#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>

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

extern void solveProducer();
extern void solveConsumer(int, const bool&);

void addEdge(const int &, const int &);
void populateGraph();


#endif
