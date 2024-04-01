#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include <cmath>
#include <iostream>
#include <map>
#include <thread>
#include "core/utils.h"
#include "core/graph.h"

void threaded(Graph *g, uint np);
void serial(Graph* g);

#endif
