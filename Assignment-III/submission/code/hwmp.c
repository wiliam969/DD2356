#include <stdio.h>
#include <omp.h>

int main(int argc, char argv[]){


    omp_set_num_threads(4); // Set the number of threads to 4
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num(); // Get the thread ID
        printf("Hello World from thread %d!\n", thread_id); // Print the thread ID
    }
    return 0;
}




