#include "ABCGraphColoringV2.h"
#include "TabuGraphColoring.h"
#include "utils.h"
#include "heuristics.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

#pragma region ABCGraphColoringV2

void ABCGraphColoringV2::initialize_employedBees_random()
{
    for (int i{0}; i < num_employed; ++i)
    {
        employedBees[i] = initialize_individual(num_colors, graph, rng);
        evaluate_fitness(graph, employedBees[i], employedsFitness[i]);
    }
}

bool ABCGraphColoringV2::is_similar_to_existing(const Individual &indv, int num_existing)
{
    for (int j{0}; j < num_existing; j++)
    {
        int diff{0};
        for (size_t i = 0; i < indv.size(); ++i)
        {
            if (indv[i] != employedBees.at(j)[i])
                diff++;
        }
        double similarity = 1.0 - (diff / (double)indv.size());
        if (similarity > threshold)
            return true; // Too similar
    }
    return false;
}

void ABCGraphColoringV2::initialize_employedBees_pseudoGreedy_v3()
{
    GreedyGraphColoring greedy = GreedyGraphColoring(graph, num_colors, rng);
    employedBees[0] = greedy.run_pseudo_greedy_v3();
    evaluate_fitness(graph, employedBees[0], employedsFitness[0]);
    for (int i{1}; i < num_employed; i++)
    {
        do
        {
            employedBees[i] = greedy.run_pseudo_greedy_v3();
            evaluate_fitness(graph, employedBees[i], employedsFitness[i]);
        } while (is_similar_to_existing(employedBees[i], i - 1));
    }
}

void ABCGraphColoringV2::initialize_employedBees_pseudoGreedy_v2()
{
    GreedyGraphColoring greedy = GreedyGraphColoring(graph, num_colors, rng);
    for (int i{0}; i < num_employed; ++i)
    {
        employedBees[i] = greedy.run_pseudo_greedy_v2();
        evaluate_fitness(graph, employedBees[i], employedsFitness[i]);
    }
}

void ABCGraphColoringV2::initialize_employedBees_pseudoGreedy()
{
    GreedyGraphColoring greedy = GreedyGraphColoring(graph, num_colors, rng);
    for (int i{0}; i < num_employed; ++i)
    {
        employedBees[i] = greedy.run_pseudo_greedy_v2();
        evaluate_fitness(graph, employedBees[i], employedsFitness[i]);
    }
}

void ABCGraphColoringV2::update_fitness_for_minimization()
{
    for (int i{0}; i < num_employed; ++i)
    {
        fitnessForMinimization[i] = 1.0 / (1 + employedsFitness[i]);
    }
}

int ABCGraphColoringV2::roulette_wheel_choose_bee()
{
    update_fitness_for_minimization();

    double total_fitness = std::accumulate(fitnessForMinimization.begin(), fitnessForMinimization.end(), 0.0);

    double r{randdouble(0, total_fitness, rng)};

    double cumulativeSum = 0.0;
    for (int i{0}; i < num_employed; ++i)
    {
        cumulativeSum += fitnessForMinimization.at(i);
        if (cumulativeSum >= r)
        {
            return i;
        }
    }
    // Retorna a última abelha (garantia de sempre retornar algo)
    return num_employed - 1;
}

void ABCGraphColoringV2::random_choice_local_search(int index_individual, Individual &indv, Fitness &fit)
{
    int rand_vertex = find_most_conflicted_vertex(indv, graph);

    if (rand_vertex == -1)
        return; // Nenhum conflito encontrado

    int old_color = indv[rand_vertex];
    int new_color = randint_diff(0, num_colors - 1, old_color, rng);

    int delta_fitness = compute_fitness_change(graph, indv, rand_vertex, new_color);

    // Aplica a mudança
    indv[rand_vertex] = new_color;
    fit += delta_fitness;

    // Se a mudança piorou, reverte
    if (delta_fitness > 0)
    {
        indv[rand_vertex] = old_color;
        fit -= delta_fitness;
    }
}

