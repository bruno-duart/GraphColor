#ifndef HEADER_TABUGRAPHCOLORINGV2
#define HEADER_TABUGRAPHCOLORINGV2

#include "metaheuristics.h"
#include <list>
#include <tuple>

typedef struct t_move
{
    int vertex;
    int from_color;
    int to_color;
} Move;

class TabuColoringV2 : public MetaHeuristic
{
private:
    std::list<std::tuple<int, int, int>> tabu_list;
    int T_iter{};
    int tabu_tenure{};
    int max_iter{};
    std::vector<Move> neighborhood{};

public:
    TabuColoringV2(const Graph &graph, int num_colors, int max_iter, int T_iter,
                   std::mt19937 &rng)
        : MetaHeuristic(graph, num_colors, rng), T_iter{T_iter}, max_iter{max_iter}
    {
    }

    void insert_tabu_move(int vertex, int from_color, int current_iter);
    bool is_tabu_move(int vertex, int from_color, int current_iter);
    void decrease_iterations(int current_iter);
    void generate_neighborhood(const Individual &indv, const Fitness &fit);

    Individual run();

    void print_tabu_list();
};

#endif