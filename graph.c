// graph.c ... Graph of strings (adjacency matrix)
// Written by John Shepherd, September 2015

// Adapted for use by z5260633 for COMP2521 Assignment 2 (August 2019)

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "graph.h"

#define strEQ(g, t) (strcmp ((g), (t)) == 0)

// Function signatures

static int vertexID (char *, char **, size_t);
static size_t addVertex (char *, char **, size_t);
static char *strdupImp (char *);

// newGraph()
// - create an initially empty Graph
Graph newGraph (size_t maxV)
{
	Graph new = malloc (sizeof *new);
	if (new == NULL)
		err (EX_OSERR, "couldn't allocate Graph");
	*new = (GraphRep){.nV = 0, .maxV = maxV};

	new->vertex = calloc (maxV, sizeof (char *));
	if (new->vertex == NULL)
		err (EX_OSERR, "calloc");
	new->edges = calloc (maxV, sizeof (Num *));
	if (new->edges == NULL)
		err (EX_OSERR, "calloc");

	for (size_t i = 0; i < maxV; i++) {
		new->edges[i] = calloc (maxV, sizeof (Num));
		if (new->edges[i] == NULL)
			err (EX_OSERR, "calloc");

		for (size_t j = 0; j < maxV; j++)
			new->edges[i][j] = 0;
	}

	return new;
}

// dropGraph(Graph)
// - clean up memory associated with Graph
void dropGraph (Graph g)
{
	if (g == NULL)
		return;

	for (size_t i = 0; i < g->nV; i++)
		free (g->vertex[i]);
	for (size_t i = 0; i < g->maxV; i++)
		free (g->edges[i]);

	free (g->vertex);
	free (g->edges);
	free (g);
}

// addEdge(Graph,Src,Dest)
// - add an edge from Src to Dest
// - returns 1 if edge successfully added
// - returns 0 if unable to add edge
//   (usually because nV exceeds maxV)
bool addEdge (Graph g, char *src, char *dest)
{
	assert (g != NULL);

	int v = vertexID (src, g->vertex, g->nV);
	if (v < 0) {
		if (g->nV >= g->maxV)
			return false;
		v = (int) addVertex (src, g->vertex, g->nV);
		g->nV++;
	}

	int w = vertexID (dest, g->vertex, g->nV);
	if (w < 0) {
		if (g->nV >= g->maxV)
			return false;
		w = (int) addVertex (dest, g->vertex, g->nV);
		g->nV++;
	}
	g->edges[v][w] = 1;

	return true;
}

// isConnected(Graph,Src,Dest)
// - check whether there is an edge from Src->Dest
bool isConnected (Graph g, char *src, char *dest)
{
	assert (g != NULL);
	int v = vertexID (src, g->vertex, g->nV);
	int w = vertexID (dest, g->vertex, g->nV);
	if (v < 0 || w < 0)
		return 0;
	return g->edges[v][w] == 1;
}

// nVertices(Graph)
// - return # vertices currently in Graph
size_t nVertices (Graph g)
{
	assert (g != NULL);
	return g->nV;
}

char *vertexName (Graph g, size_t i) {
	char *name = g->vertex[i];
	return name;
}

// showGraph(Graph)
// - display Graph
void showGraph (Graph g, int mode)
{
	assert (g != NULL);
	if (g->nV == 0) {
		puts ("Graph is empty");
		return;
	}

	printf ("Graph has %zu vertices:\n", g->nV);
	if (mode == 1) {
		for (size_t i = 0; i < g->nV; i++) {
			for (size_t j = 0; j < g->nV; j++)
				printf ("%d", g->edges[i][j]);
			putchar ('\n');
		}
	} else {
		for (size_t i = 0; i < g->nV; i++) {
			printf ("Vertex: %s\n", g->vertex[i]);
			printf ("connects to\n");
			for (size_t j = 0; j < g->nV; j++) {
				if (g->edges[i][j])
					printf ("   %s\n", g->vertex[j]);
			}
		}
	}
}

// inlinks(Graph, word)
// - returns how many links going into the node there are
size_t inlinks (Graph g, char *nodeName) {
	size_t sum = 0;
	int j = vertexID(nodeName, g->vertex, nVertices(g));
	for (int i = 0; i < nVertices(g); i++) {
		if (g->edges[i][j]) sum++;
	}

	return sum;
}

// outlinks(Graph, word)
// - returns how many links going out of the node there are
size_t outlinks (Graph g, char *nodeName) {
	size_t sum = 0;
	int i = vertexID(nodeName, g->vertex, nVertices(g));
	for (int j = 0; j < nVertices(g); j++) {
		if (g->edges[i][j]) sum++;
	}

	return sum;
}

// Helper functions

// vertexID(Str,Names,N)
// - searches for Str in array of Names[N]
// - returns index of Str if found, -1 if not
static int vertexID (char *str, char **names, size_t N)
{
	for (size_t i = 0; i < N; i++)
		if (strEQ (str, names[i]))
			return (int) i;
	return -1;
}

// addVertex(Str,Names,N) 
// - add Str at end of Names
static size_t addVertex (char *str, char **names, size_t N)
{
	names[N] = strdupImp (str);
	return N;
}

// Implementation of strdup
static char *strdupImp (char *original) {
    char *duplicate = malloc(strlen (original) + 1);  
    if (!duplicate) return NULL;          
    
    strcpy(duplicate, original);                 
    return duplicate;                            
}