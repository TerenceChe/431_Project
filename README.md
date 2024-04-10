# CMPT431 Project: All-Pairs Shortest Path

Group: 31

Members: 
* Ben Smus
* Terence Cheung
* Lily Yang

## Input Graph Data
To test the system, you would first need to generate the data you want and the expectation is that the graph is going to be weighted directed/undirected graph. If there's no path from A -> B, then you should represent the weight as `i` which means infinity. 

For simplicity, we've created a python script called random_graph.py which will allow you to create a graph of any size to test the program with.

Here's the command to generate input graphs of various sizes:

```
python3 input_graph/random_graph.py 100 ./input_graph/random_graph_100.txt
```

We've already generated input graph sizes of 50, 100, 200 ... 3000 for our experiments so you can use the prebuilt sample graphs when running the program or you can generate your own using the command above. 

## Running the program 

We assume you will be running these commands at the root of your directory, if you are not, please make sure you are.

```
# this is needed so that output files can write to this directory
mkdir -p output_graph

make all

# running the serial version using random_graph_1000.txt

./floyd_serial_threaded --mode 0 --inputFile=./input_graph/random_graph_1000.txt --outputFile=./output_graph/random_graph_serial_1000.txt

# running the parallel version using random_graph_1000.txt with 4 threads

./floyd_serial_threaded --np 4 --mode 1 --inputFile=./input_graph/random_graph_1000.txt --outputFile=./output_graph/random_graph_parallel_1000.txt

# running the distributed version of the program, you will need to submit the slurm script we provided
# we have set up the slurm script to run on 4 processes, and using the same input data of 1000 nodes

sbatch project_slurm.script

```

## Verifying correctness of the program

You must first complete the section `Running the program` so you can get the output file with the resulting matrix of all pairs shortest paths

```
diff -w ./output_graph/random_graph_serial_1000.txt ./output_graph/random_graph_parallel_1000.txt

diff -w ./output_graph/random_graph_serial_1000.txt ./output_graph/random_graph_distrib_1000.txt

# there shouldn't be any diffs when you run the 2 commands above which shows to you that the programs are generating correct results
```

