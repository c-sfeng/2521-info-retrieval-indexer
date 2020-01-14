
// =============================================================================
// readData.c
// =============================================================================
// Provides functions to read from a set of urls, and place into a Set ADT and 
// Graph ADT
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

#define MAXFILENAME         100
#define BUFSIZE             1024

static char *strdupImp (char *);

// Create a new linked list of URLs to process
Set GetCollection (void) {
    // Read data from file "collection.txt"
    FILE *urlAggregate = fopen("collection.txt", "r");
    if (urlAggregate == NULL) {
        printf("Invalid file: collection.txt\n");
        return NULL;
    }

    char urlName[MAXFILENAME];

    Set collection = newSet();

    // for every word in urlAggregate, add into collection if not already in
    while (fscanf(urlAggregate, "%s", urlName) != EOF)
        if (!isElem(collection, urlName)) insertInto(collection, urlName);

    assert(!fclose(urlAggregate));

    return collection;
}

// Generate a Graph from a Set
Graph GetGraph(Set collection) {
    // For each graph in the Set argument, read the file and create a 
	// directed edge between the origin and any files in the file itself
    Graph g = newGraph(nElems(collection));
    Link curr = collection->elems;
    char *currUrl = malloc(BUFSIZE);
    if (currUrl == NULL) {
        printf("Could not allocate memory to currUrl\n");
        return NULL;
    }

    char *urlConn = malloc(BUFSIZE);
    if (urlConn == NULL) {
        printf("Could not allocate memory to urlConn\n");
        return NULL;
    }
    
    char *filePath = malloc(BUFSIZE);
    if (filePath == NULL) { 
        printf("Could not allocate memory to filePath\n");
        return NULL;
    }

    int read = 0;

    // opens each file in the set
    while (curr != NULL) {
        currUrl = strdupImp(curr->val);
        sprintf(filePath, "%s.txt", currUrl);        
        FILE *urlFile = fopen(filePath, "r");
        if (urlFile == NULL) {
            printf("Invalid file: %s\n", filePath);
            return NULL;
        }
        
        // open each file and read section 1
        while (fscanf(urlFile, "%s", urlConn) != EOF || read != 2) {
            // while section 1 has not begun
			while (read == 0) {
                if (!strcmp (urlConn, "#start")) {
                    fscanf(urlFile, "%s", urlConn);
                    if (!strcmp (urlConn, "Section-1")) {
                        read = 1; 
                        break;
                    }
                }
            } 
			// while section 1 is being read
            while (read == 1) { 
				// looking for the end of section 1
                fscanf(urlFile, "%s", urlConn);
                if (!strcmp (urlConn, "#end")) {
                    fscanf(urlFile, "%s", urlConn);
                    if (!strcmp (urlConn, "Section-1")) {
                        read = 2; 
                        break;
                    }
                }
                // if the two files are not already connected (directed), add a directed edge
                if (!isConnected(g, currUrl, urlConn) && strcmp(currUrl, urlConn)) {
                    assert(addEdge(g, currUrl, urlConn));
                }
            }
        }

        assert(!fclose(urlFile));
        memset(urlConn, 0, BUFSIZE);
        memset(filePath, 0, BUFSIZE);
        read = 0;
        curr = curr->next;
    }

    free (currUrl);
    free (urlConn);
    free (filePath);
    return g;
}

// Implementation of strdup
static char *strdupImp (char *original) {
    char *duplicate = malloc(strlen (original) + 1);  
    if (!duplicate) return NULL;          
    
    strcpy(duplicate, original);                 
    return duplicate;                            
}
