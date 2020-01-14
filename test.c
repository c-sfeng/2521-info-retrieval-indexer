#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void heapPermutation (int *arr, int size, int n);
void printArr(int *arr, int n);
void swap (int *arr, int a, int b);

int main (void) {
    int *arr = malloc(12);
    int size = 3;
    int n = 3;
    for (int i = 0; i < n; i++) {
        arr[i] = i + 1;
    }

    heapPermutation(arr, size, n);

    double x = 7;
    double y = 5;
    double result = x / y; 

    printf("%f\n", result);

    return 0;
}

void heapPermutation (int *arr, int size, int n) {
    if (size == 1) {
        // calculate the w(c,p)
        printArr(arr, n);
        return;
    }

    for (int i = 0; i < size; i++) {
        heapPermutation(arr, size - 1, n);
        if (size % 2 == 1) swap(arr, 0, size - 1);
        else swap(arr, i, size - 1);
    }
}

void printArr(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void swap (int *arr, int a, int b) {
    int temp;
    temp = arr[b];
    arr[b] = arr[a];
    arr[a] = temp;
}