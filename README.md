# 431_Project
Parallel all pairs shortest path algorithm

# Input Graph Data
* to test the system, you would need to first generate the data you want and the expectation is that the graph is going to be weighted directed/undirected graph. If there's no path from A -> B, then you should represent the weight as `i` which means infinity. 

# Running locally using Docker (DRAFT - NOT READY)

* to run the program locally
* first build the docker image which includes all the required tools
* pre-requisit - you must have docker installed, if you don't, then you just have to install the tools locally

```
cd project
docker build -t my_mpi_image .

# mounts the current folder so you can make the changes and test it
docker run --rm -it -v .:/project my_mpi_image

```

* once you are inside the container

```
mpirun -np 8 ./curve_area_parallel 
```

## Running locally without Docker

mode 0 = serial
mode 1 = using threads
mode 2 = using MPI

```
make all
./shortest_path_floyd --np 4 --mode 0  --inputFile /Users/lyang/Document/personal/school/cmpt431/431_Project/input_graph/test_data_1.txt

```