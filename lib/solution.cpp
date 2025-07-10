#include "solution.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <cstdlib> // Para usar rand()
#include <ctime>   // Para usar time()
#include <random>

// Método para avaliar a fitness de uma solução
void evaluate_fitness(const Graph &graph, const Individual &indv, Fitness &fitness)
{
    fitness = 0;
    for (int node = 0; node < graph.getNumVertices(); node++)
    {
        for (auto &neighboor : graph.adjList[node])
        {
            if (indv[node] == indv[neighboor])
            {
                fitness++;
            }
        }
    }
    fitness /= 2;
}

// Método para gerar uma solução aleatória
void random_individual(int num_color, const Graph &graph, Individual &indv, std::mt19937 &rng)
{
    for (int i = 0; i < graph.getNumVertices(); i++)
    {
        // indv[i] = (rand() % num_color) + 1;
        indv[i] = randint(0, num_color - 1, rng);
    }

    // evaluate_fitness(graph);
}

// Método para copiar uma solução para outra
void copy_individual(const Individual &from, const Fitness &fit_from, Individual &to, Fitness &fit_to)
{
    to = from;
    fit_to = fit_from;
}

// Função para criar uma solução inicial
Individual initialize_individual(int num_color, const Graph &graph, std::mt19937 &rng)
{
    Individual indv(graph.getNumVertices());
    random_individual(num_color, graph, indv, rng);
    return indv;
}

// Método para exibir a solução
void individual_toString(const Individual &indv)
{
    for (int el : indv)
        std::cout << el << " ";

    std::cout << std::endl;
}

void print_individual(const Individual &indv, const Fitness &fit)
{
    individual_toString(indv);
    std::cout << "fit: " << fit << std::endl;
}

int compute_fitness_change(Graph &graph, Individual &ind, int vertex, int new_color)
{
    int old_color = ind[vertex];
    int conflict_change = 0;

    // Percorre os vizinhos do vértice
    for (int neighbor : graph.getNeighbors(vertex))
    {
        if (ind[neighbor] == old_color)
        {
            // Removendo um conflito
            conflict_change--;
        }
        if (ind[neighbor] == new_color)
        {
            // Adicionando um conflito
            conflict_change++;
        }
    }

    return conflict_change;
}

int compute_swap_fitness_change(Graph &graph, Individual &indv, int u, int v){
    int delta{0};
    int color_u{indv[u]};
    int color_v{indv[v]};

    for (auto neighbor: graph.getNeighbors(u)){
        if (neighbor == v) continue;
        if (indv[neighbor] == color_u) delta--; // removendo conflito antigo
        if (indv[neighbor] == color_v) delta++; // possível novo conflito
    }

    for (auto neighbor: graph.getNeighbors(v)){
        if (neighbor == u) continue;
        if (indv[neighbor] == color_v) delta--; // removendo conflito antigo
        if (indv[neighbor] == color_u) delta++; // possível novo conflito
    }

    return delta;
}

int find_most_conflicted_vertex(Individual &ind, Graph &graph)
{
    int worst_vertex = -1;
    int max_conflicts = -1;

    for (int v = 0; v < graph.getNumVertices(); v++)
    {
        int conflicts = 0;
        for (int neighbor : graph.getNeighbors(v))
        {
            if (ind[v] == ind[neighbor])
            {
                conflicts++;
            }
        }

        if (conflicts > max_conflicts)
        {
            max_conflicts = conflicts;
            worst_vertex = v;
        }
    }

    return worst_vertex;
}

int find_second_most_conflicted_vertex(Individual &indv, Graph &graph, int most_conflicted)
{
    int second_worst_vertex = -1;
    int second_max_conflicts = -1;

    for (int v = 0; v < graph.getNumVertices(); v++)
    {
        if (v == most_conflicted)
            continue; // Skip the most conflicted vertex

        int conflicts = 0;
        for (int neighbor : graph.getNeighbors(v))
        {
            if (indv[v] == indv[neighbor])
            {
                conflicts++;
            }
        }

        if (conflicts > second_max_conflicts)
        {
            second_max_conflicts = conflicts;
            second_worst_vertex = v;
        }
    }

    return second_worst_vertex;
}

void explore_neighborhood(Individual &new_indv, const Individual &indv, Fitness &new_fit, const Fitness &fit, const Graph &graph, int num_colors, std::mt19937 &rng)
{
    int vertex{randint(0, graph.getNumVertices() - 1, rng)};
    copy_individual(indv, fit, new_indv, new_fit);
    int new_color = randint_diff(0, num_colors, new_indv[vertex], rng);
    new_indv[vertex] = new_color;
    evaluate_fitness(graph, new_indv, new_fit);
}

// void explore_neighborhood(Individual &new_indv, const Individual &indv, Fitness &new_fit, const Fitness &fit,
//                           const Graph &graph, int num_colors, std::mt19937 &rng, double curr_Temp)
// {
//     copy_individual(indv, fit, new_indv, new_fit);

//     int num_changes = (curr_Temp > 1.0) ? 2 : 1;

//     int max_changes{4};
//     int min_changes

//     for (int i = 0; i < num_changes; ++i)
//     {
//         int vertex = randint(0, graph.getNumVertices() - 1, rng);
//         int new_color = randint_diff(0, num_colors, new_indv[vertex], rng);
//         new_indv[vertex] = new_color;
//     }

//     evaluate_fitness(graph, new_indv, new_fit);
// }

bool vertex_has_conflicts(Graph &graph, const Individual &indv, const int &vertex_id)
{
    for (auto v : graph.getNeighbors(vertex_id))
    {
        if (indv[v] == indv[vertex_id])
        {
            return true;
        }
    }
    return false;
}