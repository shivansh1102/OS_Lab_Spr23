#include "main.hpp"

nodeData *nodes;    // to store data of each node
edgeData* edges;    // to store data of each edge

char* bufNode;  // pointer to shared memory storing nodes
char* bufEdge;  // pointer to shared memory storing edges

int *currNodes; // to store count of current nodes
int *currEdges; // to store count of current edges

void addEdge(const int &node1, const int &node2)
{
    // adding directed edge node1->node2
    nodes[node1].degree++;
    edges[*currEdges].to = node2;
    edges[*currEdges].nxt = nodes[node1].head;
    nodes[node1].head = *currEdges;
    *currEdges = (*currEdges) + 1;

    // adding directed edge node2->node1
    nodes[node2].degree++;
    edges[*currEdges].to = node1;
    edges[*currEdges].nxt = nodes[node2].head;
    nodes[node2].head = *currEdges;
    *currEdges = (*currEdges) + 1;
}

void populateGraph()
{
    ifstream file("facebook_combined.txt");

    if(!file.is_open())
    {
        cerr << "Error in opening file" << endl;
        exit(1);
    }

    int node1, node2;
    vector<pair<int, int>> _edges;
    *currNodes = 0;
    while(file >> node1 >> node2)
    {
        _edges.push_back({node1, node2});
        *currNodes = max({*currNodes, 1+node1, 1+node2});
    }
    
    // Allocating memory for nodes[] using placement new operator
    nodeData* temp;
    for(int i = 0; i < *currNodes; i++)
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
    for(int i = 0; i < 2*_edges.size(); i++) // multiplied by 2 because each edge will be stored as 2 directed edge
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

    *currEdges = 0;
    for(auto &edge : _edges)
    {
        addEdge(edge.first, edge.second);
    }

    file.close();
    _edges.clear();
}


int main(int argc, char* argv[])
{
    srand(time(NULL));  // seeding random number generator with current time

    bool flagOptimize = false;
    if(argc > 1)
    {
        if(strcmp(argv[1], "-optimize") == 0)
        flagOptimize = true;
        else
        {
            cout << "Invalid flag!" << endl;
            exit(1);
        }
    }

    const int MAXNODES = 10000, MAXEDGES = 1000000;
    int shmid1; // to store nodes
    int shmid2; // to store edges
    int shmid3; // to store currNodes & currEdges
    
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

    if((shmid3 = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666)) < 0)
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

    currNodes = (int *)shmat(shmid3, NULL, 0);
    if(currNodes == (void*) -1)
    {
        cerr << "Error in shmat" << endl;
        exit(1);
    }
    currEdges = currNodes+1;
    
    // Marking the segment to be destroyed. It will get destroyed only after 
    // count of active processes attached to it become 0.
    // Writing this here(above) helps when we exit through ctrl+c.
    shmctl(shmid1, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
    shmctl(shmid3, IPC_RMID, NULL);

    populateGraph();

    pid_t producerPID = fork();
    if(producerPID == 0)
    {
        solveProducer();
        shmdt(bufNode);
        shmdt(bufEdge);
        shmdt(currNodes);
        exit(0);
    }
    vector<pid_t>consumerPIDs(10);
    for(int i = 0; i < 10; i++)
    {
        consumerPIDs[i] = fork();
        if(consumerPIDs[i] == 0)
        {
            solveConsumer(i, flagOptimize);
            shmdt(bufNode);
            shmdt(bufEdge);
            shmdt(currNodes);
            exit(0);
        }
    }

    waitpid(producerPID, NULL, 0);
    for(int i = 0; i < 10; i++)
    waitpid(consumerPIDs[i], NULL, 0);

    consumerPIDs.clear();
    
    shmdt(bufNode);
    shmdt(bufEdge);
    shmdt(currNodes);

    return 0;
}
