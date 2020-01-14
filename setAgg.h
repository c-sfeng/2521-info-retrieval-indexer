// setAgg.h ... interface to simple Set of Strings
// Written by John Shepherd, September 2015

// Adapted for use by z5260633 for COMP2521 Assignment 2 (August 2019)

#include "set.h"

#ifndef SETAGG_H
#define SETAGG_H

typedef struct SetAggRep {
	int nSubsets;
	int *Parr;
	double sumSF;
	Set subsetHead;
} SetAggRep;

typedef struct SetAggRep *SetAgg;

// Function signatures

SetAgg newSetAgg (void);
void dropSetAgg (SetAgg);
void insertSetAggNoSort (SetAgg, Set);
int nSubsets (SetAgg);
void showSetAgg (SetAgg);
Set SetCopy (Set s);

#endif
