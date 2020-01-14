// set.c ... simple, inefficient Set of Strings
// Written by John Shepherd, September 2015

// Adapted for use by z5260633 for COMP2521 Assignment 2 (August 2019)

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "set.h"

#define strEQ(s, t) (strcmp ((s), (t)) == 0)
#define strLT(s, t) (strcmp ((s), (t)) < 0)

// Function signatures

static Link newNode (char *);
static void dropNode (Link);
static int findNode (Link, char *, Link *, Link *);
static void findNodePR (Link, Link, Link *, Link *);
static char *strdupImp (char *);

// newSet()
// - create an initially empty Set
Set newSet (void)
{
	Set new = malloc (sizeof *new);
	if (new == NULL)
		err (EX_OSERR, "couldn't allocate Set");
	*new = (SetRep){.nelems = 0, .elems = NULL, .next = NULL};
	return new;
}

// dropSet(Set)
// - clean up memory associated with Set
void dropSet (Set s)
{
	if (s == NULL)
		return;
	Link curr = s->elems;
	while (curr != NULL) {
		Link next = curr->next;
		dropNode (curr);
		curr = next;
	}

	free (s);
}

// insertInto(Set,Str)
// - ensure that Str is in Set
void insertInto (Set s, char *str)
{
	assert (s != NULL);
	Link curr, prev;
	int found = findNode (s->elems, str, &curr, &prev);
	if (found)
		return; // already in Set

	Link new = newNode (str);
	s->nelems++;
	if (prev == NULL) {
		// add at start of list of elems
		new->next = s->elems;
		s->elems = new;
	} else {
		// add into list of elems
		new->next = prev->next;
		prev->next = new;
	}
}

// insertInOrder(Set, Link)
// - ensure that the new Link is inserted into s in order of PR
void insertInOrder (Set s, Link new) {
	assert (s != NULL);
	Link curr, prev;
	findNodePR (s->elems, new, &curr, &prev);
	
	Link newOrdered = newNode (new->val);
	s->nelems++;
	if (prev == NULL) {
		// add at start of list of elems
		newOrdered->next = s->elems;
		s->elems = newOrdered;
	} else {
		// add into list of elems
		newOrdered->next = prev->next;
		prev->next = newOrdered;
	}

	assignPR(s, new->val, new->pr);
}

// insertNoSort(Set,Char,temp,pr)
// - inserts a node (with given properties) into s
void insertNoSort (Set s, char *str, double temp, double pr) {
	assert (s != NULL);
	Link new = newNode(str);
	new->temp = temp;
	new->pr = pr;
	//printf("%f\n", pr);

	s->nelems++;
	
	Link curr = s->elems;

	// first element
	if (curr == NULL) {
		s->elems = new;
		return;
	// general case
	} else {
		while (curr->next != NULL) curr = curr->next;
		if (curr->next == NULL) curr->next = new;
	}
}

// dropFrom(Set,Str)
// - ensure that Str is not in Set
void dropFrom (Set s, char *str)
{
	assert (s != NULL);
	Link curr, prev;
	int found = findNode (s->elems, str, &curr, &prev);
	if (!found)
		return;
	s->nelems--;
	if (prev == NULL)
		s->elems = curr->next;
	else
		prev->next = curr->next;
	dropNode (curr);
}

// isElem(Set,Str)
// - check whether Str is contained in Set
int isElem (Set s, char *str)
{
	assert (s != NULL);
	Link curr, prev;
	return findNode (s->elems, str, &curr, &prev);
}

// assignPR(Set,Str,Double)
// - assigns the given pr to the given list node
void assignPR (Set s, char *str, double pr) {
	assert(s);
	if (isElem(s, str) != 1) return;
	for (Link curr = s->elems; curr != NULL; curr = curr->next) {
		if strEQ(str, curr->val) curr->pr = pr;
	}
}

// assignPrevPR(Set,Str,Double)
// - assigns the given pr to the given list node as the previous
void assignPrevPR (Set s, char *str, double pr) {
	assert(s);
	if (isElem(s, str) != 1) return;
	for (Link curr = s->elems; curr != NULL; curr = curr->next) {
		if strEQ(str, curr->val) curr->temp = pr;
	}
}

// nElems(Set)
// - return # elements in Set
int nElems (Set s)
{
	assert (s != NULL);
	return s->nelems;
}

// showSet(Set)
// - display Set (for debugging)
void showSet (Set s)
{
	Link curr;
	if (s->nelems == 0)
		printf ("Set is empty\n");
	else {
		printf ("Set has %d elements:\n", s->nelems);
		int id = 0;
		curr = s->elems;
		while (curr != NULL) {
			printf ("[%03d] %s\n", id, curr->val);
			id++;
			curr = curr->next;
		}
	}
}

// Helper functions

static Link newNode (char *str)
{
	Link new = malloc (sizeof (Node));
	assert (new != NULL);
	new->val = strdupImp (str);
	new->pr = 0;
	new->temp = 0;
	new->next = NULL;
	return new;
}

static void dropNode (Link curr)
{
	assert (curr != NULL);
	free (curr->val);
	free (curr);
}

// findNode(L,Str)
// - finds where Str could be added into L
// - if already in L, curr->val == Str
// - if not already in L, curr and prev indicate where to insert
// - return value indicates whether Str found or not
static int findNode (Link list, char *str, Link *cur, Link *pre)
{
	Link curr = list, prev = NULL;
	while (curr != NULL && strLT (curr->val, str)) {
		prev = curr;
		curr = curr->next;
	}
	*cur = curr;
	*pre = prev;
	return (curr != NULL && strEQ (str, curr->val));
}

static void findNodePR (Link list, Link new, Link *cur, Link *pre)
{
	double newPR = new->pr;
	Link curr = list, prev = NULL;
	while (curr != NULL && curr->pr > newPR) {
		prev = curr;
		curr = curr->next;
	}
	*cur = curr;
	*pre = prev;
	return;
}

// Implementation of strdup
static char *strdupImp (char *original) {
    char *duplicate = malloc(strlen (original) + 1);  
    if (!duplicate) return NULL;          
    
    strcpy(duplicate, original);                 
    return duplicate;                            
}
