#include "shortest_path_floyd.h"

#define DEFAULT_MODE "0"
#define DEFAULT_NUMBER_N "1"

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
    if (mode == 1) {
        threaded(&g, np);
    }

    return 0;
}