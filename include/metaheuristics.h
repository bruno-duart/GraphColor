#ifndef HEADER_METAHEURISTICS
#define HEADER_METAHEURISTICS

#include "graphs.h"
#include "solution.h"
#include "utils.h"

class ABCGraphColoring
{
private:
    std::vector<Individual> colony;
    std::vector<Fitness> arrayFitness;
    std::vector<int> limit_no_improve;
    std::vector<double> probabilities;
    int num_bees;
    int limit;
    int max_iter;

    int num_colors;

public:
    ABCGraphColoring(int num_bees, int num_colors, int max_iter, int limit)
        : num_bees{num_bees}, num_colors{num_colors}, max_iter{max_iter}, limit{limit}
    {
        colony.resize(num_bees);
        arrayFitness.resize(num_bees);
        limit_no_improve.resize(num_bees, 0);
    }

    ~ABCGraphColoring()
    {
    }

    void initialize_colony(const Graph &graph);

    void calc_probabilities();

    void employed_bee_phase(const Graph &graph);

    void onlooker_bee_phase(const Graph &graph);

    void scout_bee_phase(const Graph &graph);

    void waggle_dance(int idx_bee, const Graph &graph);

    void random_choice_local_search(int index_individual, const Graph &graph, int num_colors);

    int find_best_bee();

    Individual run(int *last_iter, const Graph &graph);

    void print_colony() const;

    void print_probabilities() const;

    void print_limit_no_improve() const;

    void print_bee(int idx) const;
};

#endif