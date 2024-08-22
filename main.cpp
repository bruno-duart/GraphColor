#include "graphs.h"
#include "solution.h" 

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
    graph.printGraph();

    Individual indv{initialize_individual(4, graph)};
    Fitness fit{};
    evaluate_fitness(graph, indv, fit);
    print_individual(indv, fit);

    return 0;
}