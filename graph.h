#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include <iostream>
#include <vector>
#include <climits>
#include "utils.h"

class Graph {
private:
    std::vector<std::vector<int>> matrix;
    uint num_verts = -1;

public:
    Graph() = default;
    ~Graph() = default;
    uint getNumVerts() const;
    int getWeight(uint from, uint to) const;
    void setWeight(uint from, uint to, int value);
    // Check if underlying matrices are identical.
    bool operator==(const Graph& other) const;
    void printGraph() const;
    void readGraphFromFile(std::string input_file_path);
    void writeGraphToFile(std::string filename) const;
    void printDistance() const;
};

#endif
