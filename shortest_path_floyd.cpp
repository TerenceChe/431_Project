// Implementations of serial, threaded, and distributed algorithms.

#include "shortest_path_floyd.h"
#include "core/utils.h"
#include <thread>

void serial(Graph* g) {
    timer timer;
    uint size = g->getNumVerts();

    #ifdef PRINT
    std::cout << "graph before >>>> " << std::endl;
    g->printGraph();
    #endif

    timer.start();
    for (uint k = 0; k < size; k++) {
        for (uint i = 0; i < size; i++) {
            for (uint j = 0; j < size; j++) {
                int new_weight = g->getWeight(i,k) + g->getWeight(k,j);
                if (g->getWeight(i,k) != INT_MAX && g->getWeight(k,j) != INT_MAX && new_weight < g->getWeight(i,j)) {
                    g->setWeight(i,j, new_weight);
                }
            }
        }
    }
    std::cout << "Time taken (in seconds) : " << std::setprecision(TIME_PRECISION)
            << timer.stop() << "\n";
    
    #ifdef PRINT
    std::cout << "graph after >>>> " << std::endl;
    g->printGraph();
    #endif
}

void iterate(uint start_col, uint end_col, uint size, Graph *g, CustomBarrier *barrier, double *time_taken) {
    timer t1;
    t1.start();
    for (uint k = 0; k < size; k++) {
        for (uint i = 0; i < size; i++) {
            for (uint j = start_col; j < end_col; j++) {
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

    #ifdef PRINT
    std::cout << "graph before >>>> " << std::endl;
    g->printGraph();
    #endif
    
    CustomBarrier barrier(np);
    uint size = g->getNumVerts();
    double times[np] = { 0 };
    std::thread threads[np];
    std::vector<int> col_bounds = get_col_bounds(size, np);

    for (uint i = 0; i < np; i++) {
        threads[i] = std::thread(iterate, col_bounds[i], col_bounds[i + 1], size, g, &barrier, &times[i]);
    }

    for (uint i = 0; i < np; i++) {
        threads[i].join();
    }

    double overall_time = t1.stop();

    #ifdef PRINT
    std::cout << "graph after >>>> " << std::endl;
    g->printGraph();
    #endif

    std::cout << "Time taken (in seconds) : \n" << std::setprecision(TIME_PRECISION);
    for (uint i = 0; i < np; i++) {
        std::cout << i << ": "  << times[i] << "\n";
    }
    std::cout << "Overall: " << overall_time << '\n';
}

void distrib(Graph *g) {

}
