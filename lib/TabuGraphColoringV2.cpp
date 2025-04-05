#include "TabuGraphColoringV2.h"
#include "heuristics.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

#pragma region TabuSearch
/*
std::get<0>(entry)   // access vertex id
std::get<1>(entry)   // access previous color
std::get<2>(entry)   // access expiration_iter
*/

void TabuColoringV2::insert_tabu_move(int vertex, int from_color, int current_iter)
{
    tabu_list.push_back(std::make_tuple(vertex, from_color, current_iter + T_iter));
}
bool TabuColoringV2::is_tabu_move(int vertex, int from_color, int current_iter)
{
    for (const auto &entry : tabu_list)
    {
        if (std::get<0>(entry) == vertex && std::get<1>(entry) == from_color)
        {
            if (std::get<2>(entry) > current_iter)
            {
                return true; // Elemento is tabu
            }
        }
    }
    return false; // Not tabu (its'nt in list, or restriction ended)
}
void TabuColoringV2::decrease_iterations(int current_iter)
{
    tabu_list.remove_if([&](const auto &entry)
                        { return std::get<2>(entry) <= current_iter; });
}


void TabuColoringV2::generate_neighborhood(const Individual &indv, const Fitness &fit)
{
    neighborhood.clear();
    for (int v{0}; v < graph.getNumVertices(); v++)
    {
        if (vertex_has_conflicts(graph, indv, v))
        {
            for (int c{0}; c < num_colors; c++)
            {
                if (indv[v] != c)
                {
                    neighborhood.push_back({v, indv[v], c});
                }
            }
        }
    }
}

Individual TabuColoringV2::run()
{
    int num_iter{0};
    int num_gen_sol{(graph.getNumVertices() > 100) ? (int)(graph.getNumVertices() * 0.1) : 5};

    GreedyGraphColoring greedy = GreedyGraphColoring(graph, num_colors, rng);
    Individual indv = greedy.run_pseudo_greedy_v3();
    Fitness fit{};
    evaluate_fitness(graph, indv, fit);

    Individual best_indv{};
    Fitness best_fit{};

    copy_individual(indv, fit, best_indv, best_fit);

    int iter_total{0};
    while (best_fit > 0 && num_iter < max_iter)
    {
        int lower{0};
        int index{0};
        int undo_color{0};

        generate_neighborhood(indv, fit);

        Individual best_candidate = indv;
        Fitness best_candidate_fit = graph.getNumEdges();
        Move best_move = {-1, -1, -1};

        for (const auto &move : neighborhood)
        {
            Individual candidate = indv;
            candidate[move.vertex] = move.to_color;
            Fitness candidate_fit{0};
            evaluate_fitness(graph, candidate, candidate_fit);

            bool tabu = is_tabu_move(move.vertex, move.from_color, iter_total);
            bool aspiration = candidate_fit < best_fit; // Aspiration criteria

            if ((!tabu || aspiration) && (candidate_fit < best_candidate_fit))
            {
                best_candidate = candidate;
                best_candidate_fit = candidate_fit;
                best_move = move;
            }
        }

        // Apply best_move found so far
        if (best_move.vertex != -1)
        {
            indv = best_candidate;
            fit = best_candidate_fit;
            insert_tabu_move(best_move.vertex, best_move.from_color, iter_total);
        }
        // Clean expired tabu_move
        decrease_iterations(iter_total);
        iter_total++;

        if (fit < best_fit)
        {
            copy_individual(indv, fit, best_indv, best_fit);
            iter_found_best = iter_total;
            num_iter = 0;
        }
        else
        {
            num_iter++;
        }
    }
    return best_indv;
}

void TabuColoringV2::print_tabu_list()
{
    std::cout << "<vertex, from_color, iter_expiration>" << std::endl;
    for (const auto &entry : tabu_list)
    {
        std::cout << '<' << std::get<0>(entry)
                  << ',' << std::get<1>(entry)
                  << ',' << std::get<2>(entry)
                  << '>' << std::endl;
    }
}
#pragma endregion
