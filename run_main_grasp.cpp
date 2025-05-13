#include "GRASPGraphColoring.h"

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
    char method{argv[3][0]};

    Individual indv{};
    Fitness fit{};

    int max_rcl_size{num_color / 2};
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    GRASPGraphColoring grasp = GRASPGraphColoring(graph, num_color, rng, max_rcl_size);

    if (method == 'b') {
        indv = grasp.BuildPhase();
    } else if (method == 'g') {
        indv = grasp.run();
    }

    evaluate_fitness(graph, indv, fit);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);

    // Abrir o arquivo de saída
    std::string output_filename = argv[4];
    std::ofstream output_file(output_filename, std::ios::app);

    if (!output_file.is_open()) // Verificar se o arquivo foi aberto corretamente
    {
        std::cerr << "Error opening file: " << output_filename << std::endl;
        return 1;
    }

    // Gravar os resultados no arquivo de saída
    output_file << fit << ';' << time_span.count() << ';'
                << filename << ';' << graph.getNumVertices()
                << ';' << graph.getNumEdges() << std::endl;

    // Fechar o arquivo de saída
    output_file.close();

    return 0;
}
// g++ run_main_grasp.cpp lib/graphs.cpp lib/solution.cpp lib/metaheuristics.cpp lib/utils.cpp -I include -o run_grasp
// ./run_grasp