void ABCGraphColoringV2::enhanced_random_search(int index_individual, Individual &indv, Fitness &current_fitness)
{
    int most_conflicted = find_most_conflicted_vertex(indv, graph);
    int second_conflicted = find_second_most_conflicted_vertex(indv, graph, most_conflicted);

    int original_color_most = indv[most_conflicted];
    int original_color_second = indv[second_conflicted];

    // Try swapping colors between two conflicted vertices
    indv[most_conflicted] = original_color_second;
    indv[second_conflicted] = original_color_most;

    // std::cout << "most_conflicted: " << most_conflicted << std::endl;
    // std::cout << "second_conflicted:" << second_conflicted << std::endl;
    int new_fitness{};
    evaluate_fitness(graph, indv, new_fitness);

    double prob{randdouble(0, 1, rng)};
    // std::cout << "prob: " << prob << "<" << "acceptance: " << acceptance_probability << std::endl;
    // std::cout << "new_fit: " << new_fitness << "- curr_fit: "<< current_fitness << std::endl;
    if ((new_fitness < current_fitness) || (prob < acceptance_probability))
    {
        // std::cout << "new_fit: " << new_fitness << "- curr_fit: "<< current_fitness << std::endl;
        current_fitness = new_fitness;
        // std::cout << "entrei" << std::endl;
    }
    else
    {
        // Undo the swap if no improvement and not accepted
        indv[most_conflicted] = original_color_most;
        indv[second_conflicted] = original_color_second;

        // std::cout << "não entrei" << std::endl;
    }
}

void ABCGraphColoringV2::swap_conflicted_vertices(Individual &indv, Fitness &fit)
{
    int v1 = find_most_conflicted_vertex(indv, graph);
    if (v1 == -1)
        return; // Nenhum conflito encontrado

    // Escolhe um vizinho com conflito
    int v2 = -1;
    for (int neighbor : graph.getNeighbors(v1))
    {
        if (indv[neighbor] == indv[v1])
        {
            v2 = neighbor;
            break;
        }
    }
    if (v2 == -1)
        return; // Nenhum vizinho conflitante encontrado

    // Atualiza a fitness incrementalmente
    int delta_fitness = compute_fitness_change(graph, indv, v1, indv[v1]) +
                        compute_fitness_change(graph, indv, v2, indv[v2]);

    // Troca as cores
    std::swap(indv[v1], indv[v2]);
    fit += delta_fitness;

    // Se piorou, reverte a troca
    if (delta_fitness > 0)
    {
        std::swap(indv[v1], indv[v2]);
        fit -= delta_fitness;
    }
}

void ABCGraphColoringV2::waggle_dance(int idx_bee, Individual &bee, Fitness &fit)
{
    int numVertices{graph.getNumVertices()};
    int numChanges{randint(1, numVertices / 2, rng)};
    int neighbor = randint_diff(0, num_employed - 1, idx_bee, rng);

    Individual newIndv{};
    Fitness newFit{};
    copy_individual(bee, fit, newIndv, newFit);

    for (int i{0}; i < numChanges; ++i)
    {
        int v{randint(0, numVertices - 1, rng)};
        newIndv[v] = employedBees.at(neighbor)[v];
    }
    evaluate_fitness(graph, newIndv, newFit);

    if (newFit < fit)
    {
        copy_individual(newIndv, newFit, bee, fit);
    }
}

void ABCGraphColoringV2::waggle_dance_annealing(int idx_bee, Individual &bee, Fitness &fit)
{
    int numVertices{graph.getNumVertices()};
    int neighbor = randint_diff(0, num_employed - 1, idx_bee, rng);
    int v{randint(0, numVertices - 1, rng)};

    int oldColor{bee[v]};
    int new_color{employedBees.at(neighbor)[v]};
    int delta{compute_fitness_change(graph, bee, v, new_color)};

    double acceptanceProbability = exp(-delta / temperature);

    if ((delta < 0) || ((double)rand() / RAND_MAX) < acceptanceProbability)
    {
        fit += delta;
        bee[v] = new_color;
    }
    else
    {
        bee[v] = oldColor;
    }
}

void ABCGraphColoringV2::employed_bee_phase()
{
    for (int i{0}; i < num_employed; ++i)
    {
        if (limit_no_improve[i] < limit)
        {
            Fitness old_fit{employedsFitness[i]};
            // waggle_dance(i, employedBees[i], employedsFitness[i]);
            // if (randdouble(0.0, 1.0) < 0.5)
            // {
            //     random_choice_local_search(i, employedBees[i], employedsFitness[i]);
            // }
            // else
            // {
            //     swap_conflicted_vertices(employedBees[i], employedsFitness[i]);
            // }
            random_choice_local_search(i, employedBees[i], employedsFitness[i]);
            if (employedsFitness[i] < old_fit)
            {
                limit_no_improve[i] = 0;
            }
            else
            {
                limit_no_improve[i]++;
            }
        }
    }
}

