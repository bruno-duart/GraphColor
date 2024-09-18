#include "graphs.h"
#include "solution.h"
#include "heuristics.h"

#include <iostream>
#include <cstdlib> // Para usar rand()
#include <ctime>   // Para usar time()
#include <chrono>

using high_clock = std::chrono::high_resolution_clock;
using us = std::chrono::microseconds;

int main()
{
    srand(time(nullptr));
    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename = "instances/myciel3.col";
    Graph graph(filename);
    int num_color = 4;

    Individual indv{};
    Fitness fit{};

    GreedyGraphColoring greedy = GreedyGraphColoring(graph, num_color);

    std::cout << "Pseudo-greedy:\n";
    auto time = high_clock::now();
    indv = greedy.run_pseudo_greedy();
    auto time2 = high_clock::now();
    evaluate_fitness(graph, indv, fit);
    print_individual(indv, fit);
    std::cout << "Time Pseudo: " << std::chrono::duration_cast<us>(time2 - time).count() << "us" << std::endl;

    std::cout << "Greedy:\n";
    time = high_clock::now();
    indv = greedy.run();
    time2 = high_clock::now();
    evaluate_fitness(graph, indv, fit);
    print_individual(indv, fit);
    std::cout << "Time Pseudo: " << std::chrono::duration_cast<us>(time2 - time).count() << "us" << std::endl;

    return 0;
}
// g++ main_test_greedy.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/utils.cpp -I include -o main_test_greedy
// ./main_test_greedy