#include "consumer.hpp"

void printShortestPath(const int &node, ofstream& outFile, const vector<int>&dist, const vector<int>& parent)
{
    outFile << "Node " << node << ": " ;
    int temp = node;
    while(parent[temp] != temp)
    {
        outFile << temp << " -> ";
        temp = parent[temp];
    }

    outFile << temp << endl;
    outFile << "Distance: " << dist[node] << endl;
}

void multiSourceDijkstra(const vector<int>& sources, const int &iter, ofstream& outFile)
{
    const int INF = 1e9;
    int N = (*currNodes);
    vector<int>dist(N, INF);    // to store shortest distance of each node from sources
    vector<int>parent(N);   // to store parent of each node in shortest path
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>pqu;

    for(int source : sources)
    {
        dist[source] = 0;
        pqu.push({0, source});
        parent[source] = source;
    }

    int curr, currdis, neigh; 
    while(!pqu.empty())
    {
        curr = pqu.top().second;
        currdis = pqu.top().first;
        pqu.pop();

        if(dist[curr] < currdis)
        continue;

        for(int currEdgeIndex = nodes[curr].head; currEdgeIndex != -1; currEdgeIndex = edges[currEdgeIndex].nxt)
        {
            neigh = edges[currEdgeIndex].to;
            if(dist[neigh] > dist[curr] + 1)
            {
                dist[neigh] = dist[curr] + 1;
                parent[neigh] = curr;
                pqu.push({dist[neigh], neigh});
            }
        }
    }
    
    outFile << endl <<  "---------------------------------- ITERATION: " << iter << " ----------------------------------" << endl;
    
    for(int i = 0; i < N; i++)
    printShortestPath(i, outFile, dist, parent);

    dist.clear();
    parent.clear();
}

void solveConsumer(int idx)
{
    // Inserting all source nodes corresponding to current consumer in a vector
    int cntSources = (*currNodes)/10;
    vector<int>sources;
    sources.reserve(cntSources);
    
    if(idx < 9)
    {
        for(int i = idx*cntSources; i < (idx+1)*cntSources; i++)
        sources.push_back(i);
    }
    else    // if currNodes is not divisible by 10, then adding all remaining at the end to last consumer
    {
        for(int i = idx*cntSources; i < (*currNodes); i++)
        sources.push_back(i);
    }

    string outFileName = "output";
    outFileName += ((char)('0' + idx));
    outFileName += ".txt";
    
    ofstream outFile(outFileName);

    if(!outFile.is_open())
    {
        cerr << "Error in opening " << outFileName << endl;
        exit(1);
    }

    /*
        ASSUMPTION: 1/10 of New nodes added by producer each time will be added as sources for each consumer
    */
    int iter = 1, prevTotalNodes, newNodes;
    while(1)
    {
        if(iter > 1)    // Adding new nodes to sources  
        {
            newNodes = (*currNodes) - prevTotalNodes;
            if(idx < 9)
            {
                for(int i = idx*newNodes/10; i < (idx+1)*newNodes/10; i++)
                sources.push_back(prevTotalNodes + i);
            }
            else        // if newNodes is not divisible by 10, then adding all remaining at the end to last consumer
            {
                for(int i = idx*newNodes/10; i < newNodes; i++)
                sources.push_back(prevTotalNodes + i);
            }
        }

        multiSourceDijkstra(sources, iter, outFile);
        
        iter++;
        prevTotalNodes = *currNodes;
        sleep(30);
    }

    sources.clear();
    outFile.close();
}
