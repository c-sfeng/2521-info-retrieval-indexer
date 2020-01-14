
// =============================================================================
// pagerank.c 
// =============================================================================
// Outputs a list of urls sorted by a weighted PageRank algorithm to
// pagerankList.txt
// =============================================================================
// Written by z5260633 for COMP2521 Assignment 2 (August 2019)
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "graph.h"
#include "set.h"
#include "readData.h"

#define BUFSIZE 	    1024

static Set calculatePageRank (Graph, Set, double, double, int);
static Set orderPR (Set);
static void calculatePRWeight (Graph, Set, double, double, int);
static double Win (Graph, Set, char *, char *);
static double Wout (Graph, Set, char *, char *);

int main (int argc, char *argv[]) {
	if (argc != 4) {
		printf("Usage: ./pagerank dampingFactor differencePR maxIterations\n");
		return 0;
	}

	double d = atof(argv[1]);
	double diffPR = atof(argv[2]);
	int maxIter = atoi(argv[3]);

    Set s = GetCollection();
	if (s == NULL) return 0;

	Graph g = GetGraph(s);
	if (g == NULL) return 0;

	Set orderedS = calculatePageRank(g, s, d, diffPR, maxIter);

    // output urlPR to "pagerankList.txt"
	FILE *fp = fopen("pagerankList.txt", "w");
	if (fp == NULL) {
        printf("Invalid file: pagerankList.txt\n");
        return 0;
    }

	// writing to fp
	for (Link curr = orderedS->elems; curr != NULL; curr = curr->next) {
		fprintf(fp, "%s, %ld, %.7f\n", curr->val, 
				outlinks(g, curr->val), curr->pr);
	}

	assert(!fclose(fp));

	dropGraph(g);
	dropSet(s);

    return 0;
}

// calculates and orders the pages by PR
static Set calculatePageRank (Graph g, Set s, double d, 
							  double diffPR, int maxIters) {
    calculatePRWeight(g, s, d, diffPR, maxIters);

    Set orderedCollection = orderPR(s);

    return orderedCollection;
}

// orders the given collection by order of PR
static Set orderPR (Set collection) {
	Set orderedCollection = newSet();
	for (Link curr = collection->elems; curr != NULL; curr = curr->next)
		insertInOrder(orderedCollection, curr);

    return orderedCollection;
}

// calculates the PRWeight for all nodes in a graph
static void calculatePRWeight (Graph g, Set s, double d, double dPR, int max) {
	size_t n = nElems(s);

	for (Link curr = s->elems; curr != NULL; curr = curr->next)
		assignPrevPR(s, curr->val, 1 / n);
	
    double currPR = 0;

	int iteration = 0;
	double diff = dPR;
	double diffTemp = 0;
	
	while (iteration < max && diff >= dPR) {
		for (Link curr = s->elems; curr != NULL; curr = curr->next) {
			currPR = (1 - d) / n;
			double outSum = 0;
			for (Link out = s->elems; out != NULL; out = out->next) {
				// if out is connected to curr, calculate
				if (isConnected(g, out->val, curr->val)) {
					outSum += out->temp * Win(g, s, out->val, curr->val) * 
					Wout(g, s, out->val, curr->val);
				}
			}
			currPR += (d * outSum);
			assignPR(s, curr->val, currPR);

			if (curr->pr < curr->temp) diffTemp += (curr->temp - curr->pr);
			else diffTemp += (curr->pr - curr->temp);
		}
		
		// assign the currently found PR to the PR of the previous iteration
		for (Link curr = s->elems; curr != NULL; curr = curr->next)
			assignPrevPR(s, curr->val, curr->pr);
		
		diff = diffTemp;
		diffTemp = 0;
		iteration++;
	}

    return;
}


// returns the Win value as provided by assignment spec
static double Win (Graph g, Set s, char *Pj, char *Pi) {
	double Win = 0;
	double denom = 0;
	Win = inlinks(g, Pi);

	for (Link curr = s->elems; curr != NULL; curr = curr->next) 
		if (isConnected(g, Pj, curr->val)) denom += inlinks(g, curr->val);
	
	Win /= denom;

	return Win;
}

// returns the Wout value as provided by assignment spec
static double Wout (Graph g, Set s, char *Pj, char *Pi) {
	double Wout = 0;
	double denom = 0;
	Wout = outlinks(g, Pi);
	if (Wout == 0) Wout = 0.5;
	
	for (Link curr = s->elems; curr != NULL; curr = curr->next) {
		if (isConnected(g, Pj, curr->val)) {
			if (outlinks(g, curr->val) == 0) denom += 0.5;
			else denom += outlinks(g, curr->val);
		}
	}
	
	Wout /= denom;

	return Wout;
}