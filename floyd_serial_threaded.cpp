// Implementations of serial and threaded algorithms.

#include "floyd_serial_threaded.h"
#include "utils.h"
#include <thread>


void serial(Graph* g) {
    timer timer;
    uint size = g->getNumVerts();

    #ifdef PRINT
    std::cout << "<<<< distance before >>>> " << std::endl;
    g->printDistance();
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

    #ifdef PRINT
    std::cout << "<<<< distance after >>>> " << std::endl;
    g->printDistance();
    #endif

    std::cout << "Time taken (in seconds) : " << std::setprecision(TIME_PRECISION)
            << timer.stop() << "\n";

}

void iterate(uint start_col, uint end_col, uint size, Graph *g, CustomBarrier *barrier, double *time_taken) {
    timer t1;
    t1.start();
    for (uint k = 0; k < size; k++) {
        for (uint i = 0; i < size; i++) {
            for (uint j = start_col; j < end_col; j++) {
                if (g->getWeight(i,k) == INT_MAX || g->getWeight(k,j) == INT_MAX) {
                    // Infinite weight in one of the intermediate paths: not a real connection.
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
    std::cout << "<<<< distance before >>>> " << std::endl;
    g->printDistance();
    #endif
    
    CustomBarrier barrier(np);
    uint size = g->getNumVerts();
    double* times = new double[np];
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
    std::cout << "<<<< distance after >>>> " << std::endl;
    g->printDistance();
    #endif

    std::cout << "Time taken (in seconds):\n" << std::setprecision(TIME_PRECISION);
    for (uint i = 0; i < np; i++) {
        std::cout << i << ": "  << times[i] << "\n";
    }
    std::cout << "Overall: " << overall_time << '\n';
    delete[] times;
}
