/*
============================================================================
Filename    : pi.c
Author      : Your names goes here
SCIPER		: Your SCIPER numbers
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

int perform_buckets_computation(int, int, int);

int main (int argc, const char *argv[]) {
    int num_threads, num_samples, num_buckets;

    if (argc != 4) {
		printf("Invalid input! Usage: ./sharing <num_threads> <num_samples> <num_buckets> \n");
		return 1;
	} else {
        num_threads = atoi(argv[1]);
        num_samples = atoi(argv[2]);
        num_buckets = atoi(argv[3]);
	}
    
    set_clock();
    perform_buckets_computation(num_threads, num_samples, num_buckets);

    printf("Using %d threads: %d operations completed in %.4gs.\n", num_threads, num_samples, elapsed_time());
    return 0;
}

#define CACHE_LINE_SIZE 64
#define INT_SIZE sizeof(int)
#define PADDING_INTS ((CACHE_LINE_SIZE / INT_SIZE) - 1)

typedef struct {
    int ct;                 
    int padding[PADDING_INTS]; 
} PaddedCounter;

/* Parallelize and optimise this function */
int perform_buckets_computation(int num_threads, int num_samples, int num_buckets) {    
    volatile int *histogram = (int*) calloc(num_buckets, sizeof(int));

    PaddedCounter **thread_histograms = (PaddedCounter**) malloc(num_threads * sizeof(PaddedCounter*));
    for (int i = 0; i < num_threads; i++) {
        thread_histograms[i] = (PaddedCounter*) calloc(num_buckets, sizeof(PaddedCounter));
    }

    omp_set_num_threads(num_threads);

    #pragma omp parallel shared(thread_histograms)
    {
        int tid = omp_get_thread_num(); // thread id
        rand_gen rng = init_rand(tid); // random number generator
        
        #pragma omp for
        for (int i = 0; i < num_samples; i++) {
            int val = next_rand(rng) * num_buckets;
            thread_histograms[tid][val].ct++;
        }
        
        free_rand(rng);
    }
    
    // merge loop
    #pragma omp parallel for shared(thread_histograms, histogram)
    for (int b = 0; b < num_buckets; b++) 
        for (int t = 0; t < num_threads; t++) // thread id
            histogram[b] += thread_histograms[t][b].ct;
    
    // free allocated memory
    for (int i = 0; i < num_threads; i++) {
        free(thread_histograms[i]);
    }
    free(thread_histograms);

    return 0;
}
