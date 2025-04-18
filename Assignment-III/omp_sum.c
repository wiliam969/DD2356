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

double omp_sum(double *x, size_t size)
{


  double sum_val = 0.0;
  #pragma omp parallel for
    for (size_t i = 0; i < size; i++) {
      sum_val += x[i];
    }

  return sum_val;
}