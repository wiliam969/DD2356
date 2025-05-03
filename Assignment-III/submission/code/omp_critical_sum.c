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



double omp_critical_sum(double *x, size_t size)
{
  double start_time, end_time;

  start_time = omp_get_wtime();

  double sum_val = 0.0;
  #pragma omp parallel for 
    for (size_t i = 0; i < size; i++)
    {
      printf("inside for loop");

      #pragma omp critical
      {
        sum_val += x[i];
      }
    }

  end_time = omp_get_wtime();

  printf("Execution time with scheduling: %f seconds\n", end_time - start_time);

  return sum_val;
}

int main(int argc, char **argv) {

  if(argc < 2){
    printf("!!\tERROR: Expected 1 argument, got %d\n", argc - 1);
    return 1;
  }

  int N = atoi(argv[1]);

  double *n_input = (double *)malloc(N * sizeof(double));

  generate_random(n_input, N); 

  omp_critical_sum(n_input, N); 
}