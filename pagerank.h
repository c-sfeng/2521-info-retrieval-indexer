#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "graph.h"
#include "set.h"
#include "readData.h"

#ifndef PAGERANK_H
#define PAGERANK_H

// Function signatures

Set calculatePageRank (Graph g, Set s, double d, double diffPR, int maxIterations);

#endif
