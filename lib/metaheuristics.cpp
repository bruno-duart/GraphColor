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
