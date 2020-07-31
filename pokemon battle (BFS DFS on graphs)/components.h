#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <chrono>
#define PIKACHU_TURN true
#define BLASTOISE_TRUN false
using namespace std;
//Simple max operation
template<class T> T get_max(T a, T b) {
    return a > b ? a : b;
}

typedef chrono::high_resolution_clock Clock;

//Encapsulates information about actions
struct action_metrics {
    int PP;
    int accuracy;
    int damage;
    int first_usage;
    string name;
};

//Consists of pokemon HP and PP
struct Pokemon {
    int HP;
    int PP;
};

//Statistics of the node
struct Statistics {
    bool turn;
    float prob;
    short unsigned int level;
    bool is_leaf;
};

//Contains the necessary information to update pokemon states after an action
struct OutCome {
    Pokemon attacker_success;
    Pokemon defender_success;
    Statistics stats_success;
    Pokemon attacker_fail;
    Pokemon defender_fail;
    Statistics stats_fail;
    bool two_outcome;
};

//Node of the graph
struct Node {
    Pokemon pokemon1;
    Pokemon pokemon2;
    Statistics stats;
    vector<Node*> children;
    bool prev_effective;
    short unsigned int prev_index;
};

//Calculate consequence of an action both effective and uneffective possibilities
OutCome actionConsequenceCalculator(Pokemon attacker, Pokemon defender, action_metrics metrics, Statistics node_stats,  double choose_prob, int max_depth) {
    OutCome return_val;
    //If attack is effective
    return_val.attacker_success.PP =  attacker.PP + metrics.PP;
    return_val.attacker_success.HP =  attacker.HP;
    return_val.defender_success.PP =  defender.PP;
    return_val.defender_success.HP =  get_max(defender.HP - metrics.damage, 0);
    return_val.stats_success.level = node_stats.level + 1;
    return_val.stats_success.prob = node_stats.prob * choose_prob * (metrics.accuracy / 100.0);
    return_val.stats_success.turn = !node_stats.turn;
    return_val.stats_success.is_leaf = return_val.defender_success.HP == 0 || return_val.stats_success.level == max_depth;
    if (metrics.accuracy == 100) {
        return_val.two_outcome = false;
        return return_val;
    }

    //If there is possibility of noneffective action
    return_val.two_outcome = true;
    return_val.attacker_fail.PP =  attacker.PP + metrics.PP;
    return_val.attacker_fail.HP =  attacker.HP;
    return_val.defender_fail.PP =  defender.PP;
    return_val.defender_fail.HP =  defender.HP;
    return_val.stats_fail.level = node_stats.level + 1;
    return_val.stats_fail.prob = node_stats.prob * choose_prob * ((100.0 - metrics.accuracy) / 100.0);
    return_val.stats_fail.turn = !node_stats.turn;
    return_val.stats_fail.is_leaf = return_val.defender_fail.HP == 0 || return_val.stats_success.level == max_depth;
    return return_val;
}
//The main class that does all the work it creates the graph and implements all necessary search functions
class Battle {
    //These three are temp variables dont alter them!!!
    long int node_counter;
    vector<Node*> knock_outs;
    vector<Node*> best_path;