void ABCGraphColoringV2::employed_bee_phase_enhanced()
{
    for (int i{0}; i < num_employed; ++i)
    {
        // print_limit_no_improve();
        if (limit_no_improve[i] < limit)
        {
            Fitness old_fit{employedsFitness[i]};
            // waggle_dance(i, employedBees[i], employedsFitness[i]);
            // if (randdouble(0.0, 1.0) < 0.5)
            // {
            //     random_choice_local_search(i, employedBees[i], employedsFitness[i]);
            // }
            // else
            // {
            //     swap_conflicted_vertices(employedBees[i], employedsFitness[i]);
            // }
            enhanced_random_search(i, employedBees[i], employedsFitness[i]);
            if (employedsFitness[i] < old_fit)
            {
                limit_no_improve[i] = 0;
            }
            else
            {
                limit_no_improve[i]++;
            }
        }
    }
}

void ABCGraphColoringV2::employed_bee_phase_tabu(int T_iter, int max_iter_tabu)
{
    for (int i{0}; i < num_employed; ++i)
    {
        Fitness old_fit{employedsFitness[i]};
        if (limit_no_improve[i] < limit)
        {
            // waggle_dance(i, employedBees[i], employedsFitness[i]);
            // if (randdouble(0.0, 1.0) < 0.5)
            // {
            //     random_choice_local_search(i, employedBees[i], employedsFitness[i]);
            // }
            // else
            // {
            //     swap_conflicted_vertices(employedBees[i], employedsFitness[i]);
            // }
            random_choice_local_search(i, employedBees[i], employedsFitness[i]);
        }
        else if (limit_no_improve[i] % 5 == 0)
        {
            TabuColoring tabu(graph, num_colors, max_iter, T_iter, rng);

            tabu.run(employedBees[i], employedsFitness[i]);
        }
        if (employedsFitness[i] < old_fit)
        {
            limit_no_improve[i] = 0;
        }
        else
        {
            limit_no_improve[i]++;
        }
    }
}

void ABCGraphColoringV2::employed_bee_phase_annealing()
{
    for (int i{0}; i < num_employed; ++i)
    {
        Fitness old_fit{employedsFitness[i]};
        if (limit_no_improve[i] < limit)
        {
            waggle_dance_annealing(i, employedBees[i], employedsFitness[i]);
            if (employedsFitness[i] < old_fit)
            {
                limit_no_improve[i] = 0;
            }
            else
            {
                limit_no_improve[i]++;
            }
        }
    }
}

void ABCGraphColoringV2::onlooker_bee_phase_tabu(int T_iter, int max_iter_tabu)
{
    for (int i{0}; i < num_onlooker; ++i)
    {
        int sel_emp{roulette_wheel_choose_bee()};
        copy_individual(employedBees[sel_emp], employedsFitness[i], onlookerBees[i], onlookersFitness[i]);
        waggle_dance(sel_emp, onlookerBees[i], onlookersFitness[i]);
        if (onlookersFitness[i] < employedsFitness[sel_emp])
        {
            copy_individual(onlookerBees[i], onlookersFitness[i], employedBees[sel_emp], employedsFitness[sel_emp]);
            limit_no_improve[i] = 0;
        }
        else
        {
            limit_no_improve[i]++;
            if (limit_no_improve[i] % 5 == 0)
            {
                TabuColoring tabu(graph, num_colors, max_iter_tabu, T_iter, rng);
                tabu.run(onlookerBees[i], onlookersFitness[i]);
                if (onlookersFitness[i] < employedsFitness[sel_emp])
                {
                    copy_individual(onlookerBees[i], onlookersFitness[i], employedBees[sel_emp], employedsFitness[sel_emp]);
                    limit_no_improve[i] = 0;
                }
            }
        }
    }
}

void ABCGraphColoringV2::onlooker_bee_phase()
{
    for (int i{0}; i < num_onlooker; ++i)
    {
        int sel_emp{roulette_wheel_choose_bee()};
        copy_individual(employedBees[sel_emp], employedsFitness[sel_emp], onlookerBees[i], onlookersFitness[i]);
        waggle_dance(sel_emp, onlookerBees[i], onlookersFitness[i]);
        // random_choice_local_search(sel_emp, onlookerBees[i], onlookersFitness[i]);
        if (onlookersFitness[i] < employedsFitness[sel_emp])
        {
            copy_individual(onlookerBees[i], onlookersFitness[i], employedBees[sel_emp], employedsFitness[sel_emp]);
            limit_no_improve[i] = 0;
        }
        else
        {
            limit_no_improve[i]++;
        }
    }
}

