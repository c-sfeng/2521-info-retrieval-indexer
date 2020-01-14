// graph.h ... Interface to Graph of strings
// Written by John Shepherd, September 2015

// Adapted for use by z5260633 for COMP2521 Assignment 2 (August 2019)

#include <stdbool.h>
#include <stdlib.h>

#ifndef GRAPH_H
#define GRAPH_H

typedef struct GraphRep *Graph;

typedef unsigned char Num;

typedef struct GraphRep {
	size_t nV;
	size_t maxV;
	char **vertex;
	Num **edges;
} GraphRep;

// Function signatures

Graph newGraph (size_t);
void dropGraph (Graph);
bool addEdge (Graph, char *, char *);
bool isConnected (Graph, char *, char *);
size_t nVertices (Graph);
char *vertexName (Graph, size_t);
void showGraph (Graph, int);
size_t inlinks (Graph, char *);
size_t outlinks (Graph, char *);

#endif
