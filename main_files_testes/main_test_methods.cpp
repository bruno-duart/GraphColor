#include "ABCGraphColoring.h"

#include <iostream>
#include <random>
#include <chrono>

std::vector<Fitness> fitness;

int main()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename = "instances/myciel3.col";
    Graph graph(filename);
    int num_color = 4;

    // Exibe o grafo
    // graph.printGraph();

    Individual indv{initialize_individual(4, graph)};
    Fitness fit{};
    evaluate_fitness(graph, indv, fit);
    // print_individual(indv, fit);

    int num_bees{10};
    int max_iter{1};
    int limit{0};

    ABCGraphColoring abc = ABCGraphColoring(num_bees, num_color, max_iter, limit, graph, rng);

    abc.initialize_colony();
    // abc.print_colony();

    std::cout << "employed phase\n";
    abc.employed_bee_phase();
    abc.print_colony();

    // for (int i{0}; i < num_bees; ++i)
    // {
    //     abc.print_bee(i);
    //     abc.waggle_dance(i, graph);
    //     abc.print_bee(i);
    //     std::cout << "--\n";
    // }

    std::cout << "onlooker phase\n";
    abc.onlooker_bee_phase();
    abc.print_colony();

    abc.print_limit_no_improve();

    std::cout << "scout phase\n";
    abc.scout_bee_phase();
    abc.print_colony();

    int idx = abc.find_best_bee();
    std::cout << "best_bee: " << idx << '\n';
    abc.print_bee(idx);

    return 0;
}

// g++ main.cpp lib/graphs.cpp lib/solution.cpp lib/metaheuristics.cpp lib/utils.cpp -I include -o main