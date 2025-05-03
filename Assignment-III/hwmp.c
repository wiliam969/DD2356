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




/*
ml PDC/23.12
ml cpeGNU/23.12
gcc -fopenmp hwmp.c -o hwmp.x
export OMP_NUM_THREADS=4
salloc -n 1 -t 0:10:00 -A edu25.DD2356 -p shared
srun -n 1 ./hwmp.x

*/