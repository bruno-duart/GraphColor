#include "heuristics.h"
#include "graphs.h"
#include "solution.h"
#include "utils.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <set>
#include <numeric>
#include <random>

Individual GreedyGraphColoring::run_pseudo_greedy()
{
    indv.assign(graph.getNumVertices(), -1);
    Fitness fit{0};

    // Seleção dos vértices a serem pré-coloridos aleatoriamente
    std::set<int> pre_colored_vertices;
    while ((int)pre_colored_vertices.size() < 3)
    {
        int rand_vert{randint(0, graph.getNumVertices() - 1, rng)};
        pre_colored_vertices.insert(rand_vert);
    }
    // Atribuição aleatória de cores
    for (int v : pre_colored_vertices)
    {
        indv[v] = randint(0, num_color - 1, rng);
    }

    // Atribuir cores aos vértices com estratégia gulosa
    for (int v{0}; v < graph.getNumVertices(); ++v)
    {
        if (indv[v] != -1)
            continue;

        std::vector<bool> forbidden_colors(num_color, false);
        for (auto neighbor : graph.adjList[v])
        {
            if (indv[neighbor] != -1)
            {
                forbidden_colors[indv[neighbor]] = true;
            }
        }

        int chosen_color = -1;
        for (int color = 0; color < num_color; ++color)
        {
            if (!forbidden_colors[color])
            {
                chosen_color = color;
                break;
            }
        }

        if (chosen_color == -1)
        {
            chosen_color = find_color_least_conflicts(v);
            for (auto neighbor : graph.adjList[v])
            {
                if (indv[neighbor] == chosen_color)
                    ++fit;
            }
        }

        indv[v] = chosen_color;
    }
    return indv;
}
/// @brief Incluindo maior aleatoriedade
/// @return
Individual GreedyGraphColoring::run_pseudo_greedy_v2()
{
    indv.assign(graph.getNumVertices(), -1);
    Fitness fit{0};

    // Seleção dos vértices a serem pré-coloridos aleatoriamente
    std::set<int> pre_colored_vertices;
    while ((int)pre_colored_vertices.size() < std::max(3, (int)sqrt(graph.getNumVertices())))
    {
        int rand_vert{randint(0, graph.getNumVertices() - 1, rng)};
        pre_colored_vertices.insert(rand_vert);
    }
    // Atribuição aleatória de cores
    for (int v : pre_colored_vertices)
    {
        indv[v] = randint(0, num_color - 1, rng);
    }

    // Atribuir cores aos vértices com estratégia gulosa
    for (int v{0}; v < graph.getNumVertices(); ++v)
    {
        if (indv[v] != -1)
            continue;

        std::vector<bool> forbidden_colors(num_color, false);
        for (auto neighbor : graph.adjList[v])
        {
            if (indv[neighbor] != -1)
            {
                forbidden_colors[indv[neighbor]] = true;
            }
        }

        std::vector<int> available_colors;
        for (int color = 0; color < num_color; ++color)
        {
            if (!forbidden_colors[color])
            {
                available_colors.push_back(color);
                break;
            }
        }

        int chosen_color{-1};
        if (!available_colors.empty())
        {
            chosen_color = available_colors[randint(0, available_colors.size() - 1, rng)];
        }
        else
        {
            chosen_color = find_color_least_conflicts(v);
            for (auto neighbor : graph.adjList[v])
            {
                if (indv[neighbor] == chosen_color)
                    ++fit;
            }
        }

        indv[v] = chosen_color;
    }
    return indv;
}

Individual GreedyGraphColoring::run_pseudo_greedy_v3()
{
    indv.assign(graph.getNumVertices(), -1);
    Fitness fit{0};

    int num_precolored = std::max(3, static_cast<int>(sqrt(graph.getNumVertices())));

    // Generate a shuffled list of vertices
    std::vector<int> vertex_list(graph.getNumVertices());
    std::iota(vertex_list.begin(), vertex_list.end(), 0);
    std::shuffle(vertex_list.begin(), vertex_list.end(), rng);

    // Seleção dos vértices a serem pré-coloridos aleatoriamente
    std::set<int> pre_colored_vertices;
    for (int i{0}; i < num_precolored; i++)
    {
        pre_colored_vertices.insert(vertex_list[i]);
    }

    for (int v{0}; v < graph.getNumVertices(); ++v)
    {
        if (indv[v] != -1)
            continue;

        std::vector<bool> forbidden_colors(num_color, false);
        for (auto neighbor : graph.adjList[v])
        {
            if (indv[neighbor] != -1)
            {
                forbidden_colors[indv[neighbor]] = true;
            }
        }

        std::vector<int> available_colors;
        for (int color = 0; color < num_color; ++color)
        {
            if (!forbidden_colors[color])
            {
                available_colors.push_back(color);
                break;
            }
        }

        int chosen_color{-1};
        if (!available_colors.empty())
        {
            std::uniform_int_distribution<int> dist(0, available_colors.size() - 1);
            chosen_color = available_colors[dist(rng)];
        }
        else
        {
            chosen_color = find_color_least_conflicts(v);
            for (auto neighbor : graph.adjList[v])
            {
                if (indv[neighbor] == chosen_color)
                    ++fit;
            }
        }
        // Pequena chance de escolher uma cor aleatória (introduz ruído)
        if (std::bernoulli_distribution(0.1)(rng))
        {
            std::uniform_int_distribution<int> dist(0, num_color - 1);
            chosen_color = dist(rng);
        }

        indv[v] = chosen_color;
    }
    return indv;
}

Individual GreedyGraphColoring::run()
{
    /*
     Inicializar a lista de cores e contador de conflitos
    */
    indv.assign(graph.getNumVertices(), -1); // Preencher com -1 diretamente
    Fitness fit{0};

    // Atribuir cores aos vértices
    for (int v{0}; v < graph.getNumVertices(); ++v)
    {
        std::vector<bool> forbidden_colors(num_color, false);
        for (auto neighbor : graph.adjList[v])
        {
            if (indv[neighbor] != -1)
            {
                forbidden_colors[indv[neighbor]] = true;
            }
        }

        int chosen_color = -1;
        for (int color = 0; color < num_color; ++color)
        {
            if (!forbidden_colors[color])
            {
                chosen_color = color;
                break;
            }
        }

        if (chosen_color == -1)
        {
            chosen_color = find_color_least_conflicts(v);
            for (auto neighbor : graph.adjList[v])
            {
                if (indv[neighbor] == chosen_color)
                    ++fit;
            }
        }

        indv[v] = chosen_color;
    }
    return indv;
}

int GreedyGraphColoring::find_color_least_conflicts(int current_vert)
{
    std::vector<int> conflicts_by_color(num_color, 0);

    for (int color{0}; color < num_color; ++color)
    {
        for (auto neighbor : graph.adjList[current_vert])
        {
            if (indv[neighbor] == color)
                conflicts_by_color[color]++;
        }
    }
    int least_conflict_color{0};
    for (int i{1}; i < num_color; ++i)
    {
        if (conflicts_by_color[i] < conflicts_by_color[least_conflict_color])
            least_conflict_color = i;
    }
    return least_conflict_color;
}