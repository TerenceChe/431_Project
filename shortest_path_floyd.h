#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "core/graph.h"

void threaded(Graph *g, uint np);
void serial(Graph* g);
void distrib(Graph *g);

#endif
