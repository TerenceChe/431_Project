# 431_Project
Parallel all pairs shortest path algorithm

# Input Graph Data
* to test the system, you would need to first generate the data you want and the expectation is that the graph is going to be weighted directed/undirected graph. If there's no path from A -> B, then you should represent the weight as `i` which means infinity. 

## Running it
```
make all
./floyd_serial_threaded --inputFile=./input_graph/random_graph.txt --outputFile=./output_graph/random_graph_serial_threaded.txt
./floyd_distrib --inputFile=./input_graph/random_graph.txt --outputFile=./output_graph/random_graph_distrib_.txt
```
