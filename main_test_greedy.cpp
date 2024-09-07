#include "graphs.h"
#include "solution.h"
#include "heuristics.h"

#include <iostream>

int main() {
    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename = "instances/le450_15a.col";
    Graph graph(filename);
    int num_color = 15;

    Individual indv{};
    Fitness fit{};

    GreedyGraphColoring greedy = GreedyGraphColoring(graph, num_color);

    indv = greedy.run();
    evaluate_fitness(graph, indv, fit);

    print_individual(indv, fit);

    return 0;
}
// g++ main_test_greedy.cpp graphs.cpp solution.cpp heuristics.cpp utils.cpp -I include -o main_test_greedy
// ./main_test_greedy