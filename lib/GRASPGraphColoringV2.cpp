#include "GRASPGraphColoringV2.h"
#include "utils.h"
#include "heuristics.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

#include <set>
#include <queue>

#pragma region GRASP

Individual GRASPGraphColoringV2::BuildPhase()
{
    Individual indv(graph.getNumVertices(), -1);
    std::set<int> uncolored_vertices;

    for (int i{0}; i < graph.getNumVertices(); ++i)
        uncolored_vertices.insert(i);

    std::vector<int> cost(graph.getNumVertices(), 0);
    while (!uncolored_vertices.empty())
    {
        for (auto uncolor_v : uncolored_vertices)
        {
            std::set<int> used_color;
            for (auto neighbor : graph.getNeighbors(uncolor_v))
            {
                if (indv[neighbor] != -1)
                    used_color.insert(indv[neighbor]);
            }

            cost[uncolor_v] = used_color.size();
        }

        int cost_min{INT32_MAX};
        int cost_max{INT32_MIN};
        for (auto v : uncolored_vertices)
        {
            cost_min = std::min(cost_min, cost[v]);
            cost_max = std::max(cost_max, cost[v]);
        }

        double limit_rcl = cost_min + alpha * (cost_max - cost_min);

        std::vector<int> RCL;
        for (auto v : uncolored_vertices)
        {
            if (static_cast<double>(cost[v]) <= limit_rcl)
            {
                RCL.push_back(v);
            }
        }

        int v_chosen{};

        if (RCL.empty())
        {
            v_chosen = *uncolored_vertices.begin();
        }
        else
        {
            v_chosen = RCL[randint(0, RCL.size() - 1, rng)];
        }

        std::vector<bool> forbidden_colors(num_colors, false);
        for (auto neighbor : graph.getNeighbors(v_chosen))
        {
            if (indv[neighbor] != -1)
            {
                forbidden_colors[indv[neighbor]] = true;
            }
        }

        int chosen_color{-1};
        for (int color{0}; color < num_colors; ++color)
        {
            if (!forbidden_colors[color])
            {
                chosen_color = color;
                break;
            }
        }
        if (chosen_color != -1)
        {
            indv[v_chosen] = chosen_color;
        }
        else
        {
            indv[v_chosen] = find_color_least_conflicts(indv, v_chosen);
        }
        uncolored_vertices.erase(v_chosen);
    }

    return indv;
}

