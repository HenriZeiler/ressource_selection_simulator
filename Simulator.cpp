//
// Created by Henri Zeiler on 12.05.24.
//

#include "Simulator.h"
#include <iostream>

bool Simulator::resource_q_available(Resource q) {
    return q.max_slots > q.actors_at_q.size();
}

bool Simulator::swap_resource_for_actor(Actor a) {
    function_space max_util = -1;
    int max_q_idx = -1;
    for(auto q_idx:a.available_resources) {
        if(resource_q_available(resources[q_idx]) && utility_function(resources[q_idx].actors_of_type[a.type]) > max_util) {
                max_util = utility_function(resources[q_idx].actors_of_type[a.type]);
                max_q_idx = q_idx;
            }
        }
    if(max_q_idx != a.cur_resource) {       //also handles case in which no adjacent resource has slots left and updates resources fractions
        if(a.cur_resource != -1) resources[a.cur_resource].actors_of_type[a.type]--;
        a.cur_resource = max_q_idx;
        if(max_q_idx != -1) resources[max_q_idx].actors_of_type[a.type]++;
        return 1;
    }
    return 0;
}

function_space Simulator::get_total_utility(function<function_space(function_space)> utility_function) {
    function_space res = 0;
    for(auto a: actors) {
        res += utility_function(resources[a.cur_resource].actors_of_type[a.type]);
    }
    return res;
}

int Simulator::step() {
    int swaps_made = 0;
    for(auto a:actors) {
        swaps_made += swap_resource_for_actor(a);
    }
    return swaps_made;
}

//returns step at which nash equil was reached or -1
int Simulator::run_simulation(int steps, int data_collection_interval, vector<function_space>& social_welfare_at_collection_step, vector<vector<function_space>>& fraction_at_q_at_collection_step, vector<function_space>& segregation_welfare_at_collection_step) {
    for(int i=0;i<steps;i++) {
        if(!step()) {
            clog<< "nash equilibrium reached in step: " << i+1 << endl;
            return i+1;
        }
    }
    return -1;
}

Simulator::Simulator(const int number_of_types, vector<Actor> actors, vector<Resource> resources,
                     function<function_space(function_space)> utility_function) :
                     number_of_types(number_of_types), actors(actors), resources(resources), utility_function(utility_function) {
}