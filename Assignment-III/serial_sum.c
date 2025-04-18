#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void generate_random(double *input, size_t size)
{
  for (size_t i = 0; i < size; i++) {
    input[i] = rand() / (double)(RAND_MAX);
  }
}

double serial_sum(double *x, size_t size)
{
  double sum_val = 0.0;

  for (size_t i = 0; i < size; i++) {
    sum_val += x[i];
  }

  return sum_val;
}