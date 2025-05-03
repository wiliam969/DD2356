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

double serial_sum(double *x, size_t size)
{
  double start_time, end_time;

  start_time = omp_get_wtime();
  double sum_val = 0.0;

  for (size_t i = 0; i < size; i++) {
    sum_val += x[i];
  }

  end_time = omp_get_wtime();

  printf("Execution time with scheduling: %f seconds\n", end_time - start_time);
  printf("serial_sum-version: %f", sum_val); 
  return sum_val;
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
  static double local_sum_val[MAX_THREADS] = {0.0};
  double total_sum_val = 0.0;

  start_time = omp_get_wtime();
  
  #pragma omp parallel shared (local_sum_val)
  {
    int thread_id = omp_get_thread_num();

    #pragma omp for
    for (size_t i = 0; i < size; i++) {
      local_sum_val[thread_id] += x[i];
    }
  }
    
  for (int i = 0; i < MAX_THREADS; i++) {
    total_sum_val += local_sum_val[i];    
  }

  end_time = omp_get_wtime();

  printf("Execution time with scheduling: %f seconds\n", end_time - start_time);
    
  printf("omp_local_sum-version: %f", total_sum_val); 

  return total_sum_val;
}

int main(int argc, char **argv) {

  if(argc < 2){
    printf("!!\tERROR: Expected 1 argument, got %d\n", argc - 1);
    return 1;
  }

  int N = atoi(argv[1]);

  double *n_input = (double *)malloc(N * sizeof(double));

  generate_random(n_input, N); 

  serial_sum(n_input, N); 
  omp_local_sum(n_input, N); 
}