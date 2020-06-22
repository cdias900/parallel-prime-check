
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "omp.h"

#define N 21
#define THREADS 1

__global__ void prime_kernel(int *prime_count) {
  int threads = blockDim.x,
    blocks = gridDim.x,
    thread_id = threadIdx.x,
    block_id = blockIdx.x;
  int number, i, j, k, count;
  bool prime;
  for (i = block_id; i < N; i += blocks) {
    count = 0;
    number = pow((double)2, (double)i);
    for (j = thread_id; j <= number; j += threads) {
      if (j % 2 == 0) {
        count += j == 2;
        continue;
      }
      prime = true;
      for (k = 3; k * k <= j; k += 2) {
        if (j % k == 0) {
          prime = false;
          break;
        }
      }
      if (prime && j > 1) count += 1;
    }
    atomicAdd(&prime_count[i], count);
  }
}

int main() {
  int *prime_count, *prime_count_gpu;
  double t1, t2;

  t1 = omp_get_wtime();
  prime_count = (int*)calloc(sizeof(int), N);
  t2 = omp_get_wtime();
  printf("Done allocating (CPU) in %lf s\n", t2 - t1);

  t1 = omp_get_wtime();
  cudaMalloc((void**)&prime_count_gpu, sizeof(int) * N);
  t2 = omp_get_wtime();
  printf("Done allocating (GPU) in %lf s\n", t2 - t1);

  t1 = omp_get_wtime();
  cudaMemcpy(prime_count_gpu, prime_count, sizeof(int) * N, cudaMemcpyHostToDevice);
  t2 = omp_get_wtime();
  printf("Done copying to GPU in %lf s\n", t2 - t1);

  t1 = omp_get_wtime();
  prime_kernel <<<N, THREADS>>> (prime_count_gpu);
  cudaDeviceSynchronize();
  t2 = omp_get_wtime();
  printf("Done calculating in %lf s\n", t2 - t1);

  t1 = omp_get_wtime();
  cudaMemcpy(prime_count, prime_count_gpu, sizeof(int) * N, cudaMemcpyDeviceToHost);
  t2 = omp_get_wtime();
  
  printf("Done copying to CPU in %lf s\n", t2 - t1);
  int i;
  for (i = 0; i < N; i++) {
    printf("%i: %i\n", (int)pow(2, i), prime_count[i]);
  }
  free(prime_count);
  cudaFree(prime_count_gpu);
  return 0;
}
