#compiler setup
CXX = g++
CXXFLAGS = -std=c++14 -O3 -pthread $(MACRO)

COMMON= core/utils.h core/cxxopts.h core/get_time.h core/graph.h
PARALLEL= shortest_path_floyd
ALL= $(PARALLEL)


all : $(ALL)

% : %.cpp $(COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

test: shortest_path_floyd.cpp shortest_path_floyd.h test.cpp
	$(CXX) $(CXXFLAGS) test.cpp shortest_path_floyd.cpp -o test
clean :
	rm -f *.o *.obj $(ALL)