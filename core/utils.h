#ifndef UTILS_H
#define UTILS_H

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <vector>
#include <limits.h>
#include "get_time.h"
#include "cxxopts.h"

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

struct CustomBarrier {
  int num_of_threads_;
  int current_waiting_;
  int barrier_call_;
  std::mutex my_mutex_;
  std::condition_variable my_cv_;

  CustomBarrier(int t_num_of_threads)
      : num_of_threads_(t_num_of_threads), current_waiting_(0),
        barrier_call_(0) {}

  void wait() {
    std::unique_lock<std::mutex> u_lock(my_mutex_);
    int c = barrier_call_;
    current_waiting_++;
    if (current_waiting_ == num_of_threads_) {
      current_waiting_ = 0;
      // unlock and send signal to wake up
      barrier_call_++;
      u_lock.unlock();
      my_cv_.notify_all();
      return;
    }
    my_cv_.wait(u_lock, [&] { return (c != barrier_call_); });
    //  Condition has been reached. return
  }
};

// Returns a vector with column splits.
// E.g. 8 columns, 4 procs => [0, 2, 4, 6, 8].
std::vector<int> get_col_bounds(uint num_cols, uint num_procs) {
    std::vector<int> col_bounds;
    col_bounds.push_back(0);

    uint col_per_thread = (uint) (num_cols / num_procs);
    uint excess_cols = (uint) (num_cols % num_procs);

    uint last_end = 0;
    for (uint i = 0; i < num_procs; i++) {
        uint start_col = last_end; // Inclusive bound.
        uint end_col = start_col + col_per_thread; // Not an inclusive bound.
        if (excess_cols > 0) {
            end_col++;
            excess_cols--;
        }
        last_end = end_col;
        col_bounds.push_back(end_col);
    }

    return col_bounds;
}

#endif
