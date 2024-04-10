// Process 0 is leader process and reads input file into matrix.
// Then it sends submatrices (by column spits, called `proc_buffer`) to follower processes.
// Follower processes update distances in `proc_buffer` using floyd algorithm.
// Then follower processes send `proc_buffer` back to leader process.
// The leader process combines the `proc_buffer`s into a matrix and writes to output file.

#include <iostream>
#include <vector>
#include <climits>
#include <mpi.h>
#include "graph.h"
#include "utils.h"
#include "floyd_distrib.h"

template <typename T>
T get_max(const T* ar, size_t arlen) {
    T max = ar[0];
    for (size_t i = 1; i < arlen; ++i) {
        if (ar[i] > max) {
            max = ar[i];
        }
    }
    return max;
}

// Return broadcaster process rank for a certain k (it has column k in its proc_buffer).
int get_broadcaster(std::vector<int> col_bounds, int k) {
    uint i;
    for (i = 1; i < col_bounds.size(); i++) {
        int end_col = col_bounds[i];
        if (k < end_col) {
            break;
        }
    }
    return i - 1;
}

void distrib(std::string input_file_path, std::string output_file_path) {
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    int num_verts = 0;
    Graph *graph = NULL;
    double root_start_time = -1;
    double *proc_times = NULL;
    if (world_rank == 0) {
        graph = new Graph();
        graph->readGraphFromFile(input_file_path);
        num_verts = graph->getNumVerts();
        if (world_size > num_verts) {
            std::cerr << "Too many processes for this graph.\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        } else {
            printf("Num verts %d\n", num_verts);
        }
        proc_times = new double[world_rank];
        root_start_time = MPI_Wtime();
    }

    double proc_start_time = MPI_Wtime();

    MPI_Bcast(&num_verts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    std::vector<int> col_bounds = get_col_bounds(num_verts, world_size);

    int proc_start_col = col_bounds[world_rank];
    int proc_end_col = col_bounds[world_rank + 1];
    int proc_width = proc_end_col - proc_start_col;
    int proc_width_max = num_verts / world_size + 1;
    int proc_buffer_len = proc_width * num_verts;
    // Goal is to split graph so that each proc (including root) has its data in proc_buffer.
    int *proc_buffer = new int[proc_buffer_len]; 

    if (world_rank == 0) {
        // Copy from graph to proc_buffer:
        int proc_buffer_i = 0;
        for (int i = 0; i < num_verts; i++) {
            for (int j = proc_start_col; j < proc_end_col; j++) {
                int weight = graph->getWeight(i, j);
                proc_buffer[proc_buffer_i] = weight;
                proc_buffer_i++;
            }
        }

        // Send graph segments to all of the other processes:
        int *other_buffer = new int[proc_width_max * num_verts];
        for (int other_proc = 1; other_proc < world_size; other_proc++) {
            int other_start_col = col_bounds[other_proc];
            int other_end_col = col_bounds[other_proc + 1];
            int other_buffer_len = (other_end_col - other_start_col) * num_verts;

            
            int other_buffer_i = 0;
            for (int i = 0; i < num_verts; i++) {
                for (int j = other_start_col; j < other_end_col; j++) {
                    int weight = graph->getWeight(i, j);
                    other_buffer[other_buffer_i] = weight;
                    other_buffer_i++;
                }
            }
            MPI_Send(other_buffer, other_buffer_len, MPI_INT, other_proc, 0, MPI_COMM_WORLD);
        }
        delete[] other_buffer;
    } else {
        // Receive graph section from leader process.
        MPI_Recv(proc_buffer, proc_buffer_len, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    // Floyd algorithm:
    int *kth_column = new int[num_verts];
    for (int k = 0; k < num_verts; k++) {
        int broadcaster_rank = get_broadcaster(col_bounds, k);
        if (world_rank == broadcaster_rank) {
            // Fill kth column from proc_buffer:
            for (int i = 0; i < num_verts; i++) {
                kth_column[i] = proc_buffer[i * proc_width + (k - proc_start_col)];
            }
        }
        // After bcast, all procs will have kth_column with same data.
        MPI_Bcast(kth_column, num_verts, MPI_INT, broadcaster_rank, MPI_COMM_WORLD);
        for (int i = 0; i < num_verts; i++) {
            for (int j = 0; j < proc_width; j++) {
                int dist_to_k = kth_column[i];
                int dist_from_k = proc_buffer[k * proc_width + j];
                if (dist_to_k == INT_MAX || dist_from_k == INT_MAX) {
                    // Infinite weight in one of the intermediate paths: not a real connection.
                    continue;
                }
                int new_dist = dist_to_k + dist_from_k;
                int old_dist = proc_buffer[i * proc_width + j];
                if (new_dist < old_dist) {
                    proc_buffer[i * proc_width + j] = new_dist;
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    delete[] kth_column;
    // Done Floyd algorithm.

    std::vector<std::vector<int>> combined;
    if (world_rank != 0) {
        // Send proc_buffer back to root.
        MPI_Send(proc_buffer, proc_buffer_len, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        // Recv proc_buffer, combine proc_buffers into `combined`.
        combined.resize(num_verts, std::vector<int>(num_verts));
        // Write proc 0 data into combined:
        int end_col_0 = col_bounds[1];
        for (int i = 0; i < num_verts; i++) {
            for (int j = 0; j < end_col_0; j++) {
                int weight = proc_buffer[i * end_col_0 + j];
                combined[i][j] = weight;
            }
        }

        // Make a buffer big enough to hold proc_buffer from any process:
        int *recv_buffer = new int[num_verts * proc_width_max];

        // Receive proc_buffer from all processes.
        for (int send_proc = 1; send_proc < world_size; send_proc++) {
            // How many ints to receive:
            int start_col = col_bounds[send_proc];
            int end_col = col_bounds[send_proc + 1];
            int width = end_col - start_col;
            int buffer_len = width * num_verts;

            MPI_Recv(recv_buffer, buffer_len, MPI_INT, send_proc, 
                0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Copy data from recv_buffer into `combined`:
            // Zigzag through the buffer:
            for (int i = 0; i < num_verts; i++) {
                for (int j = 0; j < width; j++) {
                    int weight = recv_buffer[i * width + j];
                    int j_combined = start_col + j;
                    combined[i][j_combined] = weight;
                }
            }

        }
        delete[] recv_buffer;
    }

    double proc_time = MPI_Wtime() - proc_start_time;

    MPI_Gather(&proc_time, 1, MPI_DOUBLE, proc_times, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Root prints all times and writes `combined` to file.
    if (world_rank == 0) {
        double root_stop_time = MPI_Wtime();
        std::cout << "Time taken (in seconds):\n" << std::setprecision(TIME_PRECISION);
        for (int proc_rank = 0; proc_rank < world_size; proc_rank++) {
            std::cout << proc_rank << ": "  << proc_times[proc_rank] << "\n";
        }
        std::cout << "Overall: " << root_stop_time - root_start_time << "\n";
        std::cout << "Max time: " << get_max(proc_times, (size_t)world_size) << "\n";
        graph_vector_to_file(combined, output_file_path);
        delete[] proc_times;
        delete graph;
    }
    delete[] proc_buffer;
    MPI_Finalize();
}
