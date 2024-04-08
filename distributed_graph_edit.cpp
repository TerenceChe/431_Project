// !! Not part of Floyd-Warshall implementation !!

// Demo: how to parallelize matrix file editing using a distributed system in MPI.

// Process 0 is leader process and reads input file into matrix.
// Then it sends submatrices (by column spits) to follower processes.
// Follower processes apply some elementwise operation to the submatrices.
// Then follower processes send submatrices back to leader process.
// The leader process combines the submatrices into a matrix and writes to output file.

#include <iostream>
#include <vector>
#include <mpi.h>
#include "graph.h"
#include "utils.h"

#define INPUT_FILE "input_graph/test_large_data.txt"
#define OUTPUT_FILE "output_graph/combined.txt"

int main() {
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    int num_verts = 0;
    Graph *graph = NULL;

    if (world_rank == 0) {
        graph = new Graph();
        graph->readGraphFromFile(INPUT_FILE);
        num_verts = graph->getNumVerts();
    }
    
    MPI_Bcast(&num_verts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    std::vector<int> col_bounds = get_col_bounds(num_verts, world_size);

    int start_col = col_bounds[world_rank];
    int end_col = col_bounds[world_rank + 1];
    int buffer_len = (end_col - start_col) * num_verts;
    // Goal is to split graph and each proc (including root) has a bunch of data in here.
    int *proc_buffer = new int[buffer_len]; 

    if (world_rank == 0) {
        // Copy from graph to proc_buffer:
        int proc_buffer_i = 0;
        for (int i = 0; i < num_verts; i++) {
            for (int j = start_col; j < end_col; j++) {
                int weight = graph->getWeight(i, j);
                proc_buffer[proc_buffer_i] = weight;
                proc_buffer_i++;
            }
        }

        for (int recv_proc = 1; recv_proc < world_size; recv_proc++) {
            // Get number of integers in submatrix:
            int start_col = col_bounds[recv_proc];
            int end_col = col_bounds[recv_proc + 1];
            int buffer_len = (end_col - start_col) * num_verts;

            int *send_buffer = new int[buffer_len]; // Submatrix to send to recv_proc.
            int send_buffer_i = 0;
            for (int i = 0; i < num_verts; i++) {
                for (int j = start_col; j < end_col; j++) {
                    int weight = graph->getWeight(i, j);
                    send_buffer[send_buffer_i] = weight;
                    send_buffer_i++;
                }
            }
            MPI_Send(send_buffer, buffer_len, MPI_INT, recv_proc, 0, MPI_COMM_WORLD);
            delete[] send_buffer;
        }
    } else {
        MPI_Recv(proc_buffer, buffer_len, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    // Verify contents of proc_buffer:
    // std::printf("Proc %d:\n", world_rank);
    // for (int i = 0; i < buffer_len; i++) {
    //     std::printf("%d ", proc_buffer[i]);
    // }
    // std::printf("\n");

    // Perform operation (for now do no operation to check if 
    // able to get back original) on proc_buffer elements.
    
    // Send proc_buffer back to root, and root combines proc_buffers into `combined`.
    std::vector<std::vector<int>> combined;
    if (world_rank != 0) {
        MPI_Send(proc_buffer, buffer_len, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else { // Proc 0.
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

        // Write all other proc data into combined:
        for (int send_proc = 1; send_proc < world_size; send_proc++) {
            // How many ints to receive:
            int start_col = col_bounds[send_proc];
            int end_col = col_bounds[send_proc + 1];
            int buffer_len = (end_col - start_col) * num_verts;

            int *recv_buffer = new int[buffer_len];
            MPI_Recv(recv_buffer, buffer_len, MPI_INT, send_proc, 
                0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Copy data from recv_buffer into combined:
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

    // Root writes combined to file.
    if (world_rank == 0) {
        graph_vector_to_file(combined, OUTPUT_FILE);
        delete graph;
    }
    delete[] proc_buffer;
    MPI_Finalize();
    return 0;
}
