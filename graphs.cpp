#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

struct Edge {
    int src;
    int dest;
};

class Graph {
    std::vector<std::vector<int>> adjList;

public:
    // Construtor para criar o grafo a partir de um vetor de arestas e número de vértices
    Graph(const std::vector<Edge> &edges, int n) {
        adjList.resize(n);
        for (auto &edge: edges) {
            adjList[edge.src].push_back(edge.dest);
            adjList[edge.dest].push_back(edge.src);  // Para grafos não direcionados
        }
    }

    // Construtor para criar o grafo a partir de um arquivo DIMACS
    Graph(const std::string &filename) {
        int n = 0, m = 0;
        std::vector<Edge> edges;

        // Abrir o arquivo para leitura
        std::ifstream infile(filename);
        if (!infile) {
            std::cerr << "Erro ao abrir o arquivo!" << std::endl;
            return;
        }

        std::string line;
        while (std::getline(infile, line)) {
            if (line[0] == 'p') {
                std::istringstream iss(line);
                std::string dummy;
                iss >> dummy >> dummy >> n >> m;
                adjList.resize(n);
            } else if (line[0] == 'e') {
                std::istringstream iss(line);
                char dummy;
                int src, dest;
                iss >> dummy >> src >> dest;
                src--; // Ajuste para índices baseados em 0
                dest--;
                edges.push_back({src, dest});
            }
        }

        for (auto &edge: edges) {
            adjList[edge.src].push_back(edge.dest);
            adjList[edge.dest].push_back(edge.src);  // Para grafos não direcionados
        }
    }

    // Método para verificar a existência de uma aresta
    auto findEdgeIfExists(int nodeA, int nodeB) {
        for (auto it = adjList[nodeA].begin(); it != adjList[nodeA].end(); ++it) {
            if (*it == nodeB) {
                return it;
            }
        }
        return adjList[nodeA].end(); // Indica que a aresta não existe
    }

    // Método para imprimir o grafo
    void printGraph() const {
        for (int i = 0; i < adjList.size(); ++i) {
            std::cout << i + 1 << "--> ";  // Ajuste para exibir índices baseados em 1
            for (int v : adjList[i]) {
                std::cout << v + 1 << " ";  // Ajuste para exibir índices baseados em 1
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    // Exemplo de uso: carregando um grafo a partir de um arquivo DIMACS
    std::string filename = "myciel3.col";
    Graph graph(filename);

    // Exibe o grafo
    graph.printGraph();

    return 0;
}
