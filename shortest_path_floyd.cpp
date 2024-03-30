#include <iostream>
#include <map>
#include "core/utils.h"
#include "core/graph.h"

#define DEFAULT_MODE "0"
#define DEFAULT_NUMBER_N "1"

void serial(Graph* g) {
    timer timer;
    uint size = g->height;

    std::cout << "graph before >>>> " << std::endl;

    g->printGraph();

    timer.start();
    for (int k = 0; k < size; k++) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                // std::cout << "i: " << i << ", k: " << k << " is " << g->getWeight(i,k) << std::endl;
                // std::cout << "k: " << k << ", j: " << j << " is " << g->getWeight(k,j) << std::endl;
                // std::cout << "i: " << i << ", j: " << j << " is " << g->getWeight(i,j) << std::endl;
                int new_weight = g->getWeight(i,k) + g->getWeight(k,j);
                if (g->getWeight(i,k) != INT_MAX && g->getWeight(k,j) != INT_MAX && new_weight < g->getWeight(i,j)) {
                    g->setWeight(i,j, new_weight);
                }
            }
        }
    }
    std::cout << "Time taken (in seconds) : " << std::setprecision(TIME_PRECISION)
            << timer.stop() << "\n";
    
    std::cout << "graph after >>>> " << std::endl;

    g->printGraph();
}

int main(int argc, char *argv[]) {

    cxxopts::Options options(
        "all pairs shortest path",
        "gets the shortest path for all pairs in the graph");

    options.add_options(
      "",
      {
           { "mode", "a number repesenting the mode, 0 - serial, 1 - threads, 2 - MPI",
            cxxopts::value<uint>()->default_value(DEFAULT_MODE)},
          {"np", "number of threads of processes, default is 1 for default mode",
           cxxopts::value<uint>()->default_value(DEFAULT_NUMBER_N)},
          {"inputFile", "Input graph file path",
           cxxopts::value<std::string>()->default_value(
               "/cmpt431_project/input_graph/test_data_1.txt")},
      });

    std::map<uint, std::string> mode_map = {
        {0, "serial"},
        {1, "threads"},
        {2, "MPI"}
    };
        

    auto cl_options = options.parse(argc, argv);
    uint mode = cl_options["mode"].as<uint>();
    uint np = cl_options["np"].as<uint>();
    std::string input_file_path = cl_options["inputFile"].as<std::string>();    

    if (mode < 0 || mode > 2) {
        std::cout << "invalid mode" << std::endl;
        return 1;
    }

    if (mode > 0 && np < 2) {
        std::cout << "for thread and MPI mode, you must have n > 1" << std::endl;
    }

    Graph g; 
    g.readGraphFromFile(input_file_path);
    
    if (mode == 0) {
        np = 1;
    }

    std::cout << "Number of Threads/Processes : " << np << std::endl;
    std::cout << "Mode : " << mode_map[mode] << std::endl;

    if (mode == 0) {
        serial(&g);
    }

    return 0;
}