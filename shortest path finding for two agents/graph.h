/*
    Mustafa Burak Gurbuz
    150150082
    mustafaburakgurbuz@gmail.com
*/
#include <vector>
#include <iostream>
#include <limits>
#include <algorithm> 
#include <queue> 
using namespace std;

//struct for representing edges (used for building adj matrix)
struct edgeTriplet {
    int src_index;
    int dst_index;
    float weight;
};

//conflict info for solving the conflict
struct conflict_info {
    int node_index;
    bool j_forward;
    bool l_forward;
};

//node struct, element of a path (vector<node>)
struct node {
    int index;
    float distance;
    int prev;

    node(int ind, float dis, int p){index = ind; distance = dis; prev = p;}
    node(int ind){index = ind; distance = numeric_limits<float>::max(); prev = ind;}
};

//overload cmp operations for priority queue
bool operator> (const node &n1, const node &n2){return n1.distance > n2.distance;}
bool operator< (const node &n1, const node &n2){return n1.distance < n2.distance;}
bool operator==(const node &n1, const node &n2){return n1.distance == n2.distance;}


class Graph {
    //private attributes
    vector<vector<float>> original_network;
    int number_of_nodes;
    int JH, JD, LH, LD;

    //takes src, dst, network, time_padding and find the best route src->dst
    vector<node> find_shortest_path(int, int, vector<vector<float>>, float);
    //removes column of a given network (cuts out the connection of the node)
    vector<vector<float>> temp_delete_node(int, vector<vector<float>>);
    //plans src->dst dst->src using network1 and network2 respectively
    //due to conflict network1 may not be equal to network2 (for example, there is a conflict at dst->src path)
    vector<node> find_route(int, int, vector<vector<float>>, vector<vector<float>>);
    //takes two paths and detects first intersection (conflict)
    conflict_info find_conflict(vector<node> j_path, vector<node> l_path);
public:
    //only constructor for the class
    Graph(int, int, int, int, int);
    //build adj matrix using vector of triplets
    void build(vector<edgeTriplet>); 
    //print route like the given format
    void print_route(const char *, vector<node>);
    //solve the problem
    void simulate(bool);
};



Graph::Graph(int num_nodes, int JH, int JD, int LH, int LD) {
    number_of_nodes = num_nodes;
    this->JH = JH; this->JD = JD; this->LH = LH; this->LD = LD;
    //initilize number_of_nodes X number_of_nodes zero matrix
    original_network = vector<vector<float>>(number_of_nodes, vector<float>(number_of_nodes, 0));
}


void Graph::print_route(const char * name, vector<node> route) {
    cout<<name<<"'s Path, duration: "<< route.back().distance<<endl;
    for(int i = 0; i  < route.size(); i++) {
        if (i > 0 && route[i - 1].index == route[i].index)
            cout<<"-- return --"<<endl;
        cout<<"Node: " <<route[i].index<<" Time: "<<route[i].distance<<endl;
    }
}

vector<node> Graph::find_route(int src, int dst, vector<vector<float>> network_forward, vector<vector<float>> network_backward ){
    vector<node> full_path;
    //forward path
    vector<node> path_forward = find_shortest_path(src, dst, network_forward, 0);
    if (path_forward.empty()) {
        return vector<node>();
    }
    float time_padding = path_forward.back().distance + 30; 
    //backward path
    vector<node> path_backward = find_shortest_path(dst, src, network_backward, time_padding);
    if (path_backward.empty()) {
        return vector<node>();
    }
    //merge paths
    for(int i = 0; i < path_forward.size(); i++)
        full_path.push_back(path_forward[i]);
    for(int i = 0; i < path_backward.size(); i++)
        full_path.push_back(path_backward[i]);
    return full_path;
}

void Graph::build(vector<edgeTriplet> edgeList) {
    for(int i = 0; i < edgeList.size(); i++) {
        edgeTriplet edge = edgeList[i];
        original_network[edge.src_index][edge.dst_index] = edge.weight;
    }
}


vector<node> Graph::find_shortest_path(int src_index, int dst_index, vector<vector<float>> network, float time_padding) {
    //pq is for tracking best node so far
    priority_queue<node, vector<node>, greater<node> > pq;
    pq.push(node(src_index, 0, src_index));

    //For keeping track of all distances
    vector<node> distances;
    for(int i = 0; i < number_of_nodes; i++)
        distances.push_back(node(i));
    
    distances[src_index].distance = 0.0; //set start cost to 0
    while(true) {
        if (pq.empty()) //no solution
            return vector<node>();
        int top_node_index = pq.top().index;
        //Check  if the best node so far is the dst node
        if (top_node_index == dst_index)
            break;
        //Pop after checking dst_index, I must not pop the dst
        pq.pop();
        //Get all neighbours of the current node
        vector<float> neighbours = network[top_node_index];
        for(int i = 0; i < neighbours.size(); i++) {
            //0.0 means there is no connection skip 
            if (neighbours[i] == 0.0)
                continue;
            int node_index = i;
            int w = neighbours[i];
            //If found path is better than previous one replace it
            if (distances[node_index].distance > distances[top_node_index].distance + w) {
                distances[node_index].distance = distances[top_node_index].distance + w;
                distances[node_index].prev = top_node_index;
                pq.push(node(node_index, distances[node_index].distance, top_node_index));
            }
            network[top_node_index][node_index] = 0.0;
        }
    }

    //Backtrace dst node for finding the path
    node dst = pq.top();
    vector<node> path;
    while(true){
        int current = dst.index;
        int prev = dst.prev;
        dst.distance = dst.distance + time_padding;
        path.push_back(dst);

        if(current == prev) {
            //paths are from dst to src so reverse it
            reverse(path.begin(), path.end());
            break;
        }
        dst = distances[dst.prev];
    }
    return path;
}

