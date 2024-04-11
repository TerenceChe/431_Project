CXXFLAGS = -std=c++14 -O3 -pthread -Wall -g 

.PHONY : clean

all: floyd_serial_threaded test_threaded floyd_distrib print_shortest_path

# Executable for running both serial and threaded floyd.
floyd_serial_threaded: floyd_serial_threaded.cpp driver_serial_threaded.cpp utils.cpp graph.cpp
	g++ $(CXXFLAGS) \
	driver_serial_threaded.cpp utils.cpp graph.cpp floyd_serial_threaded.cpp -o floyd_serial_threaded

# Executable for testing that the threaded floyd returns same
# results as serial floyd.
test_threaded: floyd_serial_threaded.cpp test_threaded.cpp utils.cpp graph.cpp
	g++ $(CXXFLAGS) utils.cpp graph.cpp test_threaded.cpp floyd_serial_threaded.cpp -o test_threaded

# Executable for running distribtued (MPI) version of floyd.
floyd_distrib: floyd_distrib.cpp driver_distrib.cpp utils.cpp graph.cpp
	mpic++ $(CXXFLAGS) \
	driver_distrib.cpp utils.cpp graph.cpp floyd_distrib.cpp -o floyd_distrib

print_shortest_path: print_shortest_path.cpp graph.cpp utils.cpp
	g++ $(CXXFLAGS) print_shortest_path.cpp graph.cpp utils.cpp -o print_shortest_path

clean :
	rm floyd_serial_threaded test_threaded floyd_distrib output_graph/*
