#include "SAGraphColoring.h"
#include "heuristics.h"

#include <iostream>
#include <random>
#include <chrono>

int main(){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);

    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename = "instances/le450_15c.col";
    Graph graph(filename);
    int num_color = 15;
    int T_max{1000};
    int T_min{10};
    int max_iter{1000};
    int max_worsen{100};
    double alpha{0.9};

    SimulatedAnnealingColoring sa(graph, num_color, rng, T_max, T_min, max_iter, max_worsen, alpha);

    Individual indv = sa.run();
    Fitness fit{};
    evaluate_fitness(graph, indv, fit);
    
    print_individual(indv, fit);

    return 0;
}

//g++ main_test_simulated_annealing.cpp lib/heuristics.cpp lib/utils.cpp lib/fwlist.cpp lib/metaheuristics.cpp lib/graphs.cpp lib/solution.cpp -I include -o main_test_sa