vector<vector<float>> Graph::temp_delete_node(int id, vector<vector<float>> network) {
    for(int i = 0; i < number_of_nodes; i++) {
        network[i][id] = 0.0;
    }
    return network;
}

void Graph::simulate(bool solve_one = false) {
    //Joseph's calculation
    vector<node> j_path = find_route(JH, JD, original_network, original_network);
    //Lucy's calculation
    vector<node> l_path = find_route(LH, LD, original_network, original_network);
    //Solve conflicts
    vector<vector<float>> search_network = original_network;
    vector<node> j_new_path;
    vector<node> l_new_path;
    while(true) {
        if (j_path.empty() || l_path.empty()) {
            cout<<"No solution"<<endl;
            return;
        }
        conflict_info conflict = find_conflict(j_path, l_path);
        if (conflict.node_index != -1) {
            search_network = temp_delete_node(conflict.node_index, search_network);
            //Used goto for brevity otherwise it is harder to read nested ifs 
            //if conflict due to Joseph's destination
            if (conflict.node_index == JD) {
                l_new_path = find_route(LH, LD, conflict.l_forward ? search_network : original_network,  conflict.l_forward ? original_network : search_network);
                goto LUCY;
            }
            //if conflict due to Lucy's destination
            else if (conflict.node_index == LD) {
                j_new_path = find_route(JH, JD, conflict.j_forward ? search_network : original_network,  conflict.j_forward ? original_network : search_network);
                goto JOSEPH;
            } else {
                j_new_path = find_route(JH, JD, conflict.j_forward ? search_network : original_network,  conflict.j_forward ? original_network : search_network);
                l_new_path = find_route(LH, LD, conflict.l_forward ? search_network : original_network,  conflict.l_forward ? original_network : search_network);
            }

            if (j_new_path.empty() && l_new_path.empty()) {
                cout<<"No solution"<<endl;
                return;
            } else if (j_new_path.empty() == false && l_new_path.empty() == true) {
JOSEPH:         j_path = j_new_path;
            } else if (j_new_path.empty() == true && l_new_path.empty() == false) {
LUCY:           l_path = l_new_path;
            } else {
                if (j_new_path.back().distance + l_path.back().distance < j_path.back().distance + l_new_path.back().distance)
                    j_path = j_new_path;
                else
                    l_path = l_new_path;
            }
        } else {
            break;
        }
        if (solve_one)
            break;
    }
    //Print results
    print_route("Joseph", j_path);
    cout<<endl;
    print_route("Lucy", l_path);
}

conflict_info Graph::find_conflict(vector<node> j_path, vector<node> l_path) {
    conflict_info conflict = {-1, false, false};
    //Find index of dst in the path
    int j_dst_index = 0;
    for(; j_dst_index < j_path.size(); j_dst_index++) {
        if (j_path[j_dst_index].index == JD)
            break;
    }
    int l_dst_index = 0;
    for(; l_dst_index < l_path.size(); l_dst_index++) {
        if (l_path[l_dst_index].index == LD)
            break;
    }

    int j_ite = 0;
    int l_ite = 0;
    while(true) {
        //special case! joseph visits Lucy's destination check time interval
        if (j_path[j_ite].index == LD) {
            if(j_path[j_ite].distance < l_path[l_dst_index].distance  || j_path[j_ite].distance > (l_path[l_dst_index + 1].distance)) {
                j_ite++;
                if (j_ite == j_path.size())
                    return conflict;
            } else {
                conflict.node_index = LD;
                conflict.j_forward = j_dst_index >= j_ite ? true : false; 
                conflict.l_forward = false;
            }
        }
        //special case! Lucyvisits Josephs's destination check time interval
        if (l_path[l_ite].index == JD) {
            if(l_path[l_ite].distance < j_path[j_dst_index].distance  || l_path[l_ite].distance > (j_path[j_dst_index + 1].distance)) {
                l_ite++;
                if (l_ite == l_path.size())
                    return conflict;
            } else {
                
                conflict.node_index = JD;
                conflict.j_forward = false;
                conflict.l_forward = l_dst_index >= l_ite ? true : false; 
                return conflict;
            }
        }
        if (j_path[j_ite] > l_path[l_ite]) {
            l_ite++;
            if (l_ite == l_path.size())
                return conflict;
        } else if (j_path[j_ite] < l_path[l_ite]) {
            j_ite++;
            if (j_ite == l_path.size())
                return conflict;
        } else {
            if (j_path[j_ite].index == l_path[l_ite].index) {
                conflict.node_index = j_path[j_ite].index;
                conflict.j_forward = j_dst_index >= j_ite ? true : false; 
                conflict.l_forward = l_dst_index >= l_ite ? true : false; 
                return conflict;
            }
            else {
                j_ite++;
                if (j_ite == l_path.size())
                    return conflict;
            }
        }
    }
}