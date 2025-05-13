#ifndef HEADER_HEURISTICS
#define HEADER_HEURISTICS

#include "graphs.h"
#include "solution.h"
#include <random>

class GreedyGraphColoring
{
    Individual indv;
    Graph graph;
    int num_color;    
    std::mt19937 rng;

public:
    GreedyGraphColoring(const Graph &graph, int num_colors, 
        std::mt19937 rng)
        : graph{graph}, num_color{num_colors}, rng(rng)
    {
    }

    ~GreedyGraphColoring() {}

    Individual run();

    Individual run_pseudo_greedy();
    Individual run_pseudo_greedy_v2();
    Individual run_pseudo_greedy_v3();

    int find_color_least_conflicts(int current_vert);
};
#endif