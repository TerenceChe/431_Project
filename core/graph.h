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

    std::string line;
    while (std::getline(file, line)) {
      std::vector<int> row;
      size_t pos = 0;
      while ((pos = line.find(' ')) != std::string::npos) {
        int token = std::stoi(line.substr(0, pos));
        
        if (token < 0) {
          row.push_back(INT_MAX);
        } else {
          row.push_back(token);
        }
        line.erase(0, pos + 1);
      }
      // when we get here, we can't find any more space but the line might not be empty, so we do this check here to get the last 
      if (!line.empty()) {
        int token = std::stoi(line);
        if (token < 0) {
          row.push_back(INT_MAX);
        } else {
          row.push_back(token);
        }
      }
      matrix.push_back(row);
    }
    file.close();

    height = matrix.size();
    if (height > 0) {
      for (int i = 0; i < height - 1; i++) {
        if (matrix[i].size() != matrix[i+1].size()) {
          std::cerr << "width must be the same in your matrix" << std::endl;
          exit(1);
        }
      }
      width = matrix[0].size();
    }
    
    if (height != width) {
      std::cerr << "height and width of matrix must be the same" << std::endl;
      exit(1);
    }

    for (int i = 0; i < height; i++) {
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
          std::cout << "inf ";
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
  
  ~Graph() {}
};
