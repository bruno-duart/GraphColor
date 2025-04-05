#include "GRASPGraphColoring.h"
#include "utils.h"
#include "heuristics.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

#pragma region GRASP

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

            new_indv[v] = rcl[randint(0, rcl.size() - 1, rng)];
        }
        else
        {
            new_indv[v] = find_color_least_conflicts(new_indv, v);
        }
    }
    return new_indv;
}

void GRASPGraphColoring::LocalSearch(Individual &indv, Fitness &fit, int &acceptanceratio)
{
    Fitness old_fit{fit};

    int first_index{randint(0, graph.getNumVertices() - 1, rng)};
    int second_index{randint_diff(0, graph.getNumVertices() - 1, first_index, rng)};

    // std::cout << "first_index: " << first_index << " - second_index: " << second_index << std::endl;
    // std::cout << "first_color: " << indv[first_index] << " - second_color: " << indv[second_index] << std::endl;

    int aux{indv[first_index]};
    indv[first_index] = indv[second_index];
    indv[second_index] = aux;

    evaluate_fitness(graph, indv, fit);

    if (fit > old_fit)
    {
        // Fitness deltaE{fit - old_fit};
        // double probability{randdouble(0.0, 1.0)};
        // if (probability < std::exp(-deltaE / acceptanceratio))
        // {
        //     acceptanceratio *= 0.9;
        //     return;
        // }
        // else
        // {
        aux = indv[first_index];
        indv[first_index] = indv[second_index];
        indv[second_index] = aux;
        fit = old_fit;
        // }
    }
}

void GRASPGraphColoring::UpdateSolution(const Individual &indv, const Fitness &fit, int &acceptanceratio)
{
    evaluate_fitness(graph, best_indv, best_fit);

    if (fit < best_fit)
    {
        copy_individual(indv, fit, best_indv, best_fit);
        acceptanceratio = 0;
    }
    else
    {
        acceptanceratio++;
    }
}

Individual GRASPGraphColoring::run()
{
    Individual indv = GRASPGraphColoring::BuildPhase();
    Fitness fit;
    int acceptanceratio{1000};

    evaluate_fitness(graph, indv, fit);
    copy_individual(indv, fit, best_indv, best_fit);

    while (acceptanceratio < 100 && best_fit > 0)
    {
        GRASPGraphColoring::LocalSearch(indv, fit, acceptanceratio);
        GRASPGraphColoring::UpdateSolution(indv, fit, acceptanceratio);
    }

    return best_indv;
}

#pragma endregion
