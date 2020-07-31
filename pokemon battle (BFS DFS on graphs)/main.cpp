#include <iostream>
#include <vector>
#include <cstring>
#include "components.h"
#define DEFAULT_MAX_DEPTH 10
//Blastoise actions
action_metrics B_TACKLE = {-10,100,30,0,"Tackle"};
action_metrics B_WATER_GUN = {-20,100,40,0,"Water Gun"};
action_metrics B_BITE = {-25,100,60,0,"Bite"};
action_metrics B_SKIP = {100,100,0,3,"Skip"};

//Pikachu actions
action_metrics P_THUNDERSHOCK = {-10,100,40,0,"Thundershock"};
action_metrics P_SKULL_BASH = {-15,70,50,0,"Skull Bash"};
action_metrics P_SLAM= {-20,80,60,0,"Slam"};
action_metrics P_SKIP = {100,100,0,3,"Skip"};


int main(int argc, char **argv) {
    if (argc < 3) {
        cout<<"You forgot comman line args"<<endl;
    }
    //Create pokemons
    Pokemon pikachu = {200, 100};
    Pokemon blastoise = {200, 100};
    //Create action array
    vector<action_metrics> pikachu_actions = {P_THUNDERSHOCK, P_SKULL_BASH, P_SLAM, P_SKIP};
    vector<action_metrics> blastoise_actions = {B_TACKLE, B_WATER_GUN, B_BITE, B_SKIP};
    
    if (strcmp(argv[1], "part1") == 0) {
        Battle fight(pikachu, pikachu_actions, blastoise, blastoise_actions, atoi(argv[2]));
        fight.Simulate();
        fight.print_leaves();
    } else if (strcmp(argv[1], "part2") == 0) {
        Battle fight(pikachu, pikachu_actions, blastoise, blastoise_actions, atoi(argv[2]));
        fight.Simulate();
        if (strcmp(argv[3], "dfs") == 0) {
            fight.DFS();
        } else {
            fight.BFS();
        }
    } else if (strcmp(argv[1], "part3") == 0) {
        Battle fight(pikachu, pikachu_actions, blastoise, blastoise_actions, DEFAULT_MAX_DEPTH);
        fight.Simulate();
        fight.find_best_path(strcmp(argv[2],"pikachu") == 0);
    } else {
        cout<<"Could not understand part: "<<argv[1]<<endl;
    }
}