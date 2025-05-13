#ifndef HEADER_TABUGRAPHCOLORING
#define HEADER_TABUGRAPHCOLORING

#include "fwlist.h"
#include "metaheuristics.h"

class TabuColoring : public MetaHeuristic
{
private:
    ForwardListWrapper tabu_list;
    int T_iter{};
    int max_iter{};

public:
    TabuColoring(const Graph &graph, int num_colors, int max_iter, int T_iter,
                 std::mt19937 &rng)
        : MetaHeuristic(graph, num_colors, rng), T_iter{T_iter}, max_iter{max_iter}
    {
    }

    void insert_tabu_move(int undo_color, int idx_vert);
    int is_tabu_move(int undo_color, int idx_vert);
    void decrease_iterations();

    Individual run();
    /// @brief Run TabuSearch starting from an existing initial solution, and modify it instead of returning a new one
    /// @param indv
    void run(Individual &indv, Fitness &fit);

    void print_tabu_list();
};



#endif