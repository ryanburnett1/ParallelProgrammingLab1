#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

// Returns a random value between -1 and 1
double getRand(unsigned int *seed) {
    return (double) rand_r(seed) * 2 / (double) (RAND_MAX) - 1;
}

long double Calculate_Pi_Sequential(long long number_of_tosses) {
    unsigned int seed = (unsigned int) time(NULL);
    int number_in_circle = 0;
    double distance_squared;
    unsigned int *ptr = &seed;
    for(int i = 0; i < number_of_tosses; i++){
        double x = getRand(ptr);
        double y = getRand(ptr);
        distance_squared = x*x + y*y;
        if(distance_squared <= 1)
            number_in_circle++;
    }
    return 4*number_in_circle/(double)number_of_tosses;
}

long double Calculate_Pi_Parallel(long long number_of_tosses) {
    int global_number_in_circle = 0;
    int local_number_of_tosses = number_of_tosses/omp_get_max_threads();
#pragma omp parallel num_threads(omp_get_max_threads())
    {
        unsigned int seed = (unsigned int) time(NULL) + (unsigned int) omp_get_thread_num();
        int my_number_in_circle = 0;
        double distance_squared;
        unsigned int *ptr = &seed;
        for (int i = 0; i < local_number_of_tosses; i++) {
            double x = getRand(ptr);
            double y = getRand(ptr);
            distance_squared = x * x + y * y;
            if (distance_squared <= 1)
                my_number_in_circle++;
        }
#pragma omp critical
        global_number_in_circle += my_number_in_circle;
    }
    return 4*global_number_in_circle/(double)number_of_tosses;
}

int main() {
    struct timeval start, end;

    long long num_tosses = 10000000;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    long double sequential_pi = Calculate_Pi_Sequential(num_tosses);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    long double parallel_pi = Calculate_Pi_Parallel(num_tosses);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    // This will print the result to 10 decimal places
    printf("π = %.10Lf (sequential)\n", sequential_pi);
    printf("π = %.10Lf (parallel)", parallel_pi);

    return 0;
}