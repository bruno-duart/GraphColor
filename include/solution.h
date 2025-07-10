#ifndef HEADER_SOLUTION
#define HEADER_SOLUTION

#include "graphs.h"
#include <iostream>
#include <vector>
#include <random>

typedef std::vector<int> Individual;
typedef int Fitness;

void evaluate_fitness(const Graph &graph, const Individual& indv, Fitness& fitness);

// Método para gerar uma solução aleatória
void random_individual(int num_color, const Graph &graph, Individual& indv, std::mt19937 &rng);


// Método para copiar uma solução para outra
void copy_individual(const Individual &from, const Fitness& fit_from, Individual &to, Fitness& fit_to);

// Função para criar uma solução inicial
Individual initialize_individual(int num_color, const Graph &graph, std::mt19937 &rng);

// Método para exibir a solução
void individual_toString(const Individual& indv); 
void print_individual(const Individual& indv, const Fitness& fit);

int compute_fitness_change(Graph &graph, Individual &ind, int vertex, int new_color);
int compute_swap_fitness_change(Graph &graph, Individual &indv, int u, int v);
int find_most_conflicted_vertex(Individual &ind, Graph &graph);
int find_second_most_conflicted_vertex(Individual &indv, Graph &graph, int most_conflicted);

void explore_neighborhood(Individual& new_indv, const Individual& indv, Fitness& new_fit, const Fitness& fit, const Graph& graph, int num_colors, std::mt19937& rng);
bool vertex_has_conflicts(Graph &graph, const Individual &indv, const int &vertex_id);
#endif