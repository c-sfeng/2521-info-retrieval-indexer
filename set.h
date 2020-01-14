// set.h ... interface to simple Set of Strings
// Written by John Shepherd, September 2015

// Adapted for use by z5260633 for COMP2521 Assignment 2 (August 2019)

#ifndef SET_H
#define SET_H

typedef struct Node *Link;

typedef struct Node {
	char *val;
	double pr;
	double temp;
	Link next;
} Node;

typedef struct SetRep *Set;

typedef struct SetRep {
	int nelems;
	Link elems;
	Set next;
} SetRep;

// Function signatures

Set newSet (void);
void dropSet (Set);
void insertInto (Set, char *);
void insertInOrder (Set, Link);
void insertNoSort (Set, char *, double, double);
void dropFrom (Set, char *);
int isElem (Set, char *);
void assignPR (Set, char *, double);
void assignPrevPR (Set, char *, double);
int nElems (Set);
void showSet (Set);

#endif
