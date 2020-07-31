#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <climits>
#include <algorithm> // std::min_element

using namespace std;


int sum_INF(int a, int b){
    return (a == INT_MAX) ? a : a+b;  
}

int my_min(int a, int b){
    return (a > b) ? b : a;
}


struct Edge{
    int sourceNode;
    int destinationNode;
    int weight;
};

class Graph{
    int numberOfNodes;
    int numberOfEdges;
    vector<Edge> edges;
    
public:
    void setNumberOfNodes(int n){
        numberOfNodes = n;
    }
    int getNumberOfNodes(){
        return numberOfNodes;
    }
    void setNumberOfEdges(int n){
        numberOfEdges = n;
    }
    int getNumberOfEdges(){
        return numberOfEdges;
    }
    
    void AddEdge(int sourceNode, int destinationNode, int weight){
        Edge e = Edge();
        e.sourceNode = sourceNode;
        e.destinationNode = destinationNode;
        e.weight = weight;
        
        edges.push_back(e);
    }
    vector<Edge> getAllEdges(){
        return edges;
    }
    
    vector<Edge> getOutgoingEdges(int src){
        vector<Edge> outgoingEdges;
        
        for (Edge e : edges){
            if(e.sourceNode == src){
                outgoingEdges.push_back(e);
            }
        }
        
        return outgoingEdges;
    }
    
    int getWeightOfEdge(int src, int dst){
       int weight = 0;
       for (Edge e : edges){
           if(e.sourceNode == src && e.destinationNode == dst){
               cout << "weigth: " << e.weight << endl;
               weight = e.weight;
           }
       }
       
       return weight;
    }
    
    void printGraph(){
        cout << "************* Graph data *************" << endl;
        cout << "Number of total edges: " << numberOfEdges << endl;
        cout << "Number of total nodes: " << numberOfNodes << endl << endl;
        
        cout << "Source node --> Destination node : Edge weigth" << endl;
        for (std::vector<Edge>::const_iterator i = edges.begin(); i != edges.end(); ++i){
            std::cout << i->sourceNode << " --> " ;
            std::cout << i->destinationNode << " : " ;
            std::cout << setw(2) << i->weight << endl ;
        }
        cout << "**************************************" << endl << endl;
    }
};

class BellmanFord{
    Graph G;
    int sourceNode;
    int destinationNode;
    int** M;
    int* predecessors;
    bool graphContainsNegativeWeightCycle;
        
public:
    void setGraph(Graph g){
        G = g;
    }
    Graph getGraph(){
        return G;
    }
    void setSourceNode(int s){
        sourceNode = s;
    }
    int getSourceNode(){
        return sourceNode;
    }
    void setDestinationNode(int t){
        destinationNode = t;
    }
    int getDestinationNode(){
        return destinationNode;
    }
    bool buildDPTable(){
        int nv = G.getNumberOfNodes();
        predecessors = new int[nv];
        M = new int*[nv+1];
        for (int i = 0; i < nv+1; i++)
            M[i] = new int[nv+1];

        for(int i = 0; i < nv; i++)
            M[0][i] = INT_MAX; 

        M[0][destinationNode] = 0;
        
        //Iteration starts here!
        for(int i = 1; i < nv ; i++){
            // for every v in G
            for(int v = 0; v < nv; v++){
                int option1 = M[i -1][v];
                int option2 = INT_MAX;
                vector<int> temp;
                vector<Edge> edges =  G.getOutgoingEdges(v);
                if (edges.size() != 0)  {
                    for(Edge vw : edges){
                        temp.push_back(sum_INF(M[i - 1][vw.destinationNode], vw.weight)); 
                    }
                    auto picked_edge = min_element(temp.begin(),temp.end());
                    option2 = *picked_edge;
                    int index = picked_edge - temp.begin();
                    predecessors[edges[index].destinationNode] = edges[index].sourceNode;
                }
                M[i][v] =  my_min(option1, option2);
            }
        }
        
        for(Edge e: G.getAllEdges()){
            int u = e.sourceNode;
            int v = e.destinationNode;
            int w = e.weight;
            if(M[nv-1][u] != INT_MAX && M[nv-1][u]  > M[nv - 1][v] + w) {
                graphContainsNegativeWeightCycle = true;
                return false;
            }
        }
        return true;
    }
    
    void printDPTable(){
        if(!graphContainsNegativeWeightCycle){
            cout << endl << "Dynamic programming table: (shortest paths from each node to destination node): " << endl << endl;
            
            int numberOfNodes = G.getNumberOfNodes();
            
            for(int i = 0; i <= numberOfNodes; i++){
                for(int j = 0; j <= numberOfNodes; j++){
                    if(i == 0 && j == 0)
                        cout << "\t";
                    else if(i != 0 && j == 0)
                        cout << setw(10) << "Iteration-" << i - 1;
                    else if(i == 0 && j != 0)
                        cout << "\t" << "Node-" << j - 1 << " ";
                    else{
                        if(M[i - 1][j - 1] == INT_MAX)
                            cout << "\t " << "\u221E";
                        else
                            cout << "\t" << setw(2) << M[i - 1][j - 1];
                    }
                }
                cout << endl;
            }
            cout << endl;
        }
    }
    
    void printShortestPathBetweenSourceAndDestination(){
        int current = destinationNode;
        vector<int> path;
        while( current != sourceNode) {
            path.push_back(current);
            current = predecessors[current];
        }
        path.push_back(current);
        cout << "Path value from " << sourceNode << " to " << destinationNode << " node: ";
        for (int i=path.size() - 1; i >= 0; i--) {
            cout<<path[i]<< ((i == 0) ? "\n" :" --> ");
        }
            
    }
};

int main(int argc, char* argv[])
{
    if(argc > 1){
        Graph graph;
        
        //read graph data from file
        string inputFileName = argv[1];
        string line;
        ifstream infile(inputFileName);
        
        getline(infile, line);
        graph.setNumberOfNodes(stoi(line));
        getline(infile, line);
        graph.setNumberOfEdges(stoi(line));

        int src, dst, w;
        while (infile >> src >> dst >> w) {
           graph.AddEdge(src, dst, w);
        }
        
        graph.printGraph();
        
        BellmanFord bf;
        bf.setGraph(graph);
        
        int dstNode;
        int srcNode;
        
        cout << "Enter the destination node: ";
        cin >> dstNode;
        
        bf.setDestinationNode(dstNode);
    
        if(bf.buildDPTable()){
            bf.printDPTable();
            
            cout << "Enter a source node: ";
            cin >> srcNode;
            bf.setSourceNode(srcNode);
            
            bf.printShortestPathBetweenSourceAndDestination();
        } else {
            cout << "Graph contains negative weight cycle!" << endl;
        }
    }
    else{
        cout << "Error: please provide a test file!" << endl;
    }
    
    return 0;
}
