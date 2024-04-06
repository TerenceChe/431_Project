// Driver to run distributed implementation (MPI).

#include "shortest_path_floyd.h"
#include "core/cxxopts.h"

int main(int argc, char **argv) {
    cxxopts::Options options(
        "all pairs shortest path",
        "gets the shortest path for all pairs in the graph");

    options.add_options(
        "",
        {
            {"inputFile", "Input graph file path",
            cxxopts::value<std::string>()->default_value(
                "./input_graph/test_data_1.txt")},
        });

    auto cl_options = options.parse(argc, argv);
    std::string input_file_path = cl_options["inputFile"].as<std::string>();
    
    Graph g; 
    g.readGraphFromFile(input_file_path);

    distrib(&g);
    return 0;
}
