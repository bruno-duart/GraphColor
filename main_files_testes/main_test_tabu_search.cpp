#include "TabuGraphColoring.h"
#include <iostream>
#include <random>
#include <chrono>

int main(){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);

    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename = "instances/le450_15c.col";
    Graph graph(filename);
    int num_color = 15;
    int T_iter{25};
    int max_iter{100};

    TabuColoring tabu(graph, num_color, T_iter, max_iter, rng);

    Individual indv = tabu.run();
    Fitness fit {};
    evaluate_fitness(graph, indv, fit);

    print_individual(indv, fit);

    // tabu.print_tabu_list();

    // tabu.insert_tabu_move(3, 4);
    
    // // tabu.print_tabu_list();
    
    // tabu.insert_tabu_move(3, 5);
    
    // tabu.print_tabu_list();

    // std::cout << "(2, 4) É Tabu? " << tabu.is_tabu_move(2, 4) << std::endl;

    // tabu.decrease_iterations();
    
    // tabu.print_tabu_list();
    // tabu.insert_tabu_move(4, 5);
    
    // tabu.decrease_iterations();
    // tabu.decrease_iterations();
    // tabu.decrease_iterations();
    // tabu.decrease_iterations();
    // tabu.decrease_iterations();
    // tabu.decrease_iterations();
    // tabu.decrease_iterations();    
    // tabu.decrease_iterations();
    // tabu.print_tabu_list();
    
    // tabu.decrease_iterations();
    // tabu.print_tabu_list();

    return 0;
}

//g++ main_test_tabu_search.cpp lib/heuristics.cpp lib/utils.cpp lib/fwlist.cpp lib/metaheuristics.cpp lib/graphs.cpp lib/solution.cpp -I include -o main_test_tabu_search