#include "producer.hpp"

void allocateNewNodes(const int &newNodes)
{
    // Allocating memory for nodes[] using placement new operator
    nodeData* temp=nodes+currNodes;
    for(int i = 0; i < newNodes; i++)
    {
        temp = new(bufNode + (currNodes+i)*sizeof(nodeData)) nodeData();
        temp++;
    }
}

void allocateNewEdges(const int &newEdges)
{
    edgeData* temp = edges+currEdges;
    for(int i = 0; i < 2*newEdges; i++)     // multiplied by 2 because each edge will be stored as 2 directed edge
    {
        temp = new(bufEdge + (currEdges+i)*sizeof(edgeData)) edgeData();
        temp++;
    }
}

void solveProducer()
{
    set<pair<int, int>> degreeNode;
    // Adding (degree, node) to a set for finding popular nodes later
    for(int i = 0; i < currNodes; i++)
    {
        cout << i << " " << nodes[i].degree << endl;
        degreeNode.insert({nodes[i].degree, i});
    }

    int newNodes, newEdges;
    while(1)
    {
        sleep(50);
        newNodes = rand()%21 + 10;  // [0-20] + 10 = [10-30]
        allocateNewNodes(newNodes);
        
        for(int i = 0; i < newNodes; i++)
        {
            newEdges = rand()%20+1; // [0-19] + 1 = [1-20]
            allocateNewEdges(newEdges);

            // Adding most popular nodes
            auto it = degreeNode.end(); it--;
            for(int j = 0; j < newEdges; j++)
            {
                addEdge(currNodes+i, it->second); // it->second stores the node number
                it--;
            }

            degreeNode.insert({newEdges, currNodes+i});
        }
        currNodes += newNodes;
    }
}
