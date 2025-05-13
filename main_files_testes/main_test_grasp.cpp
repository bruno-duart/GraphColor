#include "GRASPGraphColoring.h"

#include <iostream>
#include <random>
#include <chrono>

int main()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename{"instances/le450_15a.col"};
    Graph graph(filename);
    int num_color{15};
    int max_rcl_size{num_color / 2};
    int acceptanceratio{1000};

    Individual indv{};
    Fitness fit{};

    GRASPGraphColoring grasp = GRASPGraphColoring(graph, num_color, rng, max_rcl_size);
    indv = grasp.BuildPhase();
    evaluate_fitness(graph, indv, fit);
    print_individual(indv, fit);

    // for (int i{0}; i < 5; ++i)
    // {
    //     grasp.LocalSearch(indv, fit, acceptanceratio);
    //     evaluate_fitness(graph, indv, fit);
    //     print_individual(indv, fit);
    // }

    // indv = grasp.run();
    // evaluate_fitness(graph, indv, fit);
    // print_individual(indv, fit);
    
    return 0;
}
// g++ main_test_grasp.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/metaheuristics.cpp lib/utils.cpp -I include -o main_test_grasp
// ./main_test_grasp