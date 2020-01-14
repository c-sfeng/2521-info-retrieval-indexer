
// =============================================================================
// scaledFootrule.c
// =============================================================================
// Outputs a list of urls sorted so that the scaled footrule value is a minimum
// =============================================================================
// Written by z5260633 for COMP2521 Assignment 2 (August 2019)
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "graph.h"
#include "set.h"
#include "setAgg.h"
#include "readData.h"

#define BUFSIZE     1024

static double sumScaledFootrule (SetAgg);
static void heapPermutation (int *, int, int, SetAgg, Set);
static void swap (int *, int, int);
static Set createUnionSet (SetAgg);
static int PIndex(Set, char *);
static void calculateScaledFootrule (SetAgg, Set, int *);
static char *removePunct(char *);
static char *strdupImp (char *);

int main (int argc, char *argv[]) {
    if (argc == 1) {
        printf("Usage: ./scaledFootrule arg1 ... argN\n");
        return 0;
    }

    Set collection = GetCollection();
    if (collection == NULL) return 0;

    SetAgg setAggregate = newSetAgg();

    char **fileNames = calloc(BUFSIZE, sizeof(char *));
    if (fileNames == NULL) {
        printf("Could not allocate memory for fileNames\n");
        return 0;
    }

    char *word = malloc(BUFSIZE);
    if (word == NULL) {
        printf("Could not allocate memory for word\n");
        return 0;
    }

    // places the arguments into an array
    for (int i = 1; argv[i] != NULL; i++) fileNames[i - 1] = strdupImp(argv[i]);

    // inserts sets into setAggregate
    for (int i = 0; fileNames[i] != NULL; i++) {
        FILE *fp = fopen(fileNames[i], "r");
        if (fp == NULL) { 
            printf("Invalid file: %s\n", fileNames[i]);
            return 0;
        }

        Set new = newSet();

        while (fscanf(fp, "%s", word) != EOF) {
            char *wordTemp = removePunct(word);
            if (isElem(collection, wordTemp) && !isElem(new, wordTemp))
                insertNoSort(new, wordTemp, 0, 0);
        }

        insertSetAggNoSort(setAggregate, new);
         
        dropSet(new);
        assert(!fclose(fp));
    }

    dropSet(collection);
    free(fileNames);

    Set unionSet = createUnionSet(setAggregate);
    int unionSetSize = nElems(unionSet);

    setAggregate->Parr = malloc(unionSetSize * sizeof(int));
    if (setAggregate->Parr == NULL) {
        printf("Could not allocate memory for setAggregate->Parr\n");
        return 0;
    }

    int *Parr = malloc (unionSetSize * sizeof(int));
    if (Parr == NULL) {
        printf("Could not allocate memory for Parr\n");
        return 0;
    }

    for (int i = 0; i < unionSetSize; i++) Parr[i] = i + 1;

    // recursively calculating w(c,p) for all permuations of Parr
    heapPermutation (Parr, unionSetSize, unionSetSize, setAggregate, unionSet);

    // outputting the minimum scaled-footrule distance and array
    printf("%.6f\n", setAggregate->sumSF);    
    for (int i = 0; setAggregate->Parr[i] != 0; i++) {
        int targetIndex = setAggregate->Parr[i];
        int unionIndex = 1;
        for (Link curr = unionSet->elems; curr != NULL; curr = curr->next) {
            if (unionIndex == targetIndex) {
                printf("%s\n", curr->val);
                break;
            }
            unionIndex++;
        }
    }

    return 0;
}

// summates all scaled-footrules
static double sumScaledFootrule (SetAgg sa) {
    double sum = 0;

    for (Set subset = sa->subsetHead; subset != NULL; subset = subset->next) {
        for (Link curr = subset->elems; curr != NULL; curr = curr->next) {
            sum += curr->pr;
        }
    }

    return sum;
}

// recursively generates all permutations of arr, including calculating and
// storing the minimum scaled-footrule and associated array
static void heapPermutation (int *arr, int size, int n, SetAgg sa, Set s) {
    if (size == 1) {
        // calculate the w(c,p)
        calculateScaledFootrule(sa, s, arr);

        // assigning as minimum
        double sum = sumScaledFootrule(sa);
        if (sa->sumSF == 0 || sum < sa->sumSF) {
            sa->sumSF = sum;
            for (int i = 0; i < n; i++) {
                sa->Parr[i] = arr[i];
            }
        }
        return;
    }

    for (int i = 0; i < size; i++) {
        heapPermutation(arr, size - 1, n, sa, s);
        if (size % 2 == 1) swap(arr, 0, size - 1);
        else swap(arr, i, size - 1);
    }
}

// helper function to switch ints in array arr
static void swap (int *arr, int a, int b) {
    int temp;
    temp = arr[b];
    arr[b] = arr[a];
    arr[a] = temp;
}

// returns a set made of the union of all other sets, alphabetically arranged
static Set createUnionSet (SetAgg sa) {
    Set unionSet = newSet();
    for (Set subset = sa->subsetHead; subset != NULL; subset = subset->next) {
        for (Link curr = subset->elems; curr != NULL; curr = curr->next) {
            if (!isElem(unionSet, curr->val)) insertInto(unionSet, curr->val);
        }
    }

    return unionSet;
}

// returns the index of str from the unionSet 
static int PIndex(Set unionSet, char *str) {
    int p = 0;
    for (Link curr = unionSet->elems; curr != NULL; curr = curr->next) {
        if (!strcmp(curr->val, str)) break;
        p++;
    }
    return p;
}

// calculates the scaled-footrule for a given set
static void calculateScaledFootrule (SetAgg sa, Set unionSet, int *arr) {
    // reappropriate pr for w(c,p)
    for (Set subset = sa->subsetHead; subset != NULL; subset = subset->next) {
        int position = 1;
        for (Link curr = subset->elems; curr != NULL; curr = curr->next) {
            int size = nElems(subset);
            int unionSetSize = nElems(unionSet);
            int unionIndex = arr[PIndex(unionSet, curr->val)];
            double result = fabs(((double) position / (double) size) -
                            ((double) unionIndex / (double) unionSetSize));
            curr->pr = result;
            position++;
        }
    }

    return;
}

// clearing the word after punctuation is encountered
static char *removePunct(char *original) {
    char *new = strdupImp(original);
    int completeWord = 0;
    for (int i = 0; original[i] != 0; i++) {
        if (completeWord == 1) new[i] = 0;
        if (ispunct(original[i])) {
            new[i] = 0;
            completeWord = 1;
        }
    }
    return new;
}

// Implementation of strdup
static char *strdupImp (char *original) {
    char *duplicate = malloc(strlen (original) + 1);  
    if (!duplicate) return NULL;          
    
    strcpy(duplicate, original);                 
    return duplicate;                            
}
