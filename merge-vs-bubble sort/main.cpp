#include "vector.h"
#include <iostream>
#include <time.h>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout<<"Invalid command line arguments: expected two received "<<argc - 1<<endl;
        return 1;
    }

    char * filename = argv[3];
    int N = atoi(argv[2]);
    char * type = argv[1];

    if(*type != 'b' && *type != 'm') {
        cout<<"Invalid algorithm type: "<<*type<<" use 'b' for bubble sort or 'm' for merge short" <<endl;
        return 1;
    } 

    std::ifstream infile(filename);
    if(!infile){
        cout<<"Unable to open: "<<filename<<endl;
        return 1;
    }

    burak::vector<int> array;
    int x;
    for(int i = 0; i < N; i++){
        infile >> x;
        array.push_back(x);
    }

    clock_t tick, tock;
    tick = clock();
    if (*type == 'm'){
        array.merge_sort();
    } else {
        array.bubble_sort();
    }
    tock = clock();
    cout<<(*type == 'm' ? "Merge" : "Bubble")<<" Sort took: "<<((float)tock - tick)/CLOCKS_PER_SEC<<" seconds"<<endl;

    ofstream ofile("output.txt");
    if(ofile.is_open()){
        for(int i = 0; i < array.size(); i++) {
            ofile<<array[i]<<endl;
        }
        cout<<"Sorted array is writen to output.txt"<<endl;
    } else {
        cout<<"Unable to open output.txt"<<endl;
    }
    return 0;
}