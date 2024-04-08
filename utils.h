#ifndef UTILS_H
#define UTILS_H

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <fstream>
#include <mutex>
#include <vector>
#include <limits.h>
#include "core/get_time.h"
#include "core/cxxopts.h"

#define intV int32_t
#define uintV int32_t
#define UINTV_MAX INT_MAX

#define intE int32_t
#define uintE int32_t

#define DEFAULT_NUMBER_OF_THREADS "1"
#define DEFAULT_MAX_ITER "10"
#define TIME_PRECISION 5
#define VAL_PRECISION 14
#define THREAD_LOGS 0
// #define ADDITIONAL_TIMER_LOGS 0

class CustomBarrier {
private:
    int num_of_threads_;
    int current_waiting_;
    int barrier_call_;
    std::mutex my_mutex_;
    std::condition_variable my_cv_;
public:
    CustomBarrier(int t_num_of_threads) : num_of_threads_(t_num_of_threads), current_waiting_(0),
        barrier_call_(0) {};
    void wait();
};

// Returns a vector with column splits.
// E.g. 8 columns, 4 procs => [0, 2, 4, 6, 8].
std::vector<int> get_col_bounds(uint num_cols, uint num_procs);

void graph_vector_to_file(const std::vector<std::vector<int>> matrix, std::string filename);

#endif
