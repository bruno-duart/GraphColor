#include "metaheuristics.h"
#include "graphs.h"
#include "solution.h"
#include "utils.h"

#include <iostream>
#include <algorithm>
#include <numeric>

void ABCGraphColoring::initialize_colony()
{
    for (int i = 0; i < num_bees; ++i)
    {
        colony[i] = initialize_individual(num_colors, graph);
        evaluate_fitness(graph, colony[i], arrayFitness[i]);
    }
}

void ABCGraphColoring::calc_probabilities()
{
    int sum_fit = std::accumulate(arrayFitness.begin(), arrayFitness.end(), 0);

    probabilities.clear();
    for (int &fitness : arrayFitness)
    {
        probabilities.push_back(static_cast<double>(fitness) / sum_fit);
    }
}

void ABCGraphColoring::employed_bee_phase()
{
    for (int i = 0; i < num_bees; ++i)
    {
        Fitness old_fit = arrayFitness[i];
        random_choice_local_search(i, graph, num_colors);
        if (old_fit <= arrayFitness[i])
        {
            limit_no_improve[i]++;
        }
    }
}

void ABCGraphColoring::onlooker_bee_phase()
{
    calc_probabilities();

    for (int i = 0; i < num_bees; ++i)
    {
        double r = static_cast<double>(rand()) / RAND_MAX;
        if (r < probabilities[i])
        {
            int old_fit = arrayFitness.at(i);
            waggle_dance(i, graph);
            if (old_fit <= arrayFitness.at(i))
            {
                limit_no_improve[i]++;
            }
        }
    }
}

void ABCGraphColoring::scout_bee_phase()
{
    for (int i = 0; i < num_bees; ++i)
    {
        if (limit_no_improve[i] > limit)
        {
            random_individual(num_colors, graph, colony.at(i));
            evaluate_fitness(graph, colony.at(i), arrayFitness[i]);
            limit_no_improve[i] = 0;
        }
    }
}

void ABCGraphColoring::waggle_dance(int idx_bee)
{
    int idx_other_bee = randint_diff(0, num_bees - 1, idx_bee);
    int rand_vertex = randint(0, graph.getNumVertices() - 1);

    int old_color = colony.at(idx_bee)[rand_vertex];
    int old_fit = arrayFitness.at(idx_bee);

    colony.at(idx_bee)[rand_vertex] = colony.at(idx_other_bee)[rand_vertex];

    evaluate_fitness(graph, colony.at(idx_bee), arrayFitness.at(idx_bee));

    if (arrayFitness.at(idx_bee) > old_fit)
    {
        colony.at(idx_bee)[rand_vertex] = old_color;
        arrayFitness.at(idx_bee) = old_fit;
    }
}

void ABCGraphColoring::random_choice_local_search(int index_individual, int num_colors)
{
    int rand_vertex = randint(0, graph.getNumVertices() - 1);
    int old_color = colony.at(index_individual)[rand_vertex];
    int old_fit = arrayFitness.at(index_individual);

    colony.at(index_individual)[rand_vertex] = randint_diff(1, num_colors, old_color);
    evaluate_fitness(graph, colony.at(index_individual), arrayFitness.at(index_individual));

    if (arrayFitness.at(index_individual) > old_fit)
    {
        colony.at(index_individual)[rand_vertex] = old_color;
        arrayFitness.at(index_individual) = old_fit;
    }
}

int ABCGraphColoring::find_best_bee()
{
    int idx{0};
    for (int i{1}; i < num_bees; ++i)
        if (arrayFitness[i] < arrayFitness[idx])
            idx = i;

    return idx;
}

Individual ABCGraphColoring::run(int *last_iter)
{
    initialize_colony();

    Individual best_bee{};
    Fitness best_fit{};

    int idx_best = find_best_bee();
    copy_individual(colony[idx_best], arrayFitness[idx_best], best_bee, best_fit);

    int num_iter_no_improv = 0;
    while (num_iter_no_improv < max_iter)
    {
        employed_bee_phase();
        onlooker_bee_phase();
        scout_bee_phase();

        int current_best_bee = find_best_bee();
        if (best_fit > arrayFitness[current_best_bee])
        {
            copy_individual(colony[current_best_bee], arrayFitness[current_best_bee], best_bee, best_fit);
            num_iter_no_improv = 0;
        }
        else
        {
            num_iter_no_improv++;
        }
        (*last_iter)++;
    }

    return best_bee;
}

void ABCGraphColoring::print_colony() const
{
    for (int i{0}; i < num_bees; ++i)
    {
        std::cout << i << ": ";
        print_individual(colony.at(i), arrayFitness[i]);
        std::cout << "--\n";
    }
}

void ABCGraphColoring::print_probabilities() const
{
    for (double prob : probabilities)
    {
        std::cout << prob << " ";
    }
    std::cout << "sum: " << std::accumulate(probabilities.begin(), probabilities.end(), 0.0) << std::endl;
}

void ABCGraphColoring::print_limit_no_improve() const
{
    std::cout << "limits_array:\n";
    for (int limits : limit_no_improve)
    {
        std::cout << limits << " ";
    }
    std::cout << std::endl;
}

void ABCGraphColoring::print_bee(int idx) const
{
    print_individual(colony.at(idx), arrayFitness[idx]);
}