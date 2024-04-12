#include "graph.h"
#include <random>

uint Graph::getNumVerts() const {
    return num_verts;
}

int Graph::getWeight(uint from, uint to) const {
    return matrix[from][to];
}

void Graph::setWeight(uint from, uint to, int value) {
    matrix[from][to] = value;
}

bool Graph::operator==(const Graph& other) const {
    if (num_verts != other.getNumVerts())
        return false;

    // Compare matrix elements:
    for (uint i = 0; i < num_verts; ++i) {
        for (uint j = 0; j < num_verts; ++j) {
            if (matrix[i][j] != other.matrix[i][j])
                return false;
        }
    }

    return true;
}

 void Graph::printGraph() const {
    for (auto row : matrix) {
        for (auto weight : row) {
            if (weight == INT_MAX) {
                std::cout << "i ";
            } else {
                std::cout << weight << " ";
            }
        }
        std::cout << std::endl;
    } 
}

void Graph::setMatrix(std::vector<std::vector<int>> m) {
    this->matrix = m;
}

void Graph::printDistance() const {
    
    int dataPairCount = 6;
    int totalVertices = getNumVerts();

    if (totalVertices < (dataPairCount * 2)) {
        std::cout << "I am not printing results for matrix that is too small" << std::endl;
        return;
    }

     int verticesPerThread = totalVertices / dataPairCount;
    int remainder = totalVertices % dataPairCount;
    std::vector<std::pair<int, int>> partitions(dataPairCount);

    int startVertex = 0;
    for (int i = 0; i < dataPairCount; ++i) {
        int v = verticesPerThread + (i < remainder ? 1 : 0);
        int endVertex = startVertex + v - 1;
        partitions[i] = std::make_pair(startVertex, endVertex);
        startVertex = endVertex + 1;
    }

    std::cout << "from, to, distance" << std::endl;
    for (auto pair : partitions) {
        int weight = getWeight(pair.first, pair.second);
        if (weight == INT_MAX) {
            std::cout << pair.first << ", " << pair.second << ", " << "infinity" << std::endl;
        } else {
            std::cout << pair.first << ", " << pair.second << ", " << weight << std::endl;
        }
    }

}

void Graph::readGraphFromFile(std::string input_file_path) {
    uint width = -1;
    uint height = -1;

    std::ifstream file(input_file_path);

    if (!file.is_open()) {
        std::cerr << "error: unable to open file " << input_file_path << std::endl;
        exit(1);
    }

    // Loop through input file line by line,
    // turn line into row vector, and push row into Graph::matrix.
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::string currentNumber;

        for (char c : line) {
            if (std::isdigit(c)) {
                currentNumber += c;
            } else if (c == 'i') {
                row.push_back(INT_MAX);
                currentNumber.clear();
            } else {
                if (!currentNumber.empty()) {
                    try {
                        row.push_back(std::stoi(currentNumber));
                        currentNumber.clear();
                    } catch (std::invalid_argument& e) {
                        std::cerr << "Could not convert text to number" << e.what() << std::endl;
                        exit(1);
                    }
                }
            }
        }

        if (!currentNumber.empty()) {
            try {
                row.push_back(std::stoi(currentNumber));
                currentNumber.clear();
            } catch (std::invalid_argument& e) {
                std::cerr << "Could not convert text to number" << e.what() << std::endl;
                exit(1);
            }
        }
        matrix.push_back(row);
    }
    file.close();

    // Assert that graph is a valid initial distance matrix.

    // Assert rectangular shape.
    height = matrix.size();
    if (height > 0) {
        for (uint i = 0; i < height - 1; i++) {
            if (matrix[i].size() != matrix[i+1].size()) {
                std::cerr << "width must be the same in your matrix" << std::endl;
                exit(1);
            }
        }
        width = matrix[0].size();
    }

    // Assert square shape.
    if (height != width) {
        std::cerr << "height and width of matrix must be the same " << "height: " << height << "width: " << width << std::endl;
        printGraph();
        exit(1);
    }

    num_verts = height; // Could have written num_verts = width, since they are equal.

    // Assert 0 diagonal.
    for (uint i = 0; i < height; i++) {
        if (getWeight(i, i) != 0) {
            std::cerr << "weight at: " << i << "," << i << " must be 0 " << std::endl;
            exit(1);
        }
    }
}

void Graph::writeGraphToFile(std::string filename) const {
    graph_vector_to_file(matrix, filename);
}
