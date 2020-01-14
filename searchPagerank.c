
// =============================================================================
// searchPagerank.c
// =============================================================================
// Takes a set of query words, and then outputs the files they appear in, first 
// in order of unique instances of appearance, then by the weighted pageRank 
// algorithm
// =============================================================================
// Written by z5260633 for COMP2521 Assignment 2 (August 2019)
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string.h>

#include "graph.h"
#include "set.h"
#include "readData.h"

#define BUFSIZE     1024

static Set findMatchedUrls (char *, char **);
static void incrementSearch (Set, char *);
static Set findPagerank (char *, Set);
static Set urlPRList (char *);
static char *strdupImp (char *);

int main (int argc, char *argv[]) {
    if (argc == 1) {
        printf("Usage: ./searchPagerank arg1 ... argN\n");
        printf("No results found\n");
        return 0;
    }
    
    char **searchTerms = calloc (BUFSIZE, sizeof (char *));
    if (searchTerms == NULL) {
        printf("Could not allocate memory for searchTerm\n");
        return 0;
    }
    
    // places the arguments into an array
    for (int i = 1; argv[i] != NULL; i++) {
        searchTerms[i - 1] = strdupImp(argv[i]);
    }

    // returns a list of urls that the searches appear in
    Set matchedUrls = findMatchedUrls("invertedIndex.txt", searchTerms);

    Set searchResults = findPagerank("pagerankList.txt", matchedUrls);

    int searchResultNumber = 0;

    // if the number of searches exceeds 30, break;
    for (Link curr = searchResults->elems; curr != NULL; curr = curr->next) {
        if (searchResultNumber == 30) break;
        searchResultNumber++;
        printf("%s\n", curr->val);
    }

    if (searchResultNumber == 0) printf("No results found\n");

    free(searchTerms);
    dropSet(matchedUrls);
    dropSet(searchResults);

    return 0;
}

// returns a list of the urls containing the queryWords
static Set findMatchedUrls (char *filePath, char **queryWords) {
    // opens files
    FILE *fp = fopen(filePath, "r");
    assert(fp);

    Set collection = GetCollection();

    Set urlList = newSet();
    char *word = malloc(BUFSIZE);
    if (word == NULL) {
        printf("Could not allocate memory for word\n");
        exit(0);
    }

    int readLine = 0;

    // fscanf through all words in file
    while (fscanf(fp, "%s", word) != EOF) {
        // if the next word is not in the collection and line has not begun,
        // revert line flag
        if (!isElem(collection, word) && readLine == 1) readLine = 0;
        
        // if line has not begun (reads and stores the first word as "word")
        if (readLine == 0) {
            for (int i = 0; queryWords[i] != NULL; i++) {
                if (!strcmp(word, queryWords[i])) {
                    readLine = 1;
                    break;
                }
            }
        // if line has begun (i.e. a line containing a queryWord)
        } else {
            if (!isElem(urlList, word) && isElem(collection, word)) insertInto(urlList, word);
            incrementSearch(urlList, word);
        }
    }

    free(word);
    dropSet(collection);
    assert(!fclose(fp));

    return urlList;
}

// increments the curr->temp to indicate the number of times a search term 
// appears 
static void incrementSearch (Set s, char *word) {
    for (Link curr = s->elems; curr != NULL; curr = curr->next)
        if (!strcmp(curr->val, word)) curr->temp++;
}

// returns the #searchterms-sorted / pr-sorted list
static Set findPagerank (char *filePath, Set s) {
    // s is the set with number of how many times a search appears in node->temp
    Set searchResults = newSet();
    
    // set of urls sorted by pr
    Set orderedPR = urlPRList(filePath);

    // moves the number of search hits per url to prSortedUrlList
    // curr->temp is already 0 by urlPRList(filePath)
    for (Link curr = orderedPR->elems; curr != NULL; curr = curr->next) {
        for (Link search = s->elems; search != NULL; search = search->next) {
            if (!strcmp(search->val, curr->val)) {
                curr->temp = search->temp;
                break;
            }
        }
    }

    // finds the maximum number of times search terms appear
    double max = 0;
    for (Link curr = orderedPR->elems; curr != NULL; curr = curr->next)
        if (curr->temp >= max) max = curr->temp;

    // if max is 0, return empty Set
    if (max == 0) return searchResults;

    while (max > 0) {
        // goes through the orderedPR set (Sorted descending by prWeight)
        // if curr->temp == max, insert (no sort) into SearchResults as
        // its assumed to be sorted by PR already
        for (Link curr = orderedPR->elems; curr != NULL; curr = curr->next) {
            if (curr->temp == max) {
                insertNoSort(searchResults, curr->val, 
                             max, curr->pr);
            } 
        }
        max--;
    }

    return searchResults;
}

// returns a set of the urls involved, sorted by PageRank
static Set urlPRList (char *filePath) {
    // open file
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL) {
        printf("Invalif filepath: %s\n", filePath);
        exit(0);
    }

    Set urlPRList = newSet();

    char *name = malloc(BUFSIZE);
    if (name == NULL) {
        printf("Could not allocate memory for name\n");
        exit(0);
    }

    int outlinks = 0;
    double pr = 0;

    // fscanfs through the file and extracts information per line
    // inserts the node into the set urlPRList
    while (fscanf(fp, "%s %d, %lf", name, &outlinks, &pr) != EOF) {
        name[strlen(name) - 1] = 0;
        insertNoSort(urlPRList, name, 0, pr);
    }

    // clean up
    free(name);
    assert(!fclose(fp));

    return urlPRList;
}

// Implementation of strdup
static char *strdupImp (char *original) {
    char *duplicate = malloc(strlen (original) + 1);  
    if (!duplicate) return NULL;          
    
    strcpy(duplicate, original);                 
    return duplicate;                            
}