// Função auxiliar que tenta aplicar uma troca de Kempe entre duas cores c1 e c2 a partir do vértice v
// A troca só é mantida se reduzir o número de conflitos
bool GRASPGraphColoringV2::TryKempeSwap(Individual &indv, int vertex, int color1, int color2, Fitness &current_conflicts)
{
    if (indv[vertex] != color1)
        return false;

    std::vector<bool> visited(graph.getNumVertices(), false);
    std::vector<int> kempe_chain;
    std::queue<int> q;

    visited[vertex] = true;
    q.push(vertex);

    // BFS para construir a cadeia de Kempe com as cores color1 e color2
    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        kempe_chain.push_back(u);

        for (auto neighbor : graph.getNeighbors(u))
        {
            if (!visited[neighbor] && (indv[neighbor] == color1 || indv[neighbor] == color2))
            {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }

    for (int u : kempe_chain)
    {
        if (indv[u] == color1)
            indv[u] = color2;
        else if (indv[u] == color2)
            indv[u] = color1;
    }

    Fitness new_fit{};
    evaluate_fitness(graph, indv, new_fit);

    if (new_fit < current_conflicts)
    {
        // Aceita a troca
        current_conflicts = new_fit;
        return true;
    }
    else
    {
        // Reverte a troca
        for (int u : kempe_chain)
        {
            if (indv[u] == color1)
                indv[u] = color2;
            else if (indv[u] == color2)
                indv[u] = color1;
        }
        return false;
    }
}

// Busca local baseada em Kempe Chain: tenta reduzir o número de conflitos após a construção inicial
void GRASPGraphColoringV2::LocalSearchKempeChain(Individual &indv)
{
    Fitness current_conflicts{};
    evaluate_fitness(graph, indv, current_conflicts);
    bool improved{true};

    // Continua enquanto houver melhoria e ainda existirem conflitos
    while (improved && current_conflicts > 0)
    {
        improved = false;

        for (int v{0}; v < graph.getNumVertices(); ++v)
        {
            for (int color2{0}; color2 < num_colors; ++color2)
            {
                if (color2 == indv[v])
                    continue; // Pula a cor atual

                // Tenta aplicar Kempe swap entre a cor atual de v e color 2
                if (TryKempeSwap(indv, v, indv[v], color2, current_conflicts))
                {
                    improved = true;
                    break;
                }
            }
            if (improved)
                break;
        }
    }
}

bool GRASPGraphColoringV2::OneMove(Individual &indv, Fitness &fit)
{
    for (int v{0}; v < graph.getNumVertices(); ++v)
    {
        int original_color = indv[v];
        for (int c{0}; c < num_colors; ++c)
        {
            if (c == original_color)
                continue;
            // indv[v] = c;
            // Fitness new_fit{};
            // evaluate_fitness(graph, indv, new_fit);
            Fitness delta = compute_fitness_change(graph, indv, v, c);
            if (delta < 0)
            {
                fit = delta + fit;
                indv[v] = c;
                return true;
            }
        }
        // indv[v] = original_color;
    }
    return false;
}

bool GRASPGraphColoringV2::SwapVertices(Individual &indv, Fitness &fit)
{
    for (int u{0}; u < graph.getNumVertices(); ++u)
    {
        for (int v{u + 1}; v < graph.getNumVertices(); ++v)
        {
            if (indv[u] == indv[v])
                continue;
            // std::swap(indv[u], indv[v]);
            // Fitness new_fit{};
            Fitness delta = compute_swap_fitness_change(graph, indv, u, v);
            // evaluate_fitness(graph, indv, new_fit);
            //if (new_fit < fit)
            if (delta < 0)
            {
                // fit = new_fit;
                fit += delta;
                std::swap(indv[u], indv[v]);
                return true;
            }
            // std::swap(indv[u], indv[v]);
        }
    }
    return false;
}

void GRASPGraphColoringV2::LocalSearch(Individual &indv, Fitness &fit, int &acceptanceratio)
{
}

void GRASPGraphColoringV2::UpdateSolution(const Individual &indv, const Fitness &fit, int &acceptanceratio)
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

Individual GRASPGraphColoringV2::run()
{
    Individual indv = GRASPGraphColoringV2::BuildPhase();
    Fitness fit;
    int acceptanceratio{1000};

    evaluate_fitness(graph, indv, fit);
    copy_individual(indv, fit, best_indv, best_fit);

    while (acceptanceratio < 100 && best_fit > 0)
    {
        GRASPGraphColoringV2::LocalSearch(indv, fit, acceptanceratio);
        GRASPGraphColoringV2::UpdateSolution(indv, fit, acceptanceratio);
    }

    return best_indv;
}

// Método principal que executa o GRASP
Individual GRASPGraphColoringV2::run_kempe()
{
    Individual best_indv;
    int best_conflicts = std::numeric_limits<int>::max();

    for (int i = 0; i < max_iter; ++i)
    {
        // Fase de construção
        Individual candidate = BuildPhase();

        // Fase de busca local
        LocalSearchKempeChain(candidate);

        // Avalia e atualiza o melhor indivíduo
        Fitness current_conflicts{};
        evaluate_fitness(graph, candidate, current_conflicts);
        if (current_conflicts < best_conflicts)
        {
            best_conflicts = current_conflicts;
            best_indv = candidate;
            iter_found_best = i;

            // Parada antecipada se solução viável for encontrada
            if (best_conflicts == 0)
                break;
        }
    }

    return best_indv;
}

Individual GRASPGraphColoringV2::run_swap()
{
    Individual best_indv;
    Fitness best_fit = std::numeric_limits<Fitness>::max();

    for (int i {0}; i < max_iter; ++i)
    {
        // Fase de construção
        Individual candidate = BuildPhase();
        Fitness fit{};

        evaluate_fitness(graph, candidate, fit);

        // std::cout << "-----------------------" << std::endl;
        // std::cout << "Iter: " << i << std::endl;
        // std::cout << "Initial: " << std::endl;
        // print_individual(candidate, fit);

        bool improved{true};
        while (improved && fit > 0){
            improved = SwapVertices(candidate, fit);
        }
        // std::cout << "-----------------------" << std::endl;
        // std::cout << "Swap: " << std::endl;
        // print_individual(candidate, fit);

        if (fit < best_fit){
            best_fit = fit;
            best_indv = candidate;
            iter_found_best = i;

            if (best_fit == 0)
                break;
        }
    }
    // std::cout << "-----------------------" << std::endl;
    // std::cout << "Best: " << std::endl;
    // print_individual(best_indv, best_fit);

    return best_indv;
}

Individual GRASPGraphColoringV2::run_one_move()
{
    Individual best_indv;
    Fitness best_fit = std::numeric_limits<Fitness>::max();

    for (int i{0}; i < max_iter; ++i)
    {
        // Fase de construção
        Individual candidate = BuildPhase();
        Fitness fit{};
        evaluate_fitness(graph, candidate, fit);

        // std::cout << "-----------------------" << std::endl;
        // std::cout << "Iter: " << i << std::endl;
        // std::cout << "Initial: " << std::endl;
        // print_individual(candidate, fit);

        bool improved{true};
        while (improved && fit > 0)
        {
            improved = OneMove(candidate, fit);
        }
        // std::cout << "-----------------------" << std::endl;
        // std::cout << "OneMove: " << std::endl;
        // print_individual(candidate, fit);
        if (fit < best_fit)
        {
            best_indv = candidate;
            best_fit = fit;
            iter_found_best = i;

            if (best_fit == 0)
                break;
        }
    }

    // std::cout << "-----------------------" << std::endl;
    // std::cout << "Best: " << std::endl;
    // print_individual(best_indv, best_fit);
    return best_indv;
}
#pragma endregion
