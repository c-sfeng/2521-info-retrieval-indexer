// setAgg.c ... simple, inefficient Set of Strings
// Written by John Shepherd, September 2015

// Adapted for use by z5260633 for COMP2521 Assignment 2 (August 2019)

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "setAgg.h"
#include "set.h"

// newSetAgg()
// - create an initially empty Set
SetAgg newSetAgg (void)
{
	SetAgg new = malloc (sizeof *new);
	if (new == NULL)
		err (EX_OSERR, "couldn't allocate SetAgg");
	*new = (SetAggRep){.nSubsets = 0,
		               .sumSF = 0, .subsetHead = NULL};
	return new;
}

// dropSetAgg(SetAgg)
// - clean up all memory associated with SetAgg
void dropSetAgg (SetAgg sa)
{
	if (sa == NULL)
		return;
	Set curr = sa->subsetHead;
	while (curr != NULL) {
		Set next = curr->next;
		dropSet (curr);
		curr = next;
	}

	free (sa);
}

// insertSetAggNoSort(SetAgg,Set)
// - inserts Set into SetAgg with no alphabetical or PR order
void insertSetAggNoSort (SetAgg sa, Set s) {
	assert (sa != NULL);
    assert (s != NULL);
		 
    Set new = SetCopy(s);
	sa->nSubsets++;
	Set curr = sa->subsetHead;

	// if first
	if (curr == NULL) {
		sa->subsetHead = new;
		return;
	// general case
	} else {
		while (curr->next != NULL) curr = curr->next;
		if (curr->next == NULL) curr->next = new;
	}
}

// nSubsets(SetAgg)
// - return # elements in Set
int nSubsets (SetAgg sa)
{
	assert (sa != NULL);
	return sa->nSubsets;
}

// showSet(Set)
// - display Set (for debugging)
void showSetAgg (SetAgg sa)
{
	if (sa->nSubsets == 0)
		printf ("SetAgg is empty\n");
	else {
		printf ("SetAgg has %d elements:\n", sa->nSubsets);
		int id = 0;
		Set curr = sa->subsetHead;
		while (curr != NULL) {
			printf ("[%05d]\n", id);
			id++;
            showSet(curr);
			curr = curr->next;
		}
	}
}

// SetCopy(Set)
// - copies s into a new Set, and inserts that into Set new
Set SetCopy (Set s) {
	Set new = newSet();
	for (Link curr = s->elems; curr != NULL; curr = curr->next) {
		insertNoSort(new, curr->val, curr->temp, curr->pr);
	}
	return new;
}
