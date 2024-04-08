// !! Not part of Floyd-Warshall implementation !!

// Demo: how to parallelize matrix file editing using a distributed system in MPI.

// Process 0 is leader process and reads input file into matrix.
// Then it sends submatrices (by column spits) to follower processes.
// Follower processes apply some elementwise operation to the submatrices.
// Then follower processes send submatrices back to leader process.
// The leader process combines the submatrices into a matrix and writes to output file.

#include <iostream>
#include <mpi.h>
#include "core/graph.h"
#include "core/utils.h"

#define INPUT_FILE "input_graph/test_large_data.txt"

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
    
    std::printf("Proc %d:\n", world_rank);
    for (int i = 0; i < buffer_len; i++) {
        std::printf("%d ", proc_buffer[i]);
    }
    std::printf("\n");

    // TODO: 
    // 1. Perform operation (for now do no operation to check if 
    // able to get back original) on proc_buffer elements.
    // 2. Send proc_buffer back to root, and root combines proc_buffers into one vector.
    // 3. Root initializes Graph from that vector and writes to file.

    delete[] proc_buffer;

    MPI_Finalize();
    return 0;
}
