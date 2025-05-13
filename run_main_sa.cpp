#include "SAGraphColoring.h"

#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

int main(int argc, char *argv[])
{    
    if (argc < 3)
    {
        std::cout << "Please, provide parameters: <instance_path> <num_colors> <output_path>" << std::endl;
        return 1;
    }    
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);

    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename = argv[1];
    Graph graph(filename);
    int num_color = atoi(argv[2]);
    int T_max{atoi(argv[3])};
    int T_min{10};
    int max_iter{1000};
    int max_worsen{5000};
    int alpha_int{atoi(argv[4])};
    double alpha{alpha_int / (10.0)};

    Individual indv{};
    Fitness fit{};

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    
    SimulatedAnnealingColoring sa(graph, num_color, rng, T_max, T_min, max_iter, max_worsen, alpha);

    indv = sa.run();
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
    output_file << fit << ';' << time_span.count() << ';' << T_max << ';' 
                << T_min << ';' << max_iter << ';' << max_worsen << ';' 
                << alpha << ';' << num_color << ';'<< sa.iter_found_best << ';' 
                << sa.curr_Temp << ';' << filename << ';' 
                << graph.getNumVertices() << ';' 
                << graph.getNumEdges() << ';' << "random" << std::endl;


    // Fechar o arquivo de saída
    output_file.close();

    return 0;
}
// g++ lib/run_main_greedy.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/utils.cpp -I include -o run_greedy
// ./run_greedy