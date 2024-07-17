#include <iostream>
#include <vector>

struct Edge{
    int src;
    int dest;
}

class Graph {
    vector<vector<int>> adjList;

public:
    Graph(vector<Edge> const &edges, int n){
        adjList.resize(n);

        for (auto &edge: edges){
            adjList[edge.src].push_back(edge.dest);
            adjList[edge.dst].push_back(edge.src);
        }
    }

    auto findEdgeIfExists(int nodeA, int nodeB){
         for (auto it = adjList[nodeA].begin(); it != adjList[nodeA].end(); ++it) {
            if (it->first == nodeB) {
                return it;
            }
        }
        return adjList[nodeA].end(); // Indica que a aresta não existe
    }
}

void printGraph(Graph const& graph, int n) {
    for(int i{0}; i < n; ++i){
        // print the current vertex number
        std::cout << i << "--> ";

        // print all neighboring vertices of a vertex `i`
        for (int v: graph.adjList[i]){
            std::cout << v << " ";
        }
        std::cout << endl;
    }
}