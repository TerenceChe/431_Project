#include "utils.h"

void CustomBarrier::wait() {
    std::unique_lock<std::mutex> u_lock(my_mutex_);
    int c = barrier_call_;
    current_waiting_++;
    if (current_waiting_ == num_of_threads_) {
      current_waiting_ = 0;
      // Unlock and send signal to wake up.
      barrier_call_++;
      u_lock.unlock();
      my_cv_.notify_all();
      return;
    }
    my_cv_.wait(u_lock, [&] { return (c != barrier_call_); });
        // Condition has been reached: return.
}

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

void graph_vector_to_file(const std::vector<std::vector<int>> matrix, std::string filename) {
    std::ofstream outfile(filename);
    int num_verts = matrix.size();
    if (outfile.is_open()) {
    for (int i = 0; i < num_verts; i++) {
        for (int j = 0; j < num_verts; j++) {
            outfile << matrix[i][j] << " ";
        }
        outfile << "\n";
    }
    outfile.close();
    } else {
        std::cerr << "Unable to open file: " << filename << "\n";
    }
}
