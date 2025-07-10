#include "GRASPGraphColoringV2.h"

#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cout << "Please, provide parameters: <instance_path> <num_colors> <method> <output_path>" << std::endl;
        return 1;
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);

    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename{argv[1]};
    Graph graph(filename);
    int num_color{atoi(argv[2])};
    int max_iter{atoi(argv[3])};
    int alpha_int{atoi(argv[4])};
    char method{argv[5][0]};

    double alpha{alpha_int / 10.0};

    Individual indv{};
    Fitness fit{};

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    GRASPGraphColoringV2 grasp = GRASPGraphColoringV2(graph, num_color, rng, alpha, max_iter);

    if (method == 's') {
        indv = grasp.run_swap();
    } else if (method == 'k') {
        indv = grasp.run_kempe();
    } else if (method == 'o') {
        indv = grasp.run_one_move();
    }

    evaluate_fitness(graph, indv, fit);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);

    // Abrir o arquivo de saída
    std::string output_filename = argv[6];
    std::ofstream output_file(output_filename, std::ios::app);

    if (!output_file.is_open()) // Verificar se o arquivo foi aberto corretamente
    {
        std::cerr << "Error opening file: " << output_filename << std::endl;
        return 1;
    }

    std::string init_method;
    if (method == 's') {
        init_method = "swap";
    } else if (method == 'k') {
        init_method = "kempe";
    } else if (method == 'o') {
        init_method = "one";
    }
    // Gravar os resultados no arquivo de saída
    output_file << fit << ';' << time_span.count() << ';'
                << grasp.iter_found_best << ';'
                << alpha << ';' << max_iter << ';' 
                << num_color << ';' << init_method << ';'
                << filename << ';' << graph.getNumVertices()
                << ';' << graph.getNumEdges() << std::endl;

    // Fechar o arquivo de saída
    output_file.close();

    return 0;
}
// g++ run_main_grasp_v2.cpp lib/graphs.cpp lib/solution.cpp lib/heuristics.cpp lib/metaheuristics.cpp lib/utils.cpp lib/GRASPGraphColoringV2.cpp -I include -o run_main_grasp_v2
// ./run_main_grasp_v2 instances/le450_25a.col 25 10 10 s results_grasp_v2/analysis/one/le450_25a.txt
// ./run_main_grasp_v2 filename num_color max_iter alpha_int method