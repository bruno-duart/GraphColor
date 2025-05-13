#ifndef HEADER_METAHEURISTICS
#define HEADER_METAHEURISTICS

#include "graphs.h"
#include "solution.h"

#include <random>

class MetaHeuristic
{
public:
    Graph graph;
    int num_colors;
    int num_iters;
    int iter_found_best;
    std::mt19937 rng;

    MetaHeuristic(const Graph &graph, int num_colors,
                  std::mt19937 &rng)
        : num_colors{num_colors}, graph{graph}, iter_found_best{0}, rng(rng)
    {
    }

    virtual ~MetaHeuristic() {}

    int find_color_least_conflicts(const Individual &indv, int current_vert);
    void perturb_best_solution(Individual &indv);
};

#endif