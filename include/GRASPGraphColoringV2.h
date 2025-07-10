#ifndef HEADER_GRASPGRAPHCOLORINGV2
#define HEADER_GRASPGRAPHCOLORINGV2

#include "metaheuristics.h"

class GRASPGraphColoringV2 : public MetaHeuristic
{
    Individual best_indv;
    Fitness best_fit;
    int max_iter;
    double alpha;

public:
    GRASPGraphColoringV2(const Graph &graph, int num_colors,
                       std::mt19937 &rng, double alpha, int max_iter)
        : MetaHeuristic(graph, num_colors, rng), alpha{alpha}, max_iter{max_iter}
    {
    }

    ~GRASPGraphColoringV2() override {}

    Individual BuildPhase();
    void LocalSearch(Individual &indv, Fitness &fit, int &acceptanceratio);
    void UpdateSolution(const Individual &indv, const Fitness &fit, int &acceptanceratio);

    bool TryKempeSwap(Individual& indv, int vertex, int color1, int color2, Fitness& current_conflicts);
    void LocalSearchKempeChain(Individual &indv);
    bool SwapVertices(Individual& indv, int& current_conflicts);
    bool OneMove(Individual& indv, int& current_conflicts);

    Individual run();
    Individual run_kempe();
    Individual run_one_move();
    Individual run_swap();
};


#endif