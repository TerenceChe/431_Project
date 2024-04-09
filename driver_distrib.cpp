// Driver to run distributed implementation (MPI).

#include "floyd_distrib.h"
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
            {"outputFile", "Output graph file path",
                cxxopts::value<std::string>()->default_value(
                "./output_graph/test_data_1_distrib.txt")},
        });

    auto cl_options = options.parse(argc, argv);
    std::string input_file_path = cl_options["inputFile"].as<std::string>();
    std::string output_file_path = cl_options["outputFile"].as<std::string>();
    
    distrib(input_file_path, output_file_path);
    return 0;
}
