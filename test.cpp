#include <assert.h>
#include "shortest_path_floyd.h"

int main(int argc, char *argv[]) {
    assert(argc == 2); // Need to pass input_graph.txt as second argument.
    std::string input_file_path = argv[1];
    
    Graph g1; 
    g1.readGraphFromFile(input_file_path);

    Graph g2;
    g2.readGraphFromFile(input_file_path);

    assert(g1 == g2); // Initially identical.
    serial(&g1);
    assert((g1 == g2) == false); // Serial updated g1 to be different.
    int np = 10;
    threaded(&g2, np);
    assert(g1 == g2); // Threaded updated g2 in the same way.
}
