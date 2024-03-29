# 431_Project
Parallel all pairs shortest path algorithm

# Input Graph Data
* to test the system, you would need to first generate the data you want and the expectation is that the graph is going to be weighted directed graph. For nodes that have no path from A -> B, the input file should represent the weight using a negative value means infinity. 

# Running locally

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