#include "graphs.h"
#include "solution.h"
#include "metaheuristics.h"

#include <iostream>
#include <fstream>
#include <cstdlib> // Para usar rand()
#include <ctime>   // Para usar time()
#include <chrono>

int main(int argc, char *argv[])
{
    srand(time(nullptr));
    if (argc < 8)
    {
        std::cout << "Please, provide parameters: <instance_path> <num_colors> <num_bees> <max_iter> <limit> <output_path> <method>" << std::endl;
        return 1;
    }

    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename{argv[1]};
    Graph graph(filename);
    int num_color{atoi(argv[2])};
    int num_bees{atoi(argv[3])};
    int limit{atoi(argv[4])};
    int max_iter{atoi(argv[5])};
    char method{argv[7][0]};

    Individual indv{};
    Fitness fit{};

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    ABCGraphColoring abc = ABCGraphColoring(num_bees, num_color, max_iter, limit, graph);

    indv = abc.run(method);
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

    // Gravar os resultados no arquivo de saída
    output_file << fit << ';' << time_span.count() << ';' << abc.iter_found_best << ';'
                << num_bees << ';' << limit << ';' << max_iter << ';'
                << filename << ';' << graph.getNumVertices()
                << ';' << graph.getNumEdges() << std::endl;

    // Fechar o arquivo de saída
    output_file.close();

    // std::cout << fit << ';' << time_span.count() << ';'
    //           << filename << ';' << graph.getNumVertices()
    //           << ';' << graph.getNumEdges() << std::endl;
    return 0;
}
// g++ run_main_abc.cpp lib/graphs.cpp lib/solution.cpp lib/metaheuristics.cpp lib/utils.cpp -I include -o run_abc
// ./run_abc