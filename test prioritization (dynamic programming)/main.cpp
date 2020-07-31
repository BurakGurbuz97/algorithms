//Mustafa Burak Gurbuz 150150082
#include "test.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    RegressionTest RT(argv[1]);
    vector<TestSuite> picked_suits = RT.get_best_suits();
    //cout<<RT<<endl;  You may uncomment this to see how all test suites looks like

    int total_run_time = 0;
    int total_number_of_bugs = 0;

    string print = "";
    for(TestSuite ts : picked_suits){
        total_number_of_bugs += ts.get_bugs();
        total_run_time += ts.get_runnig_time();
        print += ts.get_id() + " ";
        vector<int> sorted_test_case_indices = ts.sortTestCases();
        for(int i : sorted_test_case_indices) {
            print += to_string(i + 1) + " ";
        }
        print += '\n';
    }

    //Print the results for both part1 and part2
    cout<<"Total amount of running time: "<<total_run_time<<" Previously discovered bugs: "<<total_number_of_bugs<<endl;
    cout<<print<<endl;
}