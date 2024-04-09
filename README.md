# 431_Project
Parallel all pairs shortest path algorithm

# Input Graph Data
* to test the system, you would need to first generate the data you want and the expectation is that the graph is going to be weighted directed/undirected graph. If there's no path from A -> B, then you should represent the weight as `i` which means infinity. 

For simplicity, we've created a python script called random_graph.py which will allow you to create a graph of any size to test the program with, to generate input graphs, here's the command

```
python3 input_graph/random_graph.py 100 ./input_graph/random_graph_100.txt

```

## Running it
```
make all

# running the serial version using random_graph_1000.txt
./floyd_serial_threaded --mode 0 --inputFile=./input_graph/random_graph.txt --outputFile=./output_graph/random_graph_serial_threaded.txt

# running the parallel version using random_graph_1000.txt with 4 threads
./floyd_serial_threaded --np 4 --mode 1 --inputFile=./input_graph/random_graph.txt --outputFile=./output_graph/random_graph_serial_threaded.txt

```

