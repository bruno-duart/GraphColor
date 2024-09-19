#ifndef HEADER_METAHEURISTICS
#define HEADER_METAHEURISTICS

#include "graphs.h"
#include "solution.h"
#include "utils.h"

class MetaHeuristic
{
public:
    Graph graph;
    int num_colors;
    int num_iters;
    int iter_found_best;

    MetaHeuristic(const Graph &graph, int num_colors)
        : num_colors{num_colors}, graph{graph}
    {
    }

    virtual ~MetaHeuristic() {}

    int find_color_least_conflicts(const Individual &indv, int current_vert);
};

class GRASPGraphColoring : public MetaHeuristic
{
    Individual best_indv;
    int max_rcl_size;

public:
    GRASPGraphColoring(const Graph &graph, int num_colors, int max_rcl_size)
        : MetaHeuristic(graph, num_colors), max_rcl_size{max_rcl_size}
    {
    }

    ~GRASPGraphColoring() override {}

    Individual BuildPhase();
    void LocalSearch();
    void UpdateSolution();

    Individual run();
};

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
    ABCGraphColoring(int num_bees, int num_colors, int max_iter, int limit, const Graph &graph)
        : MetaHeuristic(graph, num_colors), num_bees{num_bees}, max_iter{max_iter}, limit{limit}
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

    void employed_bee_phase();

    void onlooker_bee_phase();

    void scout_bee_phase();

    void waggle_dance(int idx_bee, int idx_other_bee);

    void random_choice_local_search(int index_individual);

    int find_best_bee();

    Individual run();
    Individual run(char method);

    void print_colony() const;

    void print_probabilities() const;

    void print_limit_no_improve() const;

    void print_bee(int idx) const;
};

#endif