#include "metaheuristics.h"
#include "graphs.h"
#include "solution.h"
#include "utils.h"
#include "heuristics.h"

#include <iostream>
#include <algorithm>
#include <numeric>

int MetaHeuristic::find_color_least_conflicts(const Individual &indv, int current_vert)
{
    std::vector<int> conflicts_by_color(num_colors, 0);

    for (auto neighbor : graph.adjList[current_vert])
    {
        int neighbor_color = indv[neighbor];
        if (neighbor_color != -1) // Apenas contar conflitos se o vizinho já tiver uma cor
        {
            conflicts_by_color[neighbor_color]++;
        }
    }

    int least_conflict_color{0};
    for (int i{1}; i < num_colors; ++i)
    {
        if (conflicts_by_color[i] < conflicts_by_color[least_conflict_color])
            least_conflict_color = i;
    }
    return least_conflict_color;
}

void ABCGraphColoring::initialize_colony()
{
    for (int i{0}; i < num_bees; ++i)
    {
        colony[i] = initialize_individual(num_colors, graph);
        evaluate_fitness(graph, colony[i], arrayFitness[i]);
    }
}

void ABCGraphColoring::initialize_colony_pseudogreedy()
{
    GreedyGraphColoring greedy = new GreedyGraphColoring(graph, num_colors);
    for (int i{0}; i < num_bees; ++i)
    {
        colony[i] = greedy.run_pseudo_greedy();
        evaluate_fitness(graph, colony[i], arrayFitness[i]);
    }
}

void ABCGraphColoring::initialize_colony_grasp_buildphase(int max_rcl_size)
{
    GRASPGraphColoring grasp = new GRASPGraphColoring(graph, num_colors, max_rcl_size);
    for (int i{0}; i < num_bees; ++i)
    {
        colony[i] = grasp.BuildPhase();
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
    for (int i = 0; i < num_bees / 2; ++i)
    {
        Fitness old_fit = arrayFitness[i];
        random_choice_local_search(i);
        if (old_fit <= arrayFitness[i])
        {
            limit_no_improve[i]++;
        }
    }
}

void ABCGraphColoring::onlooker_bee_phase()
{
    calc_probabilities();

    int emp_idx{0};
    int i{num_bees / 2};

    // std::cout << "emp_idx: " << emp_idx << " - i : " << i << std::endl;

    while (i < num_bees)
    {
        double r = static_cast<double>(rand()) / RAND_MAX;
        if (r < probabilities[emp_idx])
        {
            // std::cout << "in: " << i << std::endl;
            int old_fit = arrayFitness.at(i);
            waggle_dance(i, emp_idx);
            if (old_fit <= arrayFitness.at(i))
            {
                limit_no_improve[i]++;
            }
        }
        else
        {
            if (++emp_idx == num_bees / 2)
                emp_idx = 0;
        }

        ++i;
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

void ABCGraphColoring::waggle_dance(int idx_bee, int idx_other_bee)
{
    // int idx_other_bee = randint(0, (num_bees / 2) - 1);
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

void ABCGraphColoring::random_choice_local_search(int index_individual)
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

Individual ABCGraphColoring::run()
{
    initialize_colony();

    Individual best_bee{};
    Fitness best_fit{};

    int idx_best = find_best_bee();
    copy_individual(colony[idx_best], arrayFitness[idx_best], best_bee, best_fit);

    int num_iter_no_improv = 0;
    num_iters = 0;

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
            iter_found_best = num_iters;
        }
        else
        {
            num_iter_no_improv++;
        }
        num_iters++;
    }

    return best_bee;
}

Individual ABCGraphColoring::run(char method)
{
    if (method == 'p') // pseudo_greedy
    {
        initialize_colony_pseudogreedy();
    }
    else if (method == 'g') // grasp buildphase
    {
        initialize_colony_grasp_buildphase(num_colors / 2);
    }
    else // random
    {
        initialize_colony();
    }

    Individual best_bee{};
    Fitness best_fit{};

    int idx_best = find_best_bee();
    copy_individual(colony[idx_best], arrayFitness[idx_best], best_bee, best_fit);

    int num_iter_no_improv = 0;
    num_iters = 0;

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
            iter_found_best = num_iters;
        }
        else
        {
            num_iter_no_improv++;
        }
        num_iters++;
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

/////////////////////////////////////////////////////

Individual GRASPGraphColoring::BuildPhase()
{
    Individual new_indv(graph.getNumVertices(), -1);

    for (int v{0}; v < graph.getNumVertices(); ++v)
    {
        std::vector<bool> restricted_colors(num_colors, false);

        for (auto adjacent : graph.adjList[v])
        {
            if (new_indv[adjacent] != -1)
            {
                restricted_colors[new_indv[adjacent]] = true;
            }
        }

        std::vector<int> available_colors{};
        for (int i{0}; i < num_colors; ++i)
        {
            if (!restricted_colors[i])
                available_colors.push_back(i);
        }

        if (!available_colors.empty())
        {
            int rcl_size = std::min((int)available_colors.size(), max_rcl_size);

            std::vector<int> rcl;
            for (int i{0}; i < rcl_size; ++i)
                rcl.push_back(available_colors[i]);

            new_indv[v] = rcl[randint(0, rcl.size() - 1)];
        }
        else
        {
            new_indv[v] = find_color_least_conflicts(new_indv, v);
        }
    }
    return new_indv;
}

void GRASPGraphColoring::LocalSearch()
{
}

void GRASPGraphColoring::UpdateSolution()
{
}

Individual GRASPGraphColoring::run()
{
    Individual indv;
    return indv;
}