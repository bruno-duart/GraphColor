#include "metaheuristics.h"
#include <iostream>

int main(){
    srand(time(nullptr));

    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename = "instances/myciel3.col";
    Graph graph(filename);
    int num_color = 4;
    int T_iter{10};

    TabuColoring tabu(T_iter, num_color, graph);

    // tabu.print_tabu_list();

    tabu.insert_tabu_move(3, 4);
    
    // tabu.print_tabu_list();
    
    tabu.insert_tabu_move(3, 5);
    
    tabu.print_tabu_list();

    std::cout << "(2, 4) É Tabu? " << tabu.is_tabu_move(2, 4) << std::endl;

    tabu.decrease_iterations();
    
    tabu.print_tabu_list();
    tabu.insert_tabu_move(4, 5);
    
    tabu.decrease_iterations();
    tabu.decrease_iterations();
    tabu.decrease_iterations();
    tabu.decrease_iterations();
    tabu.decrease_iterations();
    tabu.decrease_iterations();
    tabu.decrease_iterations();    
    tabu.decrease_iterations();
    tabu.print_tabu_list();
    
    tabu.decrease_iterations();
    tabu.print_tabu_list();

    return 0;
}

//g++ main_test_tabu_search.cpp lib/heuristics.cpp lib/utils.cpp lib/fwlist.cpp lib/metaheuristics.cpp lib/graphs.cpp lib/solution.cpp -I include -o main_test_tabu_search