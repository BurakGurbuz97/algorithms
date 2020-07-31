#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

using namespace std;


int min(int a, int b){
    return (a > b) ? b : a;
}

class Graph{
    public:
    vector <vector<int>> capacity;
    vector <vector<int>> flow;
    int nodeCount;
    vector <int> excess;
    vector <int> height; 
    
    void addCapacity(int, int, int);
    void push(int, int);
    void relabel(int);
    void initializePreflow(int);
    int overflowingVertex();
    int findMaxFlow();
    Graph(int);
    ~Graph();
};

Graph::Graph(int n){
    nodeCount = n;
    capacity.assign(nodeCount, vector<int>(nodeCount, 0));
}

void Graph::addCapacity(int from, int to, int cap){
    capacity[from][to] = cap;
}


int Graph::overflowingVertex(){
    for(int i = 0; i < nodeCount - 1; i++){
        if (excess[i] > 0) {
            return i;
        }
    }
    return -1;
}

void Graph::initializePreflow(int source){
    for(int i = 0; i < nodeCount; i++) {
        excess.push_back(0);
        height.push_back(0);
    }
    for(int u = 0; u < nodeCount; u++){
        flow.push_back(vector<int>());
        for(int v = 0; v < nodeCount; v++) {
            flow[u].push_back(0);
        }
    }
    height[source] = nodeCount;
    for(int v = 0; v < nodeCount; v++){
        flow[source][v] = capacity[source][v];
        excess[v] = capacity[source][v];
        excess[source] = excess[source] - capacity[source][v];
        capacity[v][source] = 0;
        flow[v][source] = -flow[source][v];
    }
}

void Graph::push(int u, int v){
    int cf =  capacity[u][v] - flow[u][v];
    int delta = min(excess[u], cf);
    flow[u][v] = flow[u][v] + delta;
    flow[v][u] = flow[v][u] - delta;
    excess[u] = excess[u] - delta;
    excess[v] = excess[v] + delta;
}

void Graph::relabel(int u){
    int min_h = nodeCount;
    for(int v = 0; v < nodeCount; v++){
        int cf =  capacity[u][v] - flow[u][v];
        if (cf > 0) {
            if (height[v] < min_h) {
                min_h = height[v];
            } 
        }
    }
    height[u] = min_h + 1;
}

int Graph::findMaxFlow(){
    while( overflowingVertex()  != -1) {
        int u = overflowingVertex();
        bool pushed = false;
        for(int v = 0; v < nodeCount; v++){
            if ((capacity[u][v] != flow[u][v])  && height[u] > height[v]) {
                push(u, v);
                bool pushed = true;
                break;
            }
        }
        if (!pushed)
            relabel(u);
    }
    return excess[nodeCount - 1];
}

Graph::~Graph(){

}

int main(int argumentNumber, char *arguments[]){
    int nodeCount;
    string filename = arguments[1];
    string tempString;
    int temp, from, to, flow;
    
    ifstream file;
    file.open(filename);
    file >> nodeCount;
    Graph graph(nodeCount);
    file >> tempString >> tempString >> tempString;
    while(file.good()){
        file >> from >> to >> flow;
        graph.addCapacity(from, to, flow);
    }
    graph.initializePreflow(0); // 0th node is source
    cout << "Maximum flow: " << graph.findMaxFlow() << endl;
    
    return 0;
}