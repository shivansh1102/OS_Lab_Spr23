#include "producer.hpp"

void allocateNewNodes(const int &newNodes)
{
    // Allocating memory for nodes[] using placement new operator
    nodeData* temp=nodes+(*currNodes);
    for(int i = 0; i < newNodes; i++)
    {
        temp = new(bufNode + ((*currNodes)+i)*sizeof(nodeData)) nodeData();
        temp++;
    }
}

void allocateNewEdges(const int &newEdges)
{
    // Allocating memory for edges[] using placement new operator
    edgeData* temp = edges+(*currEdges);
    for(int i = 0; i < 2*newEdges; i++)     // multiplied by 2 because each edge will be stored as 2 directed edge
    {
        temp = new(bufEdge + ((*currEdges)+i)*sizeof(edgeData)) edgeData();
        temp++;
    }
}

void solveProducer()
{
    // Handling probability for choosing a node while adding a new edge
    // Generate a random number between 1 & sum of degrees
    // If it is between 1 & degree[0], match to node 0
    // Else if between degree[0]+1 & degree[0]+degree[1], match to node 1
    // and so on...

    vector<int>prefixDegreeSum((*currNodes));
    for(int i = 0; i < (*currNodes); i++)
    prefixDegreeSum[i] = nodes[i].degree + ((i > 0) ? prefixDegreeSum[i-1] : 0);

    int newNodes, newEdges, otherNode, temp;
    while(1)
    {
        sleep(50);
        newNodes = rand()%21 + 10;  // [0-20] + 10 = [10-30]
        allocateNewNodes(newNodes);
        
        for(int i = 0; i < newNodes; i++)
        {
            newEdges = rand()%20+1; // [0-19] + 1 = [1-20]
            allocateNewEdges(newEdges);

            for(int j = 0; j < newEdges; j++)
            {
                temp = rand()%prefixDegreeSum.back() + 1;
                otherNode = lower_bound(prefixDegreeSum.begin(), prefixDegreeSum.end(), temp) - prefixDegreeSum.begin();
                addEdge((*currNodes)+i, otherNode); 

                // cout << (*currNodes)+i << " - " << otherNode << endl;
                // Now, as degree of otherNode has increased, updating prefixDegreeSum[] accordingly
                for(int k = otherNode; k < prefixDegreeSum.size(); k++)
                prefixDegreeSum[k]++;
            }

            prefixDegreeSum.push_back(prefixDegreeSum.back()+newEdges); // Adding degree of newly added node
        }
        *currNodes = (*currNodes) + newNodes;
    }
}
