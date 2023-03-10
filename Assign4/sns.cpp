#include "sns.hpp" 

const int MAXNODES = 37700, MAXEDGES = 289003;
Node* nodes = new Node[MAXNODES];
queue<Action> updates;      // to store updates between user-simulator and push-update
queue<int> updNodeFeed[10];     // to store node no. whose feed queue got updated

pthread_mutex_t mutexUpdateQueue, mutexFeedQueue[MAXNODES], mutexUpdNodeFeed[10];
pthread_cond_t condUpdateQueue = PTHREAD_COND_INITIALIZER, condUpdNodeFeed[10];

pthread_mutex_t filelock, stdoutlock;

string actionTypes[] = {"Post", "Comment", "Like"};

Node::Node() : degree(0), typeFeed(rand()%2) {}

Node::~Node()
{
    neigh.clear();
    cntMutualFriends.clear();
    while(!feedQueue.empty())
    feedQueue.pop();
    while(!wallQueue.empty())
    wallQueue.pop();
}

void Node::addNeighbour(int node)
{
    neigh.push_back(node);
    degree++;
}

bool Action::operator < (const Action& other) const     // Writing "const" is a must here as internally priority_queue implementation needs it for const objects.
{
    return global_action_id < other.global_action_id;
}

Action::Action(int userID = 0, int actionID = 0, int globalID = 0, int actionType = 0) : user_id(userID), action_id(actionID), global_action_id(globalID), action_type(actionType)
{
    time(&timestamp);
}

Action::~Action() {}

Action::Action(const Action& obj) : user_id(obj.user_id), action_id(obj.action_id), global_action_id(obj.global_action_id), action_type(obj.action_type), timestamp(obj.timestamp) {}

Action& Action::operator= (const Action &obj)
{
    if(this != &obj)
    {
        user_id = obj.user_id;
        action_id = obj.action_id;
        action_type = obj.action_type;
        global_action_id =obj.global_action_id;
        timestamp = obj.timestamp;
    }
    return *this;
}

ofstream& operator << (ofstream& outFile, const Action& obj)
{
    outFile << "user_id:" << obj.user_id << " action_id:" << obj.action_id << " action_type:" << actionTypes[obj.action_type] << " timestamp:" << obj.timestamp;
    return outFile;
}

ostream& operator << (ostream& outFile, const Action& obj)
{
    assert(obj.action_type >= 0 && obj.action_type < 3);
    cout << "user_id:" << obj.user_id << " action_id:" << obj.action_id << " action_type:" << actionTypes[obj.action_type] << " timestamp:" << obj.timestamp;
    return outFile;
}

int main()
{
    srand(time(nullptr));                  // seeding with current time
    
    string temp, temp2; 
    ifstream inFile("musae_git_edges.csv");

    if(!inFile.is_open())
    {
        cerr << "Error in opening csv file" << endl;
        exit(1);
    }

    inFile >> temp;                        // first row has coloums in csv

    int node1, node2;
    while(inFile >> temp)
    {
        temp2.clear();
        for(int i = 0; i < temp.size(); i++)
        {
            if(temp[i] == ',')
            {
                node1 = atoi(temp2.c_str());
                node2 = atoi(temp.substr(i+1).c_str());
                break;
            }
            temp2 += temp[i];
        }
        nodes[node1].addNeighbour(node2);
        nodes[node2].addNeighbour(node1);
    }   
    inFile.close();

    for(int i = 0; i < MAXNODES; i++)
    sort(nodes[i].neigh.begin(), nodes[i].neigh.end());

    if(pthread_mutex_init(&mutexUpdateQueue, NULL) != 0)
    {
        cerr << "Error in initialising mutexUpdateQueue" << endl;
        exit(1);
    }
    if(pthread_mutex_init(&filelock, NULL) != 0)
    {
        cerr << "Error in initialising file lock" << endl;
        exit(1);
    }
    if(pthread_mutex_init(&stdoutlock, NULL) != 0)
    {
        cerr << "Error in initialising stdout lock" << endl;
        exit(1);
    }

    for(int i = 0; i < MAXNODES; i++)
    {
        if(pthread_mutex_init(&mutexFeedQueue[i], NULL) != 0)
        {
            cerr << "Error in initialising mutexFeedQueue #" << i << endl;
            exit(1);
        }
    }

    for(int i = 0; i < 10; i++)
    {
        if(pthread_mutex_init(&mutexUpdNodeFeed[i], NULL) != 0)
        {
            cerr << "Error in initialising mutexFeedQueue #" << i << endl;
            exit(1);
        }
        condUpdNodeFeed[i] = PTHREAD_COND_INITIALIZER;
    }

    pthread_t userSimTID, readPostTID[10], pushUpdTID[25];
    pthread_attr_t usattr, rpattr[10], pdattr[25];
    // Initialising thread attributes with default values
    pthread_attr_init(&usattr);
    for(int i = 0; i < 25; i++)
    pthread_attr_init(&pdattr[i]);
    for(int i = 0; i < 10; i++)
    pthread_attr_init(&rpattr[i]);
    
    // Creating threads
    pthread_create(&userSimTID, &usattr, userSimulator, nullptr);
    for(int i = 0; i < 25; i++)
    {
        int *arg = new int(i);
        pthread_create(&pushUpdTID[i], &pdattr[i], pushUpdates, static_cast<void*>(arg));
    }

    for(int i = 0; i < 10; i++)
    {
        int *arg = new int(i);
        pthread_create(&readPostTID[i], &rpattr[i], readPosts, static_cast<void*>(arg));
    }

    // Waiting for threads    
    pthread_join(userSimTID, nullptr);
    for(int i = 0; i < 10; i++)
    pthread_join(readPostTID[i], nullptr);
    for(int i = 0; i < 25; i++)
    pthread_join(pushUpdTID[i], nullptr);

    // Destroying all mutex locks
    pthread_mutex_destroy(&mutexUpdateQueue);
    for(int i = 0; i < MAXNODES; ++i)
    pthread_mutex_destroy(&mutexFeedQueue[i]);
    for(int i = 0; i < 10; ++i)
    pthread_mutex_destroy(&mutexUpdNodeFeed[i]);
    pthread_mutex_destroy(&filelock);
    pthread_mutex_destroy(&stdoutlock);

    // Destroying all conditional variables
    pthread_cond_destroy(&condUpdateQueue);
    for(int i = 0; i < 10; i++)
    pthread_cond_destroy(&condUpdNodeFeed[i]);

    delete[] nodes;
    
    return 0;
}