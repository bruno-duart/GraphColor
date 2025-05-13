#ifndef HEADER_SAGRAPHCOLORING
#define HEADER_SAGRAPHCOLORING

#include "metaheuristics.h"

class SimulatedAnnealingColoring : public MetaHeuristic
{
private:
    int Temp_Max;
    int Temp_Min;
    int Max_iter_no_improv;
    int Max_worsen;
    double alpha;

public:
    SimulatedAnnealingColoring(const Graph &graph, int num_colors,
                               std::mt19937 &rng, int T_max, int T_min, int Max_iter_no_improv, int Max_worsen, double alpha)
        : MetaHeuristic(graph, num_colors, rng), Temp_Max{T_max}, Temp_Min{T_min}, alpha{alpha}, Max_iter_no_improv{Max_iter_no_improv}, Max_worsen{Max_worsen} {}
    Individual run();
    Individual run_v2();

    int curr_Temp{0};
};


#endif