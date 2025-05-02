#include <stdio.h>
#include <stdlib.h>
#include <omp.h>



void generate_random(double *input, size_t size)
{
  #pragma omp parallel for

  for (size_t i = 0; i < size; i++) {
    input[i] = rand() / (double)(RAND_MAX);
  }
}


/*
2.Implement a parallel version of the serial_sum called omp_sum 
and use the omp parallel for construct to parallelize the program. Run the code with 32 threads and measure the execution time (average + standard deviation). Should the code be working correctly? If not, why not? 
Please also compare the speedup with the number of cores your job is assigned to. 

Suspect they want to point out that addition is not atomic with this?
*/
#ifndef MAX_THREADS
#define MAX_THREADS 32
#endif


double omp_local_sum(double *x, size_t size)
{
  double start_time, end_time;

  start_time = omp_get_wtime();
  
  double local_sum_val[MAX_THREADS] = {0.0};
  double total_sum_val = 0.0;
  #pragma omp parallel private (+:local_sum_val)
  {

    int thread_id = omp_get_thread_num();
    int num_threads = omp_get_num_threads();
    //divide array into chunk
    size_t chunk_size = size / num_threads;
    size_t start = thread_id * chunk_size;
    // incase division is uneven
    size_t end = (thread_id == num_threads - 1) ? size : start + chunk_size;

    for (size_t i = 0; i < size; i++) {
      
      local_sum_val[i] += x[i];
  
      
    }
  }
    //for each thread
    for (int i = 0; i < MAX_THREADS; i++) {
      total_sum_val += local_sum_val[i];
      
    }

  end_time = omp_get_wtime();

  printf("Execution time with scheduling: %f seconds\n", end_time - start_time);
    
  

  return total_sum_val;
}