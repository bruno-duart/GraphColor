#ifndef HEADER_GRASPGRAPHCOLORING
#define HEADER_GRASPGRAPHCOLORING

#include "metaheuristics.h"

class GRASPGraphColoring : public MetaHeuristic
{
    Individual best_indv;
    Fitness best_fit;
    int max_rcl_size;

public:
    GRASPGraphColoring(const Graph &graph, int num_colors,
                       std::mt19937 &rng, int max_rcl_size)
        : MetaHeuristic(graph, num_colors, rng), max_rcl_size{max_rcl_size}
    {
    }

    ~GRASPGraphColoring() override {}

    Individual BuildPhase();
    void LocalSearch(Individual &indv, Fitness &fit, int &acceptanceratio);
    void UpdateSolution(const Individual &indv, const Fitness &fit, int &acceptanceratio);

    Individual run();
};


#endif