void ABCGraphColoringV2::onlooker_bee_phase_enhanced()
{
    for (int i{0}; i < num_onlooker; ++i)
    {
        int sel_emp{roulette_wheel_choose_bee()};
        copy_individual(employedBees[sel_emp], employedsFitness[sel_emp], onlookerBees[i], onlookersFitness[i]);
        // waggle_dance(sel_emp, onlookerBees[i], onlookersFitness[i]);
        enhanced_random_search(sel_emp, onlookerBees[i], onlookersFitness[i]);
        if (onlookersFitness[i] < employedsFitness[sel_emp])
        {
            copy_individual(onlookerBees[i], onlookersFitness[i], employedBees[sel_emp], employedsFitness[sel_emp]);
            limit_no_improve[sel_emp] = 0;
        }
        else
        {
            limit_no_improve[sel_emp]++;
        }
    }
}

void ABCGraphColoringV2::onlooker_bee_phase_annealing()
{
    for (int i{0}; i < num_onlooker; ++i)
    {
        int sel_emp{roulette_wheel_choose_bee()};
        copy_individual(employedBees[sel_emp], employedsFitness[i], onlookerBees[i], onlookersFitness[i]);
        waggle_dance_annealing(sel_emp, onlookerBees[i], onlookersFitness[i]);
        if (onlookersFitness[i] < employedsFitness[i])
        {
            copy_individual(onlookerBees[i], onlookersFitness[i], employedBees[sel_emp], employedsFitness[i]);
            limit_no_improve[i] = 0;
        }
        else
        {
            limit_no_improve[i]++;
        }
    }
}

void ABCGraphColoringV2::scout_bee_phase()
{
    for (int i{0}; i < num_employed; ++i)
    {
        if (limit_no_improve[i] > limit)
        {
            random_individual(num_colors, graph, employedBees[i], rng);
            evaluate_fitness(graph, employedBees[i], employedsFitness[i]);
            limit_no_improve[i] = 0;
        }
    }
}

void ABCGraphColoringV2::scout_bee_phase_enhanced(Individual &best_bee, Fitness &best_fit)
{
    for (int i = 0; i < employedBees.size(); ++i)
    {
        if (limit_no_improve[i] > limit)
        {
            copy_individual(best_bee, best_fit, employedBees[i], employedsFitness[i]);
            perturb_best_solution(employedBees[i]);
            evaluate_fitness(graph, employedBees[i], employedsFitness[i]);
            limit_no_improve[i] = 0;
        }
    }
}

int ABCGraphColoringV2::find_best_bee()
{
    int idx{0};
    for (int i{1}; i < num_employed; i++)
        if (employedsFitness[i] < employedsFitness[idx])
            idx = i;

    return idx;
}

Individual ABCGraphColoringV2::return_bee(int idx_bee)
{
    return employedBees.at(idx_bee);
}

Individual ABCGraphColoringV2::run(char method_init)
{
    if (method_init == 'r')
    {
        initialize_employedBees_random();
    }
    else if (method_init == 'p')
    {
        // std::cout << "aqui" << std::endl;
        // initialize_employedBees_pseudoGreedy();
        initialize_employedBees_pseudoGreedy_v2();
    }

    Individual best_bee{};
    Fitness best_fit{};

    int idx_best = find_best_bee();
    copy_individual(employedBees[idx_best], employedsFitness[idx_best], best_bee, best_fit);

    int num_iter_no_improv = 0;
    num_iters = 0;

    while (num_iter_no_improv < max_iter)
    {
        employed_bee_phase();
        onlooker_bee_phase();
        scout_bee_phase();

        int current_best_bee = find_best_bee();
        if (best_fit > employedsFitness[current_best_bee])
        {
            copy_individual(employedBees[current_best_bee], employedsFitness[current_best_bee], best_bee, best_fit);
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

Individual ABCGraphColoringV2::run_enhanced(char method_init)
{
    if (method_init == 'r')
    {
        initialize_employedBees_random();
    }
    else if (method_init == 'p')
    {
        initialize_employedBees_pseudoGreedy_v3();
    }

    Individual best_bee{};
    Fitness best_fit{};

    int idx_best = find_best_bee();
    copy_individual(employedBees[idx_best], employedsFitness[idx_best], best_bee, best_fit);

    int num_iter_no_improv = 0;
    num_iters = 0;

    while ((num_iter_no_improv < max_iter) && (best_fit > 0))
    {
        // std::cout << num_iters << std::endl;
        employed_bee_phase_enhanced();
        onlooker_bee_phase_enhanced();
        scout_bee_phase_enhanced(best_bee, best_fit);

        int current_best_bee = find_best_bee();
        if (best_fit > employedsFitness[current_best_bee])
        {
            copy_individual(employedBees[current_best_bee], employedsFitness[current_best_bee], best_bee, best_fit);
            num_iter_no_improv = 0;
            iter_found_best = num_iters;
        }
        else
        {
            num_iter_no_improv++;
        }
        num_iters++;

        // Reduce acceptance probability over iterations (Simulated Annealing style)
        acceptance_probability *= 0.99;
        if (acceptance_probability < 0.01)
        {
            acceptance_probability = 0.01; // Keep a minimum threshold for occasional jumps
        }
        // print_individual(best_bee, best_fit);
        if (best_fit == 0)
            break;
    }

    return best_bee;
}

Individual ABCGraphColoringV2::run_annealing(char method_init)
{
    if (method_init == 'r')
    {
        initialize_employedBees_random();
    }
    else if (method_init == 'p')
    {
        initialize_employedBees_pseudoGreedy_v2();
    }

    Individual best_bee{};
    Fitness best_fit{};
    // for (int i{0}; i < num_employed; ++i){
    //     print_employedBee(i);
    // }

    int idx_best = find_best_bee();
    copy_individual(employedBees[idx_best], employedsFitness[idx_best], best_bee, best_fit);

    int num_iter_no_improv = 0;
    num_iters = 0;

    while (num_iter_no_improv < max_iter)
    {
        employed_bee_phase_annealing();
        onlooker_bee_phase_annealing();
        scout_bee_phase();

        int current_best_bee = find_best_bee();
        if (best_fit > employedsFitness[current_best_bee])
        {
            copy_individual(employedBees[current_best_bee], employedsFitness[current_best_bee], best_bee, best_fit);
            num_iter_no_improv = 0;
            iter_found_best = num_iters;
        }
        else
        {
            num_iter_no_improv++;
        }
        num_iters++;

        temperature *= cooling_rate;

        if (best_fit == 0)
            break;
    }

    return best_bee;
}

Individual ABCGraphColoringV2::run_tabucol(char method_init, int T_iter, int max_iter_tabu)
{

    if (method_init == 'r')
    {
        initialize_employedBees_random();
    }
    else if (method_init == 'p')
    {
        initialize_employedBees_pseudoGreedy_v2();
    }

    Individual best_bee{};
    Fitness best_fit{};

    int idx_best = find_best_bee();
    copy_individual(employedBees[idx_best], employedsFitness[idx_best], best_bee, best_fit);

    int num_iter_no_improv = 0;
    num_iters = 0;

    while (num_iter_no_improv < max_iter)
    {
        employed_bee_phase();
        onlooker_bee_phase_tabu(T_iter, max_iter_tabu);
        scout_bee_phase();

        int current_best_bee = find_best_bee();
        if (best_fit > employedsFitness[current_best_bee])
        {
            copy_individual(employedBees[current_best_bee], employedsFitness[current_best_bee], best_bee, best_fit);
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

void ABCGraphColoringV2::print_colony() const
{
    std::cout << "EmployedBees: " << std::endl;
    for (int i{0}; i < num_employed; ++i)
    {
        std::cout << i << ": ";
        print_individual(employedBees.at(i), employedsFitness.at(i));
        std::cout << "--\n";
    }
    std::cout << "OnlookerBees: " << std::endl;
    for (int i{0}; i < num_onlooker; ++i)
    {
        std::cout << i << ": ";
        print_individual(onlookerBees.at(i), onlookersFitness.at(i));
        std::cout << "--\n";
    }
}

void ABCGraphColoringV2::print_fitnessForMinimization() const
{
    for (double prob : fitnessForMinimization)
    {
        std::cout << prob << " ";
    }
    std::cout << "sum: " << std::accumulate(fitnessForMinimization.begin(), fitnessForMinimization.end(), 0.0) << std::endl;
}

void ABCGraphColoringV2::print_limit_no_improve() const
{
    std::cout << "limits_array:\n";
    for (int limits : limit_no_improve)
    {
        std::cout << limits << " ";
    }
    std::cout << std::endl;
}

void ABCGraphColoringV2::print_employedBee(int idx) const
{
    print_individual(employedBees.at(idx), employedsFitness.at(idx));
}
void ABCGraphColoringV2::print_onlookerBee(int idx) const
{
    print_individual(onlookerBees.at(idx), onlookersFitness.at(idx));
}

#pragma endregion
