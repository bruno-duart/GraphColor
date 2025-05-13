#include "heuristics.h"
#include "TabuGraphColoringV2.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cout << "Please, provide parameters: <instance_path> <num_colors> <T_iter> <max_iter> <output_path>" << std::endl;
        return 1;
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);

    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename = argv[1];
    Graph graph(filename);
    int num_color = atoi(argv[2]);
    int T_iter{atoi(argv[3])};
    int max_iter{atoi(argv[4])};

    Individual indv{};
    Fitness fit{};

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    
    TabuColoringV2 tabu(graph, num_color, max_iter, T_iter, rng);

    indv = tabu.run();
    evaluate_fitness(graph, indv, fit);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);

    // Abrir o arquivo de saída
    std::string output_filename = argv[5];
    std::ofstream output_file(output_filename, std::ios::app);

    if (!output_file.is_open()) // Verificar se o arquivo foi aberto corretamente
    {
        std::cerr << "Error opening file: " << output_filename << std::endl;
        return 1;
    }

    // Gravar os resultados no arquivo de saída
    output_file << fit << ';' << time_span.count() << ';' << tabu.iter_found_best << ';' 
                << max_iter << ';' << T_iter << ';' << num_color << ';'
                << filename << ';' << graph.getNumVertices() << ';' 
                << graph.getNumEdges() << ';' << "greedy" << std::endl;

    // Fechar o arquivo de saída
    output_file.close();

    return 0;
}
// g++ run_main_tabu_v2.cpp lib/graphs.cpp lib/solution.cpp lib/utils.cpp lib/heuristics.cpp lib/metaheuristics.cpp lib/TabuGraphColoringV2.cpp -I include -o run_tabu_v2
// ./run_tabu_v2 instances/le450_15a.col 15 10 10 results_tabu_v2/le450_15a.txt

//./run_tabu_v2 instances/qg.order60.col 60 25 10000 results_tabu_v2/qg.order60.txt