// Test that the threaded implementation produces the same results as
// the serial one.

#include <assert.h>
#include "floyd_serial_threaded.h"
#include "core/cxxopts.h"


int main(int argc, char *argv[]) {
     cxxopts::Options options(
        "all pairs shortest path",
        "gets the shortest path for all pairs in the graph");
    
    options.add_options(
        "",
        {
            {"inputFile", "Input graph file path",
            cxxopts::value<std::string>()->default_value(
                "./input_graph/random_graph.txt")},
        });
    
    auto cl_options = options.parse(argc, argv);
    std::string input_file_path = cl_options["inputFile"].as<std::string>();
    
    Graph g1; 
    g1.readGraphFromFile(input_file_path);

    Graph g2;
    g2.readGraphFromFile(input_file_path);

    assert(g1 == g2); // Initially identical.
    serial(&g1);
    assert((g1 == g2) == false); // Serial updated g1 to be different.
    int np = 6; // Good number of processes to see speedup on my system.
    threaded(&g2, np);
    assert(g1 == g2); // Threaded updated g2 in the same way.
    std::printf("All assertions passed\n");
}
