#include "consumer.hpp"

const int INF = 1e9;
vector<int>dist;    // to store shortest distance of each node from sources
vector<int>parent;  // to store parent of each node in shortest path

void printShortestPath(const int &node, ofstream& outFile)
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

void multiSourceDijkstra(const int &idx, const vector<int>& sources, const int &iter, ofstream& outFile, const bool& flagOptimize)
{
    auto startTime = chrono::high_resolution_clock().now();

    int N = (*currNodes);
    dist.assign(N, INF);
    parent.assign(N, -1);
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
    
    auto endTime = chrono::high_resolution_clock().now();
    auto timeTaken = chrono::duration_cast<chrono::microseconds>(endTime-startTime);
    cout << "Process#" << idx << " Iteration#" << iter << ": " << timeTaken.count() << endl;

    outFile << endl <<  "---------------------------------- ITERATION: " << iter << " ----------------------------------" << endl;
    
    for(int i = 0; i < N; i++)
    printShortestPath(i, outFile);

    if(!flagOptimize)
    {
        dist.clear();
        parent.clear();
    }
}

void solveConsumer(int idx, const bool &flagOptimize)
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
    vector<int>newSources; int sumTime = 0;
    while(1)
    {
        if(iter > 1)    // Adding new nodes to sources  
        {
            newNodes = (*currNodes) - prevTotalNodes;
            if(idx < 9)
            {
                for(int i = idx*newNodes/10; i < (idx+1)*newNodes/10; i++)
                {
                    sources.push_back(prevTotalNodes + i);
                    if(flagOptimize)
                    newSources.push_back(prevTotalNodes + i);
                }
            }
            else        // if newNodes is not divisible by 10, then adding all remaining at the end to last consumer
            {
                for(int i = idx*newNodes/10; i < newNodes; i++)
                {
                    sources.push_back(prevTotalNodes + i);
                    if(flagOptimize)
                    newSources.push_back(prevTotalNodes + i);
                }
            }
        }
        if(flagOptimize && iter > 1)
        sumTime += optimizedSP(idx, sources, newSources, newNodes, iter, outFile);
        else
        multiSourceDijkstra(idx, sources, iter, outFile, flagOptimize);
        
        if(flagOptimize && iter > 1 && (iter-1)%10 == 0)
        {
            cout << "Avg time by Process#" << idx << " in first " << iter-1 << " iterations = " << sumTime/(iter-1) << endl;
        }

        iter++;
        prevTotalNodes = *currNodes;
        newSources.clear();
        sleep(30);
    }

    sources.clear();
    outFile.close();
}

// returns time taken to execute the code
int optimizedSP(const int& idx, const vector<int>& sources, const vector<int>& newSources, const int& newNodes, const int &iter, ofstream &outFile)
{
    auto startTime = chrono::high_resolution_clock().now();

    int N = (*currNodes);

    // First adding dist[] values of all new nodes as INF
    for(int i = 0; i < newNodes; i++)
    {
        dist.push_back(INF);
        parent.push_back(-1);
    }

    queue<int>qu;
    // Now, making dist[] values of new source nodes as 0
    for(auto newSource : newSources)
    {
        dist[newSource] = 0;
        parent[newSource] = newSource;
        qu.push(newSource);
    }
    // Now, running a PRUNED MULTISOURCE BFS from new source nodes to propogate their effect
    int curr, neigh, currdis;
    
    while(!qu.empty())
    {
        curr = qu.front();
        qu.pop();
        for(int currEdgeIndex = nodes[curr].head; currEdgeIndex != -1; currEdgeIndex = edges[currEdgeIndex].nxt)
        {
            neigh = edges[currEdgeIndex].to;
            if(dist[neigh] > dist[curr] + 1)
            {
                dist[neigh] = dist[curr] + 1;
                parent[neigh] = curr;
                qu.push(neigh);
            }
        }
    }  

    bool updated;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>pqu;
    
    // Now, updating the values of remaining new nodes added in this iteration
    // As dist[] values for all nodes other than newly added nodes are final,
    // dist[] values for newly added non-source nodes can be found by 1-hop distance, ie, just iterating over its neighbours
    // Also, processing in order the nodes were added, so that we can consider edges between newly added nodes
    // When we are considering ith newly added node, dist[] values for 1st, 2nd, ... , (i-1)th node are final
    for(int i = N-newNodes; i < N; i++)
    {
        for(int currEdgeIndex = nodes[i].head; currEdgeIndex != -1; currEdgeIndex = edges[currEdgeIndex].nxt)
        {
            neigh = edges[currEdgeIndex].to;
            if(dist[i] > 1 + dist[neigh])
            {
                dist[i] = 1 + dist[neigh];
                parent[i] = neigh;
            }
        }

        // Now, consider this node had neighbours a, b, c, d
        // We should consider change in them also due to addition of this node
        for(int currEdgeIndex = nodes[i].head; currEdgeIndex != -1; currEdgeIndex = edges[currEdgeIndex].nxt)
        {
            neigh = edges[currEdgeIndex].to;
            if(dist[neigh] > 1 + dist[i])
            {
                dist[neigh] = 1 + dist[i];
                parent[neigh] = i;
            }
        }
    }

    // Now, we need to propogate the updation in neighbours of those nodes
    // whose dist[] values got updated.
    // So, running an algorithm similar to PRUNED MULTISOURCE DIJKSTRA

    while(!pqu.empty())
    {
        curr = pqu.top().second;
        currdis = pqu.top().first;
        pqu.pop();

        if(currdis > dist[curr])
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

    auto endTime = chrono::high_resolution_clock().now();
    auto timeTaken = chrono::duration_cast<chrono::microseconds>(endTime-startTime);
    cout << "Process#" << idx << " Iteration#" << iter << ": " << timeTaken.count() << endl;

    outFile << endl <<  "---------------------------------- ITERATION: " << iter << " ----------------------------------" << endl;
    
    for(int i = 0; i < N; i++)
    printShortestPath(i, outFile);

    return (int)timeTaken.count();
}