    //These are variables for the data structure
    Node* root;
    int max_depth;
    vector<action_metrics> actions1;
    vector<action_metrics> actions2;
    //Search for a path from root to target node in a recursive manner
    bool DFS_look_for_node(Node* node, vector<Node*> path ,Node* target){
        path.push_back(node);
        if(node == target) {
            best_path = path;
            return true;
        }
        if (node->stats.is_leaf == false){
            for(int i = 0; i < node->children.size(); i ++) {
                bool flag = DFS_look_for_node(node->children[i], path, target);
                if (flag)
                    return true;
            }
        }
        return false;
    }
    //Composes necessary messages for the part3 
    string compose_action_msg(Node* src, Node* dst) {
        string pokemon = src->stats.turn == PIKACHU_TURN ? "Pikachu":"Blastoise";
        string attack =  src->stats.turn == PIKACHU_TURN ? actions1[dst->prev_index].name : actions2[dst->prev_index].name ;
        string effect =  dst->prev_effective ? "effective" : "noneffective";
        string msg = pokemon + " used " + attack + ". It's " + effect + ".\n";
        return msg;
    }
    //DFS logic this is called by public funtion
    void DFS_helper(Node* node){
        node_counter++;
        if (node->stats.is_leaf == false){
            for(int i = 0; i < node->children.size(); i ++) {
                DFS_helper(node->children[i]);
            }
        }
    }
    //Find nodes that the loser pokemon is knocked out
    void get_knock_outs_recursive(Node* node, bool first_is_winner){
        if (node->stats.is_leaf == false){
            for(int i = 0; i < node->children.size(); i ++) {
                get_knock_outs_recursive(node->children[i], first_is_winner);
            }
        } else {
            if (first_is_winner && node->pokemon2.HP == 0) {
                knock_outs.push_back(node);
            } else if (!first_is_winner && node->pokemon1.HP == 0) {
                knock_outs.push_back(node);
            }
        }
    }
    //Print leaves this is for part1
    void print_leaves_recursive(Node* node) {
        if (node->stats.is_leaf == false){
            for(int i = 0; i < node->children.size(); i ++) {
                print_leaves_recursive(node->children[i]);
            }
        } else {
            cout<<"P_HP:"<<node->pokemon1.HP<<" P_PP:"<<node->pokemon1.PP<<" ";
            cout<<"B_HP:"<<node->pokemon2.HP<<" B_PP:"<<node->pokemon2.PP<<" ";
            cout<<" PROB:"<<node->stats.prob<<endl;
        }
    }
    //Simulate the game and create layers recursively (depth-first approach)
    void play_recurvise(Node& node, int depth_of_caller) {
        if (depth_of_caller == max_depth || node.pokemon1.HP <= 0 ||node.pokemon2.HP <= 0)
            return;
        vector<action_metrics> v = node.stats.turn ? actions1 : actions2;
        Pokemon attacker = node.stats.turn ? node.pokemon1 : node.pokemon2;
        Pokemon defender = node.stats.turn ? node.pokemon2 : node.pokemon1;
        double prob = depth_of_caller >= 3 ? 0.25 : 1.0 / 3.0 ;
        for(int i = 0; i < v.size(); i ++) {
            if (v[i].first_usage > depth_of_caller) {
                continue;
            }
            OutCome outcome = actionConsequenceCalculator(attacker, defender, v[i], node.stats, prob, max_depth);
            Node* child_success = new Node;
            child_success->pokemon1 = node.stats.turn ? outcome.attacker_success : outcome.defender_success;
            child_success->pokemon2 = node.stats.turn ? outcome.defender_success : outcome.attacker_success;
            child_success->stats = outcome.stats_success;
            child_success->children = vector<Node*>() ;
            child_success->prev_effective = true;
            child_success->prev_index = i;
            play_recurvise(*child_success, depth_of_caller + 1);
            node.children.push_back(child_success);
            if (outcome.two_outcome) {
                Node* child_fail = new Node;
                child_fail->pokemon1 = node.stats.turn ? outcome.attacker_fail : outcome.defender_fail;
                child_fail->pokemon2 = node.stats.turn ? outcome.defender_fail : outcome.attacker_fail;
                child_fail->stats = outcome.stats_fail;
                child_fail->children = vector<Node*>();
                child_fail->prev_effective = false;
                child_fail->prev_index = i;
                play_recurvise(*child_fail, depth_of_caller + 1);
                node.children.push_back(child_fail);
            }
        }
    }
public:
    //Constructor
    Battle(Pokemon poke1, vector<action_metrics> actions1, Pokemon poke2, vector<action_metrics> actions2, int max_depth){
        cout.precision(4);
        this->actions1 = actions1;
        this->actions2 = actions2;
        this->max_depth = max_depth;
        root = new Node;
        root->pokemon1 = poke1;
        root->pokemon2 = poke2;
        root->prev_index = -1;
        root->prev_effective = true;
        root->stats = {PIKACHU_TURN, 1.0, 0, false};
    }
    //Wrapper for private function
    void print_leaves(){ print_leaves_recursive(root);}
    //Wrapper for private function
    void Simulate() {play_recurvise(*root, 0);}
    //BFS and track number of nodes and time elapsed
    void BFS(){
        auto t1 = Clock::now();
        node_counter = 0;
        queue<Node *> q;
        q.push(root);
        while(!q.empty()) {
            Node* node = q.front();
            q.pop();
            if(node == NULL)
                return;
            for(int i = 0; i < node->children.size(); i++) {
                q.push(node->children[i]);
            }
            node_counter++;
        }
        auto t2 = Clock::now();
        cout<<"BFS --> "<<"time_elapsed: "<<chrono::duration_cast<chrono::milliseconds>(t2 - t1).count()<<" ms node_count: "<< node_counter<<endl;
    }
    //Wrapper for private function and also keeps track of time and nodes
    void DFS(){
        auto t1 = Clock::now();
        node_counter = 0;
        DFS_helper(root);
        auto t2 = Clock::now();
        cout<<"DFS --> "<<"time_elapsed: "<<chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()<<" ms node_count: "<< node_counter<<endl;
    }
    //Find the shortest possible knock_out for part3
    Node* get_best_knock_out(bool winner_pokemon1) {
        knock_outs = vector<Node*>();
        get_knock_outs_recursive(root, winner_pokemon1);
        if(knock_outs.size() == 0) {
            cout<<"Increase the max depth, cannot found any knock out path."<<endl;
        }
        int best_depth = max_depth;
        Node* best_node = NULL;
        for(int i = 0; i < knock_outs.size(); i++) {
            if (knock_outs[i]->stats.level < best_depth) {
                best_depth = knock_outs[i]->stats.level;
                best_node = knock_outs[i];
            }
        }
        return best_node;
    }
    //Wrap 3 private functions and print the best path (one of them) 
    vector<Node*> find_best_path(bool winner_pokemon1) {
        Node* target = get_best_knock_out(winner_pokemon1);
        bool flag = DFS_look_for_node(root, vector<Node*>(), target);
        for(int i = 1; i < best_path.size(); i++) {
            Node* src = best_path[i - 1];
            Node* dst = best_path[i];
            cout<<compose_action_msg(src, dst);
        }
        cout<<endl<<"Level count : "<<target->stats.level<<endl;
        cout<<"Probability : "<<target->stats.prob<<endl;
        return best_path;
    }
};