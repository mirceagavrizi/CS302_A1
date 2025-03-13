/*
============================================================================
Filename    : pi.c
Author      : Your names goes here
SCIPER		: Your SCIPER numbers
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "utility.h"

double calculate_pi (int num_threads, int samples);

int main (int argc, const char *argv[]) {

    int num_threads, num_samples;
    double pi;

    if (argc != 3) {
		printf("Invalid input! Usage: ./pi <num_threads> <num_samples> \n");
		return 1;
	} else {
        num_threads = atoi(argv[1]);
        num_samples = atoi(argv[2]);
	}

    set_clock();
    pi = calculate_pi (num_threads, num_samples);

    printf("- Using %d threads: pi = %.15g computed in %.4gs.\n", num_threads, pi, elapsed_time());

    return 0;
}


double calculate_pi (int num_threads, int samples) {
    double pi;
    int count = 0;

    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {

        int tid = omp_get_thread_num(); //thread id
        rand_gen rng = init_rand(tid); //random number generator
        int local_count = 0;

        #pragma omp for
        for(int i = 0; i < samples; i++){
            double x = rng.rand_func(rng);
            double y = rng.rand_func(rng);
            if((x*x + y*y) <= 1.0)
                local_count++;
        }

        #pragma omp atomic
        count += local_count;

        free_rand(rng);
    }


    pi = 4.0 * ((double)count / (double)samples);
    return pi;
}
