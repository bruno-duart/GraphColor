#include "metaheuristics.h"
#include "graphs.h"
#include "solution.h"
#include "utils.h"
#include "heuristics.h"
#include "fwlist.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

#pragma region MetaHeuristic
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

#pragma endregion

#pragma region ABCGraphColoring

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
    GreedyGraphColoring greedy = GreedyGraphColoring(graph, num_colors);
    for (int i{0}; i < num_bees; ++i)
    {
        colony[i] = greedy.run_pseudo_greedy();
        evaluate_fitness(graph, colony[i], arrayFitness[i]);
    }
}

void ABCGraphColoring::initialize_colony_grasp_buildphase(int max_rcl_size)
{
    GRASPGraphColoring grasp = GRASPGraphColoring(graph, num_colors, max_rcl_size);
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

void ABCGraphColoring::employed_bee_phase(bool random)
{
    for (int i = 0; i < num_bees / 2; ++i)
    {
        Fitness old_fit = arrayFitness[i];
        if (random)
            random_choice_local_search(i);
        else
            swap_conflicted_vertices(i);
        if (old_fit <= arrayFitness[i])
        {
            limit_no_improve[i]++;
        }
    }
}

// void ABCGraphColoring::onlooker_bee_phase()
// {
//     calc_probabilities();

//     int emp_idx{0};
//     int i{num_bees / 2};

//     // std::cout << "emp_idx: " << emp_idx << " - i : " << i << std::endl;

//     while (i < num_bees)
//     {
//         double r = static_cast<double>(rand()) / RAND_MAX;
//         if (r < probabilities[emp_idx])
//         {
//             // std::cout << "in: " << i << std::endl;
//             int old_fit = arrayFitness.at(i);
//             waggle_dance(i, emp_idx);
//             if (old_fit <= arrayFitness.at(i))
//             {
//                 limit_no_improve[i]++;
//             }
//         }
//         else
//         {
//             if (++emp_idx == num_bees / 2)
//                 emp_idx = 0;
//         }

//         ++i;
//     }
// }

void ABCGraphColoring::onlooker_bee_phase()
{
    calc_probabilities(); // Atualiza probabilidades com base na fitness

    int emp_idx{0};
    int i{num_bees / 2};

    while (i < num_bees)
    {
        double r = static_cast<double>(rand()) / RAND_MAX;

        // Critério híbrido: 80% roleta, 20% escolha aleatória
        if (r < 0.8)
        {
            if (r < probabilities[emp_idx])
            {
                int old_fit = arrayFitness.at(i);
                waggle_dance(i, emp_idx);

                if (old_fit <= arrayFitness.at(i))
                {
                    limit_no_improve[i]++;
                }
            }
        }
        else
        {
            // Escolhe aleatoriamente um empregado para seguir, garantindo diversidade
            int rand_emp_idx = randint(0, num_bees / 2 - 1);
            int old_fit = arrayFitness.at(i);
            waggle_dance(i, rand_emp_idx);

            if (old_fit <= arrayFitness.at(i))
            {
                limit_no_improve[i]++;
            }
        }

        // Ajuste do índice dos empregados
        if (++emp_idx == num_bees / 2)
            emp_idx = 0;

        ++i;
    }
}

// void ABCGraphColoring::scout_bee_phase()
// {
//     for (int i = 0; i < num_bees; ++i)
//     {
//         if (limit_no_improve[i] > limit)
//         {
//             random_individual(num_colors, graph, colony.at(i));
//             evaluate_fitness(graph, colony.at(i), arrayFitness[i]);
//             limit_no_improve[i] = 0;
//         }
//     }
// }

void ABCGraphColoring::scout_bee_phase()
{
    static int global_no_improve = 0; // Contador global de iterações sem melhora

    for (int i = 0; i < num_bees; ++i)
    {
        if (limit_no_improve[i] > limit)
        {
            // 🔹 Critério adaptativo: se ninguém melhorou há muitas iterações, diminuir 'limit'
            if (++global_no_improve > 10)
            {
                limit = std::max(5, limit - 1); // Reduz limite (mínimo 5)
                global_no_improve = 0;
            }

            // 🔹 Em vez de reset completo, perturba ligeiramente antes de recriar a solução
            if (rand() % 100 < 30) // 30% das vezes, tenta pequena mutação
            {
                small_mutation(i);
            }
            else
            {
                random_individual(num_colors, graph, colony.at(i));
                evaluate_fitness(graph, colony.at(i), arrayFitness[i]);
            }

            limit_no_improve[i] = 0; // Reset contador da abelha
        }
    }
}

void ABCGraphColoring::small_mutation(int index)
{
    int rand_vertex = randint(0, graph.getNumVertices() - 1);
    colony.at(index)[rand_vertex] = randint(1, num_colors); // Escolhe cor aleatória
    evaluate_fitness(graph, colony.at(index), arrayFitness[index]);
}

void ABCGraphColoring::waggle_dance(int idx_bee, int idx_other_bee)
{
    // int idx_other_bee = randint(0, (num_bees / 2) - 1);
    int rand_vertex = randint(0, graph.getNumVertices() - 1);

    int old_color = colony.at(idx_bee)[rand_vertex];
    int old_fit = arrayFitness.at(idx_bee);
    int new_color = colony.at(idx_other_bee)[rand_vertex];

    /*  colony.at(idx_bee)[rand_vertex] = colony.at(idx_other_bee)[rand_vertex];

        evaluate_fitness(graph, colony.at(idx_bee), arrayFitness.at(idx_bee));

        if (arrayFitness.at(idx_bee) > old_fit)
        {
            colony.at(idx_bee)[rand_vertex] = old_color;
            arrayFitness.at(idx_bee) = old_fit;
        }
    */

    // Calcula a mudança na fitness
    int delta_fitness = compute_fitness_change(graph, colony[idx_bee], rand_vertex, new_color);

    // Aplica a mudança
    colony[idx_bee][rand_vertex] = new_color;
    arrayFitness[idx_bee] += delta_fitness;

    // Se a mudança piorou, reverte
    if (delta_fitness > 0)
    {
        colony[idx_bee][rand_vertex] = old_color;
        arrayFitness[idx_bee] -= delta_fitness;
    }
}

// void ABCGraphColoring::random_choice_local_search(int index_individual)
// {
//     int rand_vertex = randint(0, graph.getNumVertices() - 1);
//     int old_color = colony.at(index_individual)[rand_vertex];
//     int old_fit = arrayFitness.at(index_individual);
//     int new_color = randint_diff(1, num_colors, old_color);

//     /*colony.at(index_individual)[rand_vertex] = randint_diff(1, num_colors, old_color);
//     evaluate_fitness(graph, colony.at(index_individual), arrayFitness.at(index_individual));

//     if (arrayFitness.at(index_individual) > old_fit)
//     {
//         colony.at(index_individual)[rand_vertex] = old_color;
//         arrayFitness.at(index_individual) = old_fit;
//     }*/
//     // Calcula a mudança na fitness
//     int delta_fitness = compute_fitness_change(graph, colony[index_individual], rand_vertex, new_color);

//     // Aplica a mudança
//     colony[index_individual][rand_vertex] = new_color;
//     arrayFitness[index_individual] += delta_fitness;

//     // Se a mudança piorou, reverte
//     if (delta_fitness > 0)
//     {
//         colony[index_individual][rand_vertex] = old_color;
//         arrayFitness[index_individual] -= delta_fitness;
//     }
// }

void ABCGraphColoring::random_choice_local_search(int index_individual)
{
    int rand_vertex = find_most_conflicted_vertex(colony[index_individual], graph);

    if (rand_vertex == -1)
        return; // Nenhum conflito encontrado

    int old_color = colony.at(index_individual)[rand_vertex];
    int new_color = randint_diff(1, num_colors, old_color);

    int delta_fitness = compute_fitness_change(graph, colony[index_individual], rand_vertex, new_color);

    // Aplica a mudança
    colony[index_individual][rand_vertex] = new_color;
    arrayFitness[index_individual] += delta_fitness;

    // Se a mudança piorou, reverte
    if (delta_fitness > 0)
    {
        colony[index_individual][rand_vertex] = old_color;
        arrayFitness[index_individual] -= delta_fitness;
    }
}

void ABCGraphColoring::swap_conflicted_vertices(int index_individual)
{
    int v1 = find_most_conflicted_vertex(colony[index_individual], graph);
    if (v1 == -1)
        return; // Nenhum conflito encontrado

    // Escolhe um vizinho com conflito
    int v2 = -1;
    for (int neighbor : graph.getNeighbors(v1))
    {
        if (colony[index_individual][neighbor] == colony[index_individual][v1])
        {
            v2 = neighbor;
            break;
        }
    }
    if (v2 == -1)
        return; // Nenhum vizinho conflitante encontrado

    // Troca as cores
    std::swap(colony[index_individual][v1], colony[index_individual][v2]);

    // Atualiza a fitness incrementalmente
    int delta_fitness = compute_fitness_change(graph, colony[index_individual], v1, colony[index_individual][v1]) +
                        compute_fitness_change(graph, colony[index_individual], v2, colony[index_individual][v2]);

    arrayFitness[index_individual] += delta_fitness;

    // Se piorou, reverte a troca
    if (delta_fitness > 0)
    {
        std::swap(colony[index_individual][v1], colony[index_individual][v2]);
        arrayFitness[index_individual] -= delta_fitness;
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
        employed_bee_phase(true);
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

        if (best_fit == 0)
            break;
    }

    return best_bee;
}

Individual ABCGraphColoring::run(char method, int rcl_size)
{
    if (method == 'p') // pseudo_greedy
    {
        initialize_colony_pseudogreedy();
    }
    else if (method == 'g') // grasp buildphase
    {
        initialize_colony_grasp_buildphase(rcl_size);
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
        employed_bee_phase(true); // true é random_search, false é swap_two_colors
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

        if (best_fit == 0)
            break;
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

#pragma endregion

/////////////////////////////////////////////////////
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

            new_indv[v] = rcl[randint(0, rcl.size() - 1)];
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

    int first_index{randint(0, graph.getNumVertices() - 1)};
    int second_index{randint_diff(0, graph.getNumVertices() - 1, first_index)};

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
    // return
    // Iniciar solução

    int num_iter{0};
    int num_gen_sol{(graph.getNumVertices() > 100) ? (int)(graph.getNumVertices() * 0.1) : 5};

    std::vector<Individual> arr_solutions;
    std::vector<Fitness> arr_fit;

    Individual indv = initialize_individual(num_colors, graph);
    Fitness fit{};
    evaluate_fitness(graph, indv, fit);

    Individual best_indv{};
    Fitness best_fit{graph.getNumEdges()}; // Inicializando com número de arestas do grafo

    for (int i{0}; i < num_gen_sol; ++i)
    {
        arr_solutions.push_back(Individual(graph.getNumVertices()));
        arr_fit.push_back(graph.getNumEdges());
    }

    while (best_fit > 0 && num_iter < max_iter)
    {
        int lower{0};
        int index{0};
        int undo_color{0};

        for (int i{0}; i < num_gen_sol; ++i)
        {
            int vertex{randint(0, graph.getNumVertices() - 1)};
            copy_individual(indv, fit, arr_solutions[i], arr_fit[i]);
            int new_color = randint_diff(0, num_colors, arr_solutions[i][vertex]);
            arr_solutions[i][vertex] = new_color;
            evaluate_fitness(graph, arr_solutions[i], arr_fit[i]);

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
        }
        else
        {
            num_iter++;
        }
    }
    return best_indv;
}

void TabuColoring::print_tabu_list()
{
    tabu_list.print();
}
#pragma endregion