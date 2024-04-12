#include "graph.h"

uint get_next_node(uint current_node, int end_node, const Graph &graph, const Graph &distances) {
    std::vector<int> adjs;
    for (uint node = 0; node < graph.getNumVerts(); node++) {
        if (graph.getWeight(current_node, node) != INT_MAX && node != current_node) {
            adjs.push_back(node);
        }
    }
    // From the nodes that connect to current node, which one is the closest to start_node?
    int closest_distance = INT_MAX;
    uint closest_node = 0;
    for (int adj : adjs) {
        int current_distance = graph.getWeight(current_node, adj) + distances.getWeight(adj, end_node);
        if (current_distance < closest_distance) {
            closest_distance = current_distance;
            closest_node = adj;
        }
    }

    return closest_node;
}

int main(int argc, char *argv[]) {
    cxxopts::Options options(
        "Prints shortest path from graph matrix file and distance matrix file");

    options.add_options(
        "",
        {
            {"graph", "Input graph file",
                cxxopts::value<std::string>()->default_value(
                "./input_graph/test_data_1.txt")},
            {"distances", "Distance matrix",
                cxxopts::value<std::string>()->default_value(
                "./distance_matrix/test_data_1_serial_threaded.txt")},
            {"start", "Start node",
                cxxopts::value<uint>()->default_value("0")},
            {"end", "End node",
                cxxopts::value<uint>()->default_value("1")},
        });
    
    auto cl_options = options.parse(argc, argv);
    std::string graph_file = cl_options["graph"].as<std::string>();
    std::string distances_file = cl_options["distances"].as<std::string>();
    uint start_node = cl_options["start"].as<uint>();
    uint end_node = cl_options["end"].as<uint>();

    Graph graph;
    graph.readGraphFromFile(graph_file);
    
    uint num_verts = graph.getNumVerts();
    if (start_node >= num_verts || end_node >= num_verts) {
        std::cerr << "Invalid start or end node. ABORTING.\n";
        return 1;
    }

    Graph distances;
    distances.readGraphFromFile(distances_file);

    if (distances.getNumVerts() != num_verts) {
        std::cerr << "Distance matrix and graph are of different sizes. ABORTING.\n";
        return 1;
    }

    std::vector<int> shortest_path;
    shortest_path.push_back(start_node);
    uint current_node = start_node;

    uint shortest_path_distance = distances.getWeight(start_node, end_node);
    if (shortest_path_distance == INT_MAX) {
        std::printf("There is no path from %d to %d.\n", start_node, end_node);
    }

    while (current_node != end_node) {
        current_node = get_next_node(current_node, end_node, graph, distances);
        shortest_path.push_back(current_node);
    }

    std::printf(
        "Shortest path (distance %d) from %d to %d:\n", 
        shortest_path_distance, 
        start_node, 
        end_node);
    
    for (uint i = 0; i < shortest_path.size(); i++) {
        std::printf("%d", shortest_path[i]);
        if (i != shortest_path.size() - 1) {
            std::printf(" -> ");
        }
    }

    std::printf("\n");

    return 0;
}
