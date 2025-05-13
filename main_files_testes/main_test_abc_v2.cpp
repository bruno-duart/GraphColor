#include "ABCGraphColoringV2.h"

#include <iostream>
#include <random>
#include <chrono>

std::vector<Fitness> fitness;

int main()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename = "instances/myciel6.col";
    Graph graph(filename);
    int num_color = 7;

    // Exibe o grafo
    // graph.printGraph();

    Individual indv{initialize_individual(4, graph)};
    Fitness fit{};
    evaluate_fitness(graph, indv, fit);
    // print_individual(indv, fit);

    int num_bees{10};
    int max_iter{1};
    int limit{0};

    ABCGraphColoringV2 abc = ABCGraphColoringV2(num_bees, num_color, max_iter, limit, graph, rng);

    abc.initialize_employedBees_pseudoGreedy_v2();
    abc.print_colony();

    std::cout << "employed phase\n";
    abc.employed_bee_phase();
    // abc.print_colony();

    for (int i{0}; i < num_bees/2; ++i)
    {
        abc.print_employedBee(i);
        abc.waggle_dance(i, indv, fit);
        abc.print_employedBee(i);
        std::cout << "--\n";
    }

    // std::cout << "onlooker phase\n";
    // abc.onlooker_bee_phase();
    // abc.print_colony();

    // abc.print_limit_no_improve();

    // std::cout << "scout phase\n";
    // abc.scout_bee_phase();
    // abc.print_colony();

    // int idx = abc.find_best_bee();
    // std::cout << "best_bee: " << idx << '\n';
    // abc.print_employedBee(idx);
    // abc.print_onlookerBee(idx);

    return 0;
}

// g++ main_test_abc_v2.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/metaheuristics.cpp lib/utils.cpp lib/ABCGraphColoringV2.cpp lib/TabuGraphColoring.cpp lib/fwlist.cpp -I include -o main_test_abc_v2