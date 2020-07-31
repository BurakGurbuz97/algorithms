/*
    Mustafa Burak Gurbuz
    150150082
    mustafaburakgurbuz@gmail.com
*/
#include <iostream>
#include <fstream>
#include <vector>
#include "graph.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout<<"No test file specified!"<<endl;
        return 1;
    }
    vector<edgeTriplet> edge_list;
    int JH, JD, LH, LD, number_of_nodes;
    ifstream file;
    file.open(argv[1]);
    file >> number_of_nodes;
    file >> JH >> JD >> LH >> LD;
    //Parses lines and fill edge_list until it is unable to parse
    for(edgeTriplet edge ;file >> edge.src_index >> edge.dst_index >> edge.weight; edge_list.push_back(edge)); 

    Graph city_network(number_of_nodes, JH, JD, LH, LD);
    city_network.build(edge_list);
    //solve_one = false: solves all 
    city_network.simulate(false);
    return 0;
}