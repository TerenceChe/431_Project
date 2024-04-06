CXXFLAGS = -std=c++14 -O3 -pthread -Wall -g

.PHONY : clean

all: serial_threaded test_threaded distributed

# Executable for running both serial and threaded floyd.
serial_threaded: shortest_path_floyd.cpp driver_serial_threaded.cpp
	g++ $(CXXFLAGS) -DPRINT=1 \
	driver_serial_threaded.cpp shortest_path_floyd.cpp -o serial_threaded

# Executable for testing that the threaded floyd returns same
# results as serial floyd.
test_threaded: shortest_path_floyd.cpp test_threaded.cpp
	g++ $(CXXFLAGS) test_threaded.cpp shortest_path_floyd.cpp -o test_threaded

# Executable for running distribtued (MPI) version of floyd.
distributed: shortest_path_floyd.cpp driver_distributed.cpp
	mpic++ $(CXXFLAGS) \
	driver_distributed.cpp shortest_path_floyd.cpp -o distributed

clean :
	rm serial_threaded test_threaded distribtued
