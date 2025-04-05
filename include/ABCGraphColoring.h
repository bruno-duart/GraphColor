#ifndef HEADER_ABCGRAPHCOLORINGV2
#define HEADER_ABCGRAPHCOLORINGV2

#include "metaheuristics.h"

class ABCGraphColoring : public MetaHeuristic
{
private:
    std::vector<Individual> colony;
    std::vector<Fitness> arrayFitness;
    std::vector<int> limit_no_improve;
    std::vector<double> probabilities;
    int num_bees;
    int limit;
    int max_iter;

public:
    ABCGraphColoring(int num_bees, int num_colors, int max_iter, int limit, const Graph &graph,
                     std::mt19937 &rng)
        : MetaHeuristic(graph, num_colors, rng), num_bees{num_bees}, max_iter{max_iter}, limit{limit}
    {
        colony.resize(num_bees);
        arrayFitness.resize(num_bees);
        limit_no_improve.resize(num_bees, 0);
    }

    ~ABCGraphColoring() override
    {
    }

    void initialize_colony();

    void initialize_colony_pseudogreedy();

    void initialize_colony_grasp_buildphase(int max_rcl_size);

    void calc_probabilities();

    void employed_bee_phase(bool random = true);
    void employed_bee_phase_tabu(int T_iter, int max_iter);

    void onlooker_bee_phase();

    void scout_bee_phase();

    void waggle_dance(int idx_bee, int idx_other_bee);

    void random_choice_local_search(int index_individual);
    void swap_conflicted_vertices(int index_individual);

    int find_best_bee();

    Individual run();
    Individual run(char method, int rcl_size);
    Individual run_tabucol(int T_iter, int max_iter_tabu);

    void print_colony() const;

    void print_probabilities() const;

    void print_limit_no_improve() const;

    void print_bee(int idx) const;
    void small_mutation(int index);
};
#endif