#include "graphs.h"
#include "solution.h"
#include "metaheuristics.h"

#include <iostream>
#include <cstdlib> // Para usar rand()
#include <ctime>   // Para usar time()

std::vector<Fitness> fitness;

int main()
{
    srand(time(nullptr));

    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename = "instances/le450_15a.col";
    Graph graph(filename);
    int num_color = 15;

    Individual indv{};
    Fitness fit{};

    int num_bees{400};
    int max_iter{1000};
    int limit{50};

    ABCGraphColoring abc = ABCGraphColoring(num_bees, num_color, max_iter, limit, graph);

    indv = abc.run();
    evaluate_fitness(graph, indv, fit);

    print_individual(indv, fit);

    // std::cout << fit << ';' << num_color << ';' << num_bees << ';' << limit << ';' << max_iter;

    return 0;
}

// g++ main.cpp lib/graphs.cpp lib/solution.cpp lib/metaheuristics.cpp lib/utils.cpp -I include -o main
//fprintf(results, "%d;%f;%d;%d;%d;%d;%d\n", best->num_conflitos, (double)time0/CLOCKS_PER_SEC, *iter, num_bees, num_colors, limit, max_iter);