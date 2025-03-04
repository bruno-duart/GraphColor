#ifndef HEADER_SOLUTION
#define HEADER_SOLUTION

#include "graphs.h"
#include <iostream>
#include <vector>

typedef std::vector<int> Individual;
typedef int Fitness;

void evaluate_fitness(const Graph &graph, const Individual& indv, Fitness& fitness);

// Método para gerar uma solução aleatória
void random_individual(int num_color, const Graph &graph, Individual& indv);


// Método para copiar uma solução para outra
void copy_individual(const Individual &from, const Fitness& fit_from, Individual &to, Fitness& fit_to);

// Função para criar uma solução inicial
Individual initialize_individual(int num_color, const Graph &graph);

// Método para exibir a solução
void individual_toString(const Individual& indv); 
void print_individual(const Individual& indv, const Fitness& fit);

int compute_fitness_change(Graph &graph, Individual &ind, int vertex, int new_color);
int find_most_conflicted_vertex(Individual &ind, Graph &graph);
#endif