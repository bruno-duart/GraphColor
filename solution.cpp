#include "solution.h"
#include <iostream>
#include <vector>
#include <cstdlib> // Para usar rand()
#include <ctime>   // Para usar time()

// Método para avaliar a fitness de uma solução
void evaluate_fitness(const Graph &graph, const Individual& indv, Fitness& fitness)
{
    fitness = 0;
    for (int node=0; node < graph.getNumVertices(); node++){
        for(auto& neighboor: graph.adjList[node]){
            if(indv[node] == indv[neighboor]){
                fitness++;
            }
        }
    }
    fitness /= 2;
}

// Método para gerar uma solução aleatória
void random_individual(int num_color, const Graph &graph, Individual& indv)
{
    for (int i = 0; i < graph.getNumVertices(); i++)
        indv[i] = (rand() % num_color) + 1;

    // evaluate_fitness(graph);    
}


// Método para copiar uma solução para outra
void copy_individual(const Individual &from, const Fitness& fit_from, Individual &to, Fitness& fit_to)
{
    to = from;
    fit_to = fit_from;
}

// Função para criar uma solução inicial
Individual initialize_individual(int num_color, const Graph &graph)
{
    Individual indv(graph.getNumVertices());
    random_individual(num_color, graph, indv);
    return indv;
}

// Método para exibir a solução
void individual_toString(const Individual& indv)
{
    for (int el: indv)
        std::cout << el << " ";
    
    std::cout << std::endl;
}

void print_individual(const Individual& indv, const Fitness& fit) {
    individual_toString(indv);
    std::cout << "fit: " << fit << std::endl;
}
