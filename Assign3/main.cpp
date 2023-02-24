#include "main.hpp"

void addEdge(const int &node1, const int &node2)
{
    // adding directed edge node1->node2
    edges[currEdges].to = node2;
    edges[currEdges].nxt = head[node1];
    head[node1] = currEdges;
    currEdges++;

    // adding directed edge node2->node1
    edges[currEdges].to = node1;
    edges[currEdges].nxt = head[node2];
    head[node2] = currEdges;
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

    // Initialising head[i] to -1 
    for(int i = 0; i < currNodes; i++)
    head[i] = -1;

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
    if((shmid1 = shmget(IPC_PRIVATE, MAXNODES*sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        cerr << "Error in shmget" << endl;
        exit(1);
    }

    if((shmid2 = shmget(IPC_PRIVATE, MAXEDGES*sizeof(edgeData), IPC_CREAT | 0666)) < 0)
    {
        cerr << "Error in shmget" << endl;
        exit(1);
    }

    head = (int*)shmat(shmid1, NULL, 0);
    if(head == (void*) -1)
    {
        cerr << "Error in shmat" << endl;
        exit(1);
    }

    edges = (edgeData*)shmat(shmid2, NULL, 0);
    if(edges == (void*) -1)
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
