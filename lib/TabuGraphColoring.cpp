#include "TabuGraphColoring.h"
#include "utils.h"
#include "heuristics.h"
#include "fwlist.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>

#pragma region TabuSearch
void TabuColoring::insert_tabu_move(int undo_color, int idx_vert)
{
    tabu_list.push_back(idx_vert, undo_color, T_iter);
}
int TabuColoring::is_tabu_move(int undo_color, int idx_vert)
{
    int position{0};
    for (const auto &node : tabu_list.get_list())
    {
        if (node.index_i == idx_vert && node.undo_color == undo_color)
            return position;
        position++;
    }
    return -1;
}
void TabuColoring::decrease_iterations()
{
    auto &flist = tabu_list.get_list();
    auto prev = flist.before_begin();

    for (auto it = flist.begin(); it != flist.end();)
    {
        it->count_iter--;
        if (it->count_iter == 0)
        {
            it = flist.erase_after(prev);
            tabu_list.decrease_size();
        }
        else
        {
            prev = it;
            ++it;
        }
    }
}

Individual TabuColoring::run()
{
    int num_iter{0};
    int num_gen_sol{(graph.getNumVertices() > 100) ? (int)(graph.getNumVertices() * 0.1) : 5};

    std::vector<Individual> arr_solutions;
    std::vector<Fitness> arr_fit;

    GreedyGraphColoring greedy = GreedyGraphColoring(graph, num_colors, rng);
    Individual indv = greedy.run_pseudo_greedy_v3();
    // Individual indv = initialize_individual(num_colors, graph);
    Fitness fit{};
    evaluate_fitness(graph, indv, fit);

    Individual best_indv{};
    Fitness best_fit{};

    copy_individual(indv, fit, best_indv, best_fit);

    for (int i{0}; i < num_gen_sol; ++i)
    {
        arr_solutions.push_back(Individual(graph.getNumVertices()));
        arr_fit.push_back(graph.getNumEdges());
    }

    int iter_total{0};
    while (best_fit > 0 && num_iter < max_iter)
    {
        int lower{0};
        int index{0};
        int undo_color{0};

        for (int i{0}; i < num_gen_sol; ++i)
        {
            explore_neighborhood(arr_solutions[i], indv, arr_fit[i], fit, graph, num_colors, rng);

            if (arr_fit[i] < arr_fit[lower])
            {
                lower = i;
            }
        }

        for (int vertex{0}; vertex < graph.getNumVertices(); ++vertex)
        {
            if (arr_solutions[lower][vertex] != indv[vertex])
            {
                undo_color = indv[vertex];
                index = vertex;
                break;
            }
        }

        int is_tabu{is_tabu_move(undo_color, index)};
        if (is_tabu == 0)
        {
            copy_individual(arr_solutions[lower], arr_fit[lower], indv, fit);
            insert_tabu_move(undo_color, index);
        }
        else if (arr_fit[lower] < fit)
        {
            tabu_list.list_erase(is_tabu);
            copy_individual(arr_solutions[lower], arr_fit[lower], indv, fit);
        }
        decrease_iterations();

        if (fit < best_fit)
        {
            num_iter = 0;
            copy_individual(indv, fit, best_indv, best_fit);
            iter_found_best = iter_total;
        }
        else
        {
            num_iter++;
        }
        iter_total++;
    }
    return best_indv;
}

void TabuColoring::run(Individual &indv, Fitness &fit)
{
    int num_iter{0};
    int num_gen_sol{(graph.getNumVertices() > 100) ? (int)(graph.getNumVertices() * 0.1) : 5};

    std::vector<Individual> arr_solutions;
    std::vector<Fitness> arr_fit;

    Individual best_indv{};
    Fitness best_fit{};

    copy_individual(indv, fit, best_indv, best_fit);

    for (int i{0}; i < num_gen_sol; ++i)
    {
        arr_solutions.push_back(Individual(graph.getNumVertices()));
        arr_fit.push_back(graph.getNumEdges());
    }

    int iter_total{0};
    while (best_fit > 0 && num_iter < max_iter)
    {
        int lower{0};
        int index{0};
        int undo_color{0};

        for (int i{0}; i < num_gen_sol; ++i)
        {
            explore_neighborhood(arr_solutions[i], indv, arr_fit[i], fit, graph, num_colors, rng);

            if (arr_fit[i] < arr_fit[lower])
            {
                lower = i;
            }
        }

        for (int vertex{0}; vertex < graph.getNumVertices(); ++vertex)
        {
            if (arr_solutions[lower][vertex] != indv[vertex])
            {
                undo_color = indv[vertex];
                index = vertex;
                break;
            }
        }

        int is_tabu{is_tabu_move(undo_color, index)};
        if (is_tabu == 0)
        {
            copy_individual(arr_solutions[lower], arr_fit[lower], indv, fit);
            insert_tabu_move(undo_color, index);
        }
        else if (arr_fit[lower] < fit)
        {
            tabu_list.list_erase(is_tabu);
            copy_individual(arr_solutions[lower], arr_fit[lower], indv, fit);
        }
        decrease_iterations();

        if (fit < best_fit)
        {
            num_iter = 0;
            copy_individual(indv, fit, best_indv, best_fit);
            iter_found_best = iter_total;
        }
        else
        {
            num_iter++;
        }
        iter_total++;
    }
    copy_individual(best_indv, best_fit, indv, fit);
}

void TabuColoring::print_tabu_list()
{
    tabu_list.print();
}
#pragma endregion
