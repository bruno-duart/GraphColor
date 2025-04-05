#include "SAGraphColoring.h"
#include "utils.h"
#include "heuristics.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

#pragma region SimulatedAnnealing
Individual SimulatedAnnealingColoring::run()
{

    GreedyGraphColoring greedy = GreedyGraphColoring(graph, num_colors, rng);
    Individual indv = greedy.run();
    //Individual indv = initialize_individual(num_colors, graph);
    Fitness fit{};
    evaluate_fitness(graph, indv, fit);

    Individual best_indv{};
    Fitness best_fit{};
    copy_individual(indv, fit, best_indv, best_fit);

    Individual eval_indv{};
    Fitness eval_fit{};

    int deltaE;
    int num_iter{0};
    int num_worsen{};
    double probability{};
    curr_Temp = Temp_Max;
    int iter_count{0};

    do
    {
        num_worsen = 0;
        do
        {
            explore_neighborhood(eval_indv, indv, eval_fit, fit, graph, num_colors, rng);
            deltaE = eval_fit - fit;

            probability = randdouble(0.0, RAND_MAX, rng) / (double)RAND_MAX;

            if (deltaE < 0)
            {
                copy_individual(eval_indv, eval_fit, indv, fit);
                num_iter = 0;
                if (fit < best_fit)
                {
                    iter_found_best = iter_count;
                    copy_individual(indv, fit, best_indv, best_fit);
                }
            }
            else if (probability < exp(-deltaE / ((double)curr_Temp)))
            {
                num_iter = 0;
                num_worsen++;
                copy_individual(indv, fit, eval_indv, eval_fit);
            }
            else
            {
                num_iter++;
            }
            
            iter_count++;
            if (best_fit == 0) {
                break;
            }
        } while ((num_iter < Max_iter_no_improv) && (num_worsen < Max_worsen));

        if (best_fit == 0){
            break;
        }
        curr_Temp *= alpha;
    } while (curr_Temp > Temp_Min);
    return best_indv;
}
#pragma endregion