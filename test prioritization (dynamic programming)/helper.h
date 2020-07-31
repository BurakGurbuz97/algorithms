//Mustafa Burak Gurbuz 150150082
#include <vector>
#include <string>

using namespace std;

int max(int a, int b) {
    return (a > b) ? a : b;
}

class KnapSackProblemSolver {
    vector<vector<int>>  MEMO;  // Memory for memoization
    bool MEMO_FLAG;             // False if memort is not filled True if it is populated
    int weight_limit;           // Weight limit for the knapsack (Time limit for test)
    int number_of_items;        // Number of items (Number of test suites)
    vector<int> weights;        // weights of items (run times of tests)
    vector<int> values;         // values of items (bugs dicovered)

    public:
        KnapSackProblemSolver(int num_items, int w_lim);
        void build_table(vector<int> vals, vector<int> ws);
        int get_best_value();
        vector<int> get_best_items();
};

//Constructor that initalized memory filled with zeros
KnapSackProblemSolver::KnapSackProblemSolver(int num_items, int w_lim) {
    //+1 because if indexing starts from 1, it is easier to read and track 
    MEMO = vector<vector<int>>(num_items + 1, vector<int>(w_lim + 1, 0)); 
    MEMO_FLAG = false; weight_limit = w_lim; number_of_items = num_items;
}

void KnapSackProblemSolver::build_table(vector<int> vals, vector<int> ws) {
    //Something wrong with the inputs
    if (number_of_items != vals.size() || number_of_items != ws.size())
        throw "Size of values and weigths wrong";
    weights = ws; values = vals;

    //Main dynamic programming logic O(number_of_items X weight_limit)
    for(int i = 0; i < number_of_items + 1; i++) {
        for (int w = 0 ; w < weight_limit +1; w++) {
            if (i == 0 || w == 0) {
                MEMO[i][w] = 0;
            } else if (weights[i - 1] <= w) {
                MEMO[i][w] = max(values[i -1] + MEMO[i - 1][w - weights[i - 1]], MEMO[i - 1][w]);
            } else {
                MEMO[i][w] = MEMO[i - 1][w];
            }
        }
    }
    //MEMO is valid set flag to true
    MEMO_FLAG = true;
}

//Bottom right corner of the memory table has the best possible value
int KnapSackProblemSolver::get_best_value() {
    if (MEMO_FLAG)
        return MEMO[number_of_items][weight_limit];
    else
        throw "Call build_table(...) first";
}

//Use memory to back track items. Start from right corner and go up and left as described in the slides
vector<int> KnapSackProblemSolver::get_best_items() {
    int best_value = this->get_best_value();
    int weight = weight_limit;
    vector<int> included;
    for(int i = number_of_items; i > 0; i--) {
        if (best_value <= 0)
            break;
        if (best_value == MEMO[i - 1][weight])
            continue;
        else {
            included.push_back(i);
            weight = weight - weights[i - 1]; 
            best_value = best_value - values[i - 1];
        } 
    }
    return included;
}

//Min operation for triplets
int min(int a, int b, int c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

//Actually this could be just a function but I am sticking with the OOP
class DynamicEditDistance {
    vector<vector<int>>  MEMO;
public:
    int distance(string str1, string str2);
};


int DynamicEditDistance::distance(string str1, string str2) {
    ///////////////////////////////////////////////////////////
    //Convert string representation to units so that each number
    //is treated as seperate units
    vector<int> str1_v;
    vector<int> str2_v;
    string temp;
    stringstream ss1(str1);
    while(getline(ss1, temp, '-')) {
           str1_v.push_back(stoi(temp));
    }

    stringstream ss2(str2);
    while(getline(ss2, temp, '-')) {
           str2_v.push_back(stoi(temp));
    }
    ///////////////////////////////////////////////////////////

    //Init memory
    MEMO = vector<vector<int>>(str1_v.size() + 1, vector<int>(str2_v.size() + 1, 0));

    //Main dynamic programming logic. O(str1_v.size() X str2_v.size())
    for(int i = 0; i <= str1_v.size(); i++) {
        for(int j = 0; j <= str2_v.size(); j++) {
            if (i == 0)
                MEMO[i][j] = j;
            else if (j == 0)
                MEMO[i][j] = i;
            else if (str1_v[i-1] == str2_v[j-1])
                MEMO[i][j] = MEMO[i-1][j-1];
            else
                MEMO[i][j] = 1 + min(MEMO[i][j-1], MEMO[i-1][j], MEMO[i-1][j-1]);
        }
    }

    return MEMO[str1_v.size()][str2_v.size()];
}