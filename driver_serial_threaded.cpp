// Driver to run serial and threaded implementations.

#include "floyd_serial_threaded.h"
#include "core/cxxopts.h"

#define DEFAULT_MODE "0"
#define DEFAULT_NUMBER_N "1"

int main(int argc, char *argv[]) {
    cxxopts::Options options(
        "all pairs shortest path",
        "gets the shortest path for all pairs in the graph");

    options.add_options(
        "",
        {
            { "mode", "a number repesenting the mode, 0 - serial, 1 - threads",
                cxxopts::value<uint>()->default_value(DEFAULT_MODE)},
            {"np", "number of threads of processes, default is 1 for default mode",
                cxxopts::value<uint>()->default_value(DEFAULT_NUMBER_N)},
            {"inputFile", "Input graph file path",
                cxxopts::value<std::string>()->default_value(
                "./input_graph/test_data_1.txt")},
            {"outputFile", "Output graph file path",
                cxxopts::value<std::string>()->default_value(
                "./distance_matrix/test_data_1_serial_threaded.txt")},
        });

    std::map<uint, std::string> mode_map = {
        {0, "serial"},
        {1, "threads"},
    }; 

    auto cl_options = options.parse(argc, argv);
    uint mode = cl_options["mode"].as<uint>();
    uint np = cl_options["np"].as<uint>();
    std::string input_file_path = cl_options["inputFile"].as<std::string>();
    std::string output_file_path = cl_options["outputFile"].as<std::string>();

    if (mode < 0 || mode > 1) {
        std::cout << "invalid mode" << std::endl;
        return 1;
    }

    if (mode > 0 && np < 2) {
        std::cout << "for thread mode, you must have n > 1" << std::endl;
        return 1;
    }

    Graph g; 
    g.readGraphFromFile(input_file_path);
    
    if (mode == 0) {
        np = 1;
    }

    std::cout << "Number of Threads : " << np << std::endl;
    std::cout << "Number of Vertices : " << g.getNumVerts() << std::endl;
    std::cout << "Mode : " << mode_map[mode] << std::endl;

    if (mode == 0) {
        serial(&g);
    }
    if (mode == 1) {
        threaded(&g, np);
    }

    g.writeGraphToFile(output_file_path);

    return 0;
}
