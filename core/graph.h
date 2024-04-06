#include <fstream>
#include <iostream>

class Graph {
    std::vector<std::vector<int>> matrix;

public:
    uint height;
    uint width;

    Graph() = default;

    void readGraphFromFile(std::string input_file_path) {
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

        // Assert 0 diagonal.
        for (uint i = 0; i < height; i++) {
            if (getWeight(i, i) != 0) {
                std::cerr << "weight at: " << i << "," << i << " must be 0 " << std::endl;
                exit(1);
            }
        }
    }
  
    void printGraph() {
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

    int getWeight(uint from, uint to) {
        return matrix[from][to];
    }

    void setWeight(uint from, uint to, int value) {
        matrix[from][to] = value;
    }

    // Check if two distancematrices are identical.
    bool operator==(const Graph& other) const {
        // Compare height and width:
        if (height != other.height || width != other.width)
            return false;

        // Compare matrix elements:
        for (uint i = 0; i < height; ++i) {
            for (uint j = 0; j < width; ++j) {
                if (matrix[i][j] != other.matrix[i][j])
                    return false;
            }
        }

        return true;
    }
  
    ~Graph() {}
};
