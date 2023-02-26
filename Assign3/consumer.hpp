#ifndef __CONSUMER_HPP
#define __CONSUMER_HPP

#include <queue>
#include <chrono>
#include "main.hpp"

extern struct nodeData *nodes;
extern struct edgeData* edges;

extern char* bufNode;  // pointer to shared memory storing nodes
extern char* bufEdge;  // pointer to shared memory storing edges

extern int *currNodes; // to store count of current nodes
extern int *currEdges; // to store count of current edges

void printShortestPath(const int &, ofstream&, const vector<int>&, const vector<int>&);
void multiSourceDijkstra(const int &, const vector<int>&, const int &, ofstream&, const bool&);
void solveConsumer(int, const bool&);
int optimizedSP(const int&, const vector<int>&, const vector<int>&, const int&, const int &, ofstream &);

#endif
