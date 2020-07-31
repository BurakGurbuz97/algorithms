//Mustafa Burak Gurbuz 150150082
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "helper.h"
using namespace std;

class TestSuite {
    string id;  //Id of the test suite
    int bugs_detected; // number of bugd
    int running_time; // running time
    vector<string> test_case_f_profs; //frequncy profiles
    vector<int> code_coverage_numbers; // vector of code coverage values
    vector<string> OSSEFs; //Ordered sequence of statement execution frequencies
    void calculateCodeCov();
    void calculateOSSEF();
    vector<int> argsort(const vector<int>&  v);
    friend class RegressionTest; // Regression test needs to reach private valuables so made it a friend class

public:
    TestSuite(string ID, int bugs, int time, vector<string> f_profs) {
        id = ID; bugs_detected = bugs, running_time = time; test_case_f_profs = f_profs;
        this->calculateCodeCov();
        this->calculateOSSEF();
    } 
    int get_bugs(){return bugs_detected;}
    int get_runnig_time(){return running_time;}
    string get_id(){return id;}
    friend ostream& operator<<(ostream& os, const TestSuite& T);
    vector<int> sortTestCases();
};



//Sort test cases using steps defined at report part2
vector<int> TestSuite::sortTestCases(){
    //Pick the one/s with the largest code coverage
    int largest_code_cov = *max_element(code_coverage_numbers.begin(), code_coverage_numbers.end());
    vector<int> sorted_test_case_indices;
    for(int i = 0; i < code_coverage_numbers.size(); i ++) {
        if (code_coverage_numbers[i] == largest_code_cov) {
            sorted_test_case_indices.push_back(i);
        } 
    }
    //Compute OSSEF distance between remainings and superior test case
    vector< pair <int,int> > indices_and_distances;
    for(int i = 0; i < OSSEFs.size(); i ++){
        if(find(sorted_test_case_indices.begin(), sorted_test_case_indices.end(), i) != sorted_test_case_indices.end()) {
            continue;
        } else {
            string OSSEF_max = OSSEFs[sorted_test_case_indices[0]];
            string current = OSSEFs[i];
            DynamicEditDistance calculator;
            int distance = calculator.distance(OSSEF_max, current);
            indices_and_distances.push_back(make_pair(i, distance));
        }
    }
    //Sort largest to smallest distance
    sort(indices_and_distances.begin(), indices_and_distances.end(), [](const std::pair<int,int> &left, const std::pair<int,int> &right) {
        return left.second > right.second;
    });
    //Add other test cases
    for(int i = 0; i < indices_and_distances.size(); i++) {
        sorted_test_case_indices.push_back(indices_and_distances[i].first);
    }
    return sorted_test_case_indices;
}

//In order to calculaye code coverage I count both number of dashes and zeros. Codecoverage = (#dashes + 1) - #zeros 
void TestSuite::calculateCodeCov() {
    for(int i = 0; i < test_case_f_profs.size(); i++) {
        int number_of_dashes = 0;
        int number_of_zeros = 0;
        string f_prof = test_case_f_profs[i];
        for(int index = 0; index < f_prof.size(); index++) {
            if (f_prof[index] == '-')
                number_of_dashes++;
            if (f_prof[index] == '0')
                number_of_zeros++;
        }
        int code_cov = (number_of_dashes + 1) - number_of_zeros;
        code_coverage_numbers.push_back(code_cov);
    }
}


//Find the indices that will sort the array
vector<int> TestSuite::argsort(const vector<int>&  v){
    vector<int> indices;
    for(int i = 0; i < v.size(); i++)
        indices.push_back(i);
    //use stable sort since we need to preserve the original indices
    stable_sort(indices.begin(), indices.end(), [&v](int i1, int i2) {return v[i1] < v[i2];});
    return indices;
}


//Calculate OSSEF using frequency profiles
void TestSuite::calculateOSSEF() {
    for(int i = 0; i < test_case_f_profs.size(); i++) {
       stringstream ss(test_case_f_profs[i]);
       vector<int> dash_removed;
       string temp;
       while(getline(ss, temp, '-')) {
           dash_removed.push_back(stoi(temp));
       }
       vector<int> ascending_indices = argsort(dash_removed);
       string OSSEF = "";
       for(int index : ascending_indices){
           if (index != ascending_indices.back()) {
               OSSEF = OSSEF + to_string(index + 1) + "-";
           } else {
               OSSEF = OSSEF + to_string(index + 1);
           }
       }
       OSSEFs.push_back(OSSEF);
    }
}

//Function for printing using cout<<
ostream& operator<<(ostream& os, const TestSuite& T){
    os<<"ID: "<<T.id<<" bugs: "<<T.bugs_detected<<" run time: "<<T.running_time<<endl;
    os<<"\tSuites"<<endl;
    for(int i = 0; i < T.test_case_f_profs.size(); i++) {
        os<<'\t'<<'\t'<<"Code Cov" <<"("<<T.code_coverage_numbers[i]<<") "<<" Freq. Pro: "<<
        T.test_case_f_profs[i]<<" OSSEF: "<<T.OSSEFs[i]<<endl;
    }
	return os;
}

//Class that holds all the test suites
class RegressionTest {
    vector<TestSuite> test_suits;
    int time_limit;
public:
    RegressionTest(char * filename);
    friend ostream& operator<<(ostream& os, const RegressionTest& RT);
    vector<TestSuite> get_best_suits();
};

//Uses knapscaksolver from helper.h to pick best set of test suites with subject to time limit
vector<TestSuite> RegressionTest::get_best_suits() {
    KnapSackProblemSolver solver(test_suits.size(), time_limit);
    vector<int> bugs;
    vector<int> times;
    for(int i = 0; i < test_suits.size(); i++){
        bugs.push_back(test_suits[i].bugs_detected);
        times.push_back(test_suits[i].running_time);
    }
    solver.build_table(bugs, times);
    vector<TestSuite> picked_suites;
    vector<int> best_indices = solver.get_best_items();
    for(int i : best_indices) {
        picked_suites.push_back(test_suits[i - 1]);
    }
    return picked_suites;
}

//Constructor which takes a filepath and parses it in order to initialize all test suites 
RegressionTest::RegressionTest(char * filename) {
    ifstream file(filename);
    string temp;
    string time_limit;
    
    //Read first line and get the time constraint
    getline(file, temp);
    stringstream ss(temp);
    getline(ss, temp, ':');
    getline(ss, time_limit, '\n');
    this->time_limit = stoi(time_limit);

    //Skip two unimportant lines
    getline(file, temp);
    getline(file, temp);

    while(getline(file, temp)) {
        stringstream ss_ts(temp);
        string id;
        int bugs;
        int time;
        vector<string> freq_profs; 
        getline(ss_ts, id, ' ');
        getline(ss_ts, temp, ' ');
        bugs = stoi(temp);
        getline(ss_ts, temp, ' ');
        time = stoi(temp);
        string freq_prof;
        while(getline(ss_ts, freq_prof, ' ')) {
            freq_profs.push_back(freq_prof);
        }
        TestSuite suit(id, bugs, time, freq_profs);
        test_suits.push_back(suit);
    }
    file.close();
}

//For printing regresssion test objects using cout<<
ostream& operator<<(ostream& os, const RegressionTest& RT){
    os<<" Time Limit: "<<RT.time_limit<<endl;
    for(int i = 0; i < RT.test_suits.size(); i++) {
        os<<'\t'<<RT.test_suits[i]<<endl;
    }
	return os;
}