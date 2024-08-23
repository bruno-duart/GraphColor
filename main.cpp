#include "graphs.h"
#include "solution.h"
#include "metaheuristics.h"

#include <iostream>
#include <cstdlib> // Para usar rand()
#include <ctime>   // Para usar time()

std::vector<Fitness> fitness;

int main()
{
    srand(time(nullptr));

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

    ABCGraphColoring abc = ABCGraphColoring(num_bees, num_color, max_iter, limit);

    abc.initialize_colony(graph);
    // abc.print_colony();

    std::cout << "employed phase\n";
    abc.employed_bee_phase(graph);
    abc.print_colony();

    // for (int i{0}; i < num_bees; ++i)
    // {
    //     abc.print_bee(i);
    //     abc.waggle_dance(i, graph);
    //     abc.print_bee(i);
    //     std::cout << "--\n";
    // }

    std::cout << "onlooker phase\n";
    abc.onlooker_bee_phase(graph);
    abc.print_colony();

    abc.print_limit_no_improve();

    std::cout << "scout phase\n";
    abc.scout_bee_phase(graph);
    abc.print_colony();

    int idx = abc.find_best_bee();
    std::cout << "best_bee: " << idx << '\n';
    abc.print_bee(idx);

    return 0;
}

// g++ main.cpp graphs.cpp solution.cpp metaheuristics.cpp utils.cpp -I include -o main