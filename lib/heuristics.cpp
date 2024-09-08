#include "heuristics.h"
#include "graphs.h"
#include "solution.h"
#include "utils.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>

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