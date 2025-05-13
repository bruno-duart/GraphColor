#include "ABCGraphColoring.h"
#include "GRASPGraphColoring.h"
#include "TabuGraphColoring.h"
#include "utils.h"
#include "heuristics.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

#pragma region ABCGraphColoring

void ABCGraphColoring::initialize_colony()
{
    for (int i{0}; i < num_bees; ++i)
    {
        colony[i] = initialize_individual(num_colors, graph, rng);
        evaluate_fitness(graph, colony[i], arrayFitness[i]);
    }
}

void ABCGraphColoring::initialize_colony_pseudogreedy()
{
    GreedyGraphColoring greedy = GreedyGraphColoring(graph, num_colors, rng);
    for (int i{0}; i < num_bees; ++i)
    {
        colony[i] = greedy.run_pseudo_greedy();
        evaluate_fitness(graph, colony[i], arrayFitness[i]);
    }
}

void ABCGraphColoring::initialize_colony_grasp_buildphase(int max_rcl_size)
{
    GRASPGraphColoring grasp = GRASPGraphColoring(graph, num_colors, rng, max_rcl_size);
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

void ABCGraphColoring::employed_bee_phase_tabu(int T_iter, int max_iter)
{
    TabuColoring tabu(graph, num_colors, max_iter, T_iter, rng);
    for (int i = 0; i < num_bees / 2; ++i)
    {
        Fitness old_fit = arrayFitness[i];
        if (random)
            random_choice_local_search(i);
        else
            swap_conflicted_vertices(i);
        
        if ((limit_no_improve[i]%25 == 0) && (limit_no_improve[i]> 0)) {
            tabu.run(colony[i], arrayFitness[i]);
        }
        if (old_fit <= arrayFitness[i])
        {
            limit_no_improve[i]++;
        }
    }
}

void ABCGraphColoring::onlooker_bee_phase()
{
    calc_probabilities(); // Atualiza probabilidades com base na fitness

    int emp_idx{0};
    int i{num_bees / 2};

    while (i < num_bees)
    {
        double r = static_cast<double>(rand()) / RAND_MAX;

        // Critério híbrido: 80% roleta, 20% escolha aleatória
        if (r < 0.75)
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
            int rand_emp_idx = randint(0, num_bees / 2 - 1, rng);
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
                random_individual(num_colors, graph, colony.at(i), rng);
                evaluate_fitness(graph, colony.at(i), arrayFitness[i]);
            }

            limit_no_improve[i] = 0; // Reset contador da abelha
        }
    }
}

void ABCGraphColoring::small_mutation(int index)
{
    int rand_vertex = randint(0, graph.getNumVertices() - 1, rng);
    colony.at(index)[rand_vertex] = randint(1, num_colors, rng); // Escolhe cor aleatória
    evaluate_fitness(graph, colony.at(index), arrayFitness[index]);
}

void ABCGraphColoring::waggle_dance(int idx_bee, int idx_other_bee)
{
    // int idx_other_bee = randint(0, (num_bees / 2) - 1);
    int rand_vertex = randint(0, graph.getNumVertices() - 1, rng);

    int old_color = colony.at(idx_bee)[rand_vertex];
    int old_fit = arrayFitness.at(idx_bee);
    int new_color = colony.at(idx_other_bee)[rand_vertex];

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

void ABCGraphColoring::random_choice_local_search(int index_individual)
{
    int rand_vertex = find_most_conflicted_vertex(colony[index_individual], graph);

    if (rand_vertex == -1)
        return; // Nenhum conflito encontrado

    int old_color = colony.at(index_individual)[rand_vertex];
    int new_color = randint_diff(1, num_colors, old_color, rng);

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

Individual ABCGraphColoring::run_tabucol(int T_iter, int max_iter_tabu){
    
    initialize_colony_pseudogreedy();
    
    Individual best_bee{};
    Fitness best_fit{};

    int idx_best = find_best_bee();
    copy_individual(colony[idx_best], arrayFitness[idx_best], best_bee, best_fit);

    int num_iter_no_improv = 0;
    num_iters = 0;

    while (num_iter_no_improv < max_iter)
    {
        employed_bee_phase_tabu(T_iter, max_iter_tabu); 
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

