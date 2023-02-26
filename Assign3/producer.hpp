#ifndef __PRODUCER_HPP
#define __PRODUCER_HPP

#include "main.hpp"

extern struct nodeData *nodes;
extern struct edgeData* edges;

extern char* bufNode;  // pointer to shared memory storing nodes
extern char* bufEdge;  // pointer to shared memory storing edges

extern int *currNodes; // to store count of current nodes
extern int *currEdges; // to store count of current edges

void solveProducer();
void allocateNewEdges(const int &);
void allocateNewNodes(const int &);

#endif
