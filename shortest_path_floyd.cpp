#include "shortest_path_floyd.h"

void serial(Graph* g) {
    timer timer;
    uint size = g->height;

    #ifdef PRINT
    std::cout << "graph before >>>> " << std::endl;
    g->printGraph();
    #endif

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
    
    #ifdef PRINT
    std::cout << "graph after >>>> " << std::endl;
    g->printGraph();
    #endif
}

void iterate(uint start_col, uint end_col, uint size, Graph *g, CustomBarrier *barrier, double *time_taken) {
    timer t1;
    t1.start();
    for (int k = 0; k < size; k++) {
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

    #ifdef PRINT
    std::cout << "graph before >>>> " << std::endl;
    g->printGraph();
    #endif
    
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

    #ifdef PRINT
    std::cout << "graph after >>>> " << std::endl;
    g->printGraph();
    #endif

    std::cout << "Time taken (in seconds) : \n" << std::setprecision(TIME_PRECISION);
    for (int i = 0; i < np; i++) {
        std::cout << i << ": "  << times[i] << "\n";
    }
    std::cout << "Overall: " << overall_time << '\n';
}
