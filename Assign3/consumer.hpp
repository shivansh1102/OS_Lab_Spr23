#ifndef __CONSUMER_HPP
#define __CONSUMER_HPP

#include <queue>
#include "main.hpp"

extern struct nodeData *nodes;
extern struct edgeData* edges;

extern char* bufNode;  // pointer to shared memory storing nodes
extern char* bufEdge;  // pointer to shared memory storing edges

extern int *currNodes; // to store count of current nodes
extern int *currEdges; // to store count of current edges

void printShortestPath(const int &node, ofstream& outFile, const vector<int>&dist, const vector<int>& parent);
void multiSourceDijkstra(const vector<int>& sources, const int &iter, ofstream& outFile);
void solveConsumer(int idx);

#endif
