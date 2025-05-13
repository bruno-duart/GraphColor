#ifndef HEADER_ABCGRAPHCOLORINGV2
#define HEADER_ABCGRAPHCOLORINGV2

#include "metaheuristics.h"

class ABCGraphColoringV2 : public MetaHeuristic
{
private:
    std::vector<Individual> employedBees;
    std::vector<Individual> onlookerBees;
    std::vector<Fitness> employedsFitness;
    std::vector<Fitness> onlookersFitness;
    std::vector<int> limit_no_improve;
    std::vector<double> fitnessForMinimization;
    int num_bees;
    int num_employed;
    int num_onlooker;
    int limit;
    int max_iter;
    double threshold;
    double temperature;
    double acceptance_probability;
    double cooling_rate;

public:
    ABCGraphColoringV2(int num_bees, int num_colors, int max_iter, int limit, const Graph &graph,
                       std::mt19937 &rng)
        : MetaHeuristic(graph, num_colors, rng), num_bees{num_bees}, max_iter{max_iter}, limit{limit}
    {
        num_employed = num_bees / 2;
        num_onlooker = num_bees / 2;
        employedBees.resize(num_employed);
        onlookerBees.resize(num_onlooker);
        employedsFitness.resize(num_employed);
        onlookersFitness.resize(num_onlooker);
        limit_no_improve.resize(num_employed, 0);
        fitnessForMinimization.resize(num_employed);
        acceptance_probability = 0.3;
        temperature = 100.0;
        cooling_rate = 0.9;
        threshold = 0.7;
    }

    ~ABCGraphColoringV2() override
    {
    }
    bool is_similar_to_existing(const Individual &indv, int num_existing);
    void initialize_employedBees_random();
    void initialize_employedBees_pseudoGreedy();
    void initialize_employedBees_pseudoGreedy_v2();
    void initialize_employedBees_pseudoGreedy_v3();
    void update_fitness_for_minimization();
    int roulette_wheel_choose_bee();

    void random_choice_local_search(int index_individual, Individual &indv, Fitness &fit);
    void enhanced_random_search(int index_individual, Individual &indv, Fitness &current_fitness);
    void swap_conflicted_vertices(Individual &indv, Fitness &fit);
    /// @brief Sempre aplica em cima da abelha selecionada e tenta aprender com as empregadas
    /// @param idx_bee
    /// @param bees
    /// @param fits
    void waggle_dance(int idx_bee, Individual &bee, Fitness &fit);
    void waggle_dance_annealing(int idx_bee, Individual &bee, Fitness &fit);
    void employed_bee_phase();
    void employed_bee_phase_enhanced();
    void employed_bee_phase_tabu(int T_iter, int max_iter_tabu);
    void employed_bee_phase_annealing();
    void onlooker_bee_phase();
    void onlooker_bee_phase_enhanced();
    void onlooker_bee_phase_tabu(int T_iter, int max_iter_tabu);
    void onlooker_bee_phase_annealing();
    void scout_bee_phase();
    void scout_bee_phase_enhanced(Individual &best_bee, Fitness &best_fit);
    int find_best_bee();

    Individual run(char method_init);
    Individual run_enhanced(char method_init);
    Individual run_annealing(char method_init);
    Individual run_tabucol(char method_init, int T_iter, int max_iter_tabu);
    Individual return_bee(int idx_bee);

    void print_colony() const;
    void print_fitnessForMinimization() const;
    void print_limit_no_improve() const;
    void print_employedBee(int idx) const;
    void print_onlookerBee(int idx) const;
};

#endif