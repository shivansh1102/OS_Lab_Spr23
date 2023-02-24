#include "main.hpp"

void addEdge(const int &node1, const int &node2)
{
    // adding directed edge node1->node2
    edges[currEdges].to = node2;
    edges[currEdges].nxt = nodes[node1].head;
    nodes[node1].head = currEdges;
    currEdges++;

    // adding directed edge node2->node1
    edges[currEdges].to = node1;
    edges[currEdges].nxt = nodes[node2].head;
    nodes[node2].head = currEdges;
    currEdges++;
}

void populateGraph()
{
    ifstream file("facebook_combined.txt");

    int node1, node2;
    vector<pair<int, int>> _edges;
    currNodes = 0;
    while(file >> node1 >> node2)
    {
        _edges.push_back({node1, node2});
        currNodes = max({currNodes, 1+node1, 1+node2});
    }
    
    // Allocating memory for nodes[] using placement new operator
    nodeData* temp;
    for(int i = 0; i < currNodes; i++)
    {
        if(i == 0)
        {
            nodes = new(bufNode + i*sizeof(nodeData)) nodeData();
            temp = nodes;
        }
        else
        temp = new(bufNode + i*sizeof(nodeData)) nodeData();
        temp++;
    }
    
    edgeData* temp2;
    for(int i = 0; i < _edges.size(); i++)
    {
        if(i == 0)
        {
            edges = new(bufEdge + i*sizeof(edgeData)) edgeData();
            temp2 = edges;
        }
        else
        temp2 = new(bufEdge + i*sizeof(edgeData)) edgeData();
        temp2++;
    }
    
    currEdges = 0;
    for(auto &edge : _edges)
    {
        addEdge(edge.first, edge.second);
    }

    file.close();
}


int main()
{
    const int MAXNODES = 10000, MAXEDGES = 1000000;
    int shmid1, shmid2;
    if((shmid1 = shmget(IPC_PRIVATE, MAXNODES*sizeof(nodeData), IPC_CREAT | 0666)) < 0)
    {
        cerr << "Error in shmget" << endl;
        exit(1);
    }

    if((shmid2 = shmget(IPC_PRIVATE, MAXEDGES*sizeof(edgeData), IPC_CREAT | 0666)) < 0)
    {
        cerr << "Error in shmget" << endl;
        exit(1);
    }

    bufNode = (char*)shmat(shmid1, NULL, 0);
    if(bufNode == (void*) -1)
    {
        cerr << "Error in shmat" << endl;
        exit(1);
    }

    bufEdge = (char*)shmat(shmid2, NULL, 0);
    if(bufEdge == (void*) -1)
    {
        cerr << "Error in shmat" << endl;
        exit(1);
    }
    
    populateGraph();

    pid_t producerPID = fork();
    if(producerPID == 0)
    {
        solveProducer();
        exit(0);
    }
    vector<pid_t>consumerPIDs(10);
    for(int i = 0; i < 10; i++)
    {
        consumerPIDs[i] = fork();
        if(consumerPIDs[i] == 0)
        {
            solveConsumer();
            exit(0);
        }
    }

    waitpid(producerPID, NULL, 0);
    for(int i = 0; i < 10; i++)
    waitpid(consumerPIDs[i], NULL, 0);

    return 0;
}
