CXXFLAGS = -std=c++14 -O3 -pthread -Wall -g

.PHONY : clean

all: floyd_serial_threaded test_threaded floyd_distributed distributed_graph_edit

# Executable for running both serial and threaded floyd.
floyd_serial_threaded: shortest_path_floyd.cpp driver_serial_threaded.cpp utils.cpp graph.cpp
	g++ $(CXXFLAGS) -DPRINT=1 \
	driver_serial_threaded.cpp utils.cpp graph.cpp shortest_path_floyd.cpp -o floyd_serial_threaded

# Executable for testing that the threaded floyd returns same
# results as serial floyd.
test_threaded: shortest_path_floyd.cpp test_threaded.cpp utils.cpp graph.cpp
	g++ $(CXXFLAGS) utils.cpp graph.cpp test_threaded.cpp shortest_path_floyd.cpp -o test_threaded

# Executable for running distribtued (MPI) version of floyd.
floyd_distributed: shortest_path_floyd.cpp driver_distributed.cpp utils.cpp graph.cpp
	mpic++ $(CXXFLAGS) \
	driver_distributed.cpp utils.cpp graph.cpp shortest_path_floyd.cpp -o floyd_distributed

distributed_graph_edit: distributed_graph_edit.cpp utils.cpp graph.cpp
	mpic++ $(CXXFLAGS) utils.cpp graph.cpp distributed_graph_edit.cpp -o distributed_graph_edit

clean :
	rm floyd_serial_threaded test_threaded floyd_distributed distributed_graph_edit
