#ifndef HEADER_HEURISTICS
#define HEADER_HEURISTICS

#include "graphs.h"
#include "solution.h"

class GreedyGraphColoring
{
    Individual indv;
    Graph graph;
    int num_color;

public:
    GreedyGraphColoring(const Graph &graph, int num_colors)
        : graph{graph}, num_color{num_colors}
    {
    }

    ~GreedyGraphColoring() {}

    Individual run();

    Individual run_pseudo_greedy();

    int find_color_least_conflicts(int current_vert);
};
#endif