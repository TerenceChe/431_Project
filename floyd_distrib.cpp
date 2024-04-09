// Process 0 is leader process and reads input file into matrix.
// Then it sends submatrices (by column spits) to follower processes.
// Follower processes update distances in submatrices using floyd algorithm.
// Then follower processes send submatrices back to leader process.
// The leader process combines the submatrices into a matrix and writes to output file.

#include <iostream>
#include <vector>
#include <climits>
#include <mpi.h>
#include "graph.h"
#include "utils.h"
#include "floyd_distrib.h"

void distrib(std::string input_file_path, std::string output_file_path) {
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    int num_verts = 0;
    Graph *graph = NULL;

    if (world_rank == 0) {
        graph = new Graph();
        graph->readGraphFromFile(input_file_path);
        num_verts = graph->getNumVerts();
    }
    
    MPI_Bcast(&num_verts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    std::vector<int> col_bounds = get_col_bounds(num_verts, world_size);

    int proc_start_col = col_bounds[world_rank];
    int proc_end_col = col_bounds[world_rank + 1];
    int proc_width = proc_end_col - proc_start_col;
    int proc_buffer_len = proc_width * num_verts;
    // Goal is to split graph and each proc (including root) has a bunch of data in here.
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

        // Send some of the graph to all of the other processes:
        for (int other_proc = 1; other_proc < world_size; other_proc++) {
            int other_start_col = col_bounds[other_proc];
            int other_end_col = col_bounds[other_proc + 1];
            int other_buffer_len = (other_end_col - other_start_col) * num_verts;

            int *other_buffer = new int[other_buffer_len]; // Submatrix to send to recv_proc.
            int other_buffer_i = 0;
            for (int i = 0; i < num_verts; i++) {
                for (int j = other_start_col; j < other_end_col; j++) {
                    int weight = graph->getWeight(i, j);
                    other_buffer[other_buffer_i] = weight;
                    other_buffer_i++;
                }
            }
            MPI_Send(other_buffer, other_buffer_len, MPI_INT, other_proc, 0, MPI_COMM_WORLD);
            delete[] other_buffer;
        }
    } else {
        // Receive graph section from leader process.
        MPI_Recv(proc_buffer, proc_buffer_len, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    // Floyd algorithm:
    int *kth_column = NULL;
    for (int k = 0; k < num_verts; k++) {
        // Check if kth column is in our proc_buffer:
        if (k >= proc_start_col &&  k < proc_end_col) {
            kth_column = new int[num_verts];
            // Fill kth column from proc_buffer:
            for (int i = 0; i < num_verts; i++) {
                kth_column[i] = proc_buffer[i * proc_width + (k - proc_start_col)];
            }
        }
        // After bcast, all procs will have kth_column with same data.
        MPI_Bcast(kth_column, num_verts, MPI_INT, world_rank, MPI_COMM_WORLD);
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
        if (k >= proc_start_col && k < proc_end_col) {
            delete[] kth_column;
        }
    }
    
    std::vector<std::vector<int>> combined;
    if (world_rank != 0) {
        // Done Floyd, send proc_buffer back to root.
        MPI_Send(proc_buffer, proc_buffer_len, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        // Done Floyd, recv proc_buffer, combine proc_buffers into `combined`.
        combined.resize(num_verts, std::vector<int>(num_verts));
        // Write proc 0 data into combined:
        int start_col = col_bounds[0];
        int end_col = col_bounds[1];
        int width = end_col - start_col;
        for (int i = 0; i < num_verts; i++) {
            for (int j = 0; j < width; j++) {
                int weight = proc_buffer[i * width + j];
                int j_combined = start_col + j;
                combined[i][j_combined] = weight;
            }
        }

        // Write all other proc data into `combined`:
        for (int send_proc = 1; send_proc < world_size; send_proc++) {
            // How many ints to receive:
            int start_col = col_bounds[send_proc];
            int end_col = col_bounds[send_proc + 1];
            int buffer_len = (end_col - start_col) * num_verts;

            int *recv_buffer = new int[buffer_len];
            MPI_Recv(recv_buffer, buffer_len, MPI_INT, send_proc, 
                0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Copy data from recv_buffer into `combined`:
            // Zigzag through the buffer:
            int recv_buffer_width = end_col - start_col;
            for (int i = 0; i < num_verts; i++) {
                for (int j = 0; j < recv_buffer_width; j++) {
                    int weight = recv_buffer[i * recv_buffer_width + j];
                    int j_combined = start_col + j;
                    combined[i][j_combined] = weight;
                }
            }

            delete[] recv_buffer;
        }
    }

    // Root writes `combined` to file.
    if (world_rank == 0) {
        graph_vector_to_file(combined, output_file_path);
        delete graph;
    }
    delete[] proc_buffer;
    MPI_Finalize();
}
