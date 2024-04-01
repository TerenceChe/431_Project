#include <cmath>
#include <iostream>
#include <map>
#include <thread>
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

void iterate(uint start_col, uint end_col, uint size, Graph *g, CustomBarrier *barrier, double *time_taken) {
    timer t1;
    t1.start();
    for (int k = 1; k < size; k++) {
        for (int i = 0; i < size; i++) {
            for (int j = start_col; j < end_col; j++) {
                // Infinite weight in one of the intermediate paths: not a real connection.
                if (g->getWeight(i,k) == INT_MAX || g->getWeight(k,j) == INT_MAX) {
                    continue;
                }
                int new_weight = g->getWeight(i,k) + g->getWeight(k,j);
                if (new_weight < g->getWeight(i,j)) {
                    g->setWeight(i, j, new_weight);
                }
            }
        }
        barrier->wait();
    }
    *time_taken = t1.stop();
}

void threaded(Graph *g, uint np) {
    timer t1;
    t1.start();
    std::cout << "graph before >>>> " << std::endl;
    g->printGraph();
    CustomBarrier barrier(np);
    uint size = g->height;
    uint col_per_thread = (uint) (size / np);
    uint excess_cols = (uint) (size % np);
    double times[np] = { 0 };

    std::thread threads[np];

    uint last_end = 0;
    for (uint i = 0; i < np; i++) {
        uint start_col = last_end;
        uint end_col = start_col + col_per_thread;
        if (excess_cols > 0) {
            end_col++;
            excess_cols--;
        }
        last_end = end_col;

        // std::printf("Start: %d, end: %d\n", start_col, end_col);
        threads[i] = std::thread(iterate, start_col, end_col, size, g, &barrier, &times[i]);
    }

    for (uint i = 0; i < np; i++) {
        threads[i].join();
    }

    double overall_time = t1.stop();
    std::cout << "graph after >>>> " << std::endl;
    g->printGraph();

    std::cout << "Time taken (in seconds) : \n" << std::setprecision(TIME_PRECISION);
    for (int i = 0; i < np; i++) {
        std::cout << i << ": "  << times[i] << "\n";
    }
    std::cout << "Overall: " << overall_time << '\n';
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
    if (mode == 1) {
        threaded(&g, np);
    }

    return 0;
}