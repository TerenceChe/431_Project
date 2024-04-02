#compiler setup
CXX = g++
CXXFLAGS = -std=c++14 -O3 -pthread $(MACRO)

.PHONY : clean

all: main test

main: shortest_path_floyd.cpp main.cpp
	$(CXX) $(CXXFLAGS) -DPRINT=1 main.cpp shortest_path_floyd.cpp -o main.out

test: shortest_path_floyd.cpp test.cpp
	$(CXX) $(CXXFLAGS) test.cpp shortest_path_floyd.cpp -o test.out

clean :
	rm -f *.o *.obj $(ALL)
