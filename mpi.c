#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<math.h>
#include<mpi.h>
#include "omp.h"

#define N 21

bool is_prime_optimized(unsigned long long n) {
  unsigned long long i;
  if (n % 2 == 0) return n == 2;
  for (i = 3; i * i <= n; i+=2) {
    if (n % i == 0) return false;
  }
  return n > 1;
}

bool is_prime(unsigned long long n) {
  unsigned long long i;
  for (i = 2; i < n; i++) {
    if (n % i == 0) return false;
  }
  return n > 1;
}

int main(int argc, char * argv[]) {
  unsigned long long i, j, aux, count = 0, primes, number, prevNumber;
  double t1, t2;

  int size, rank;
  MPI_Status st;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  if (rank == 0) {
    t1 = omp_get_wtime();
    for (i = 0; i < N; i++) {
      for (j = 1; j < size; j++) {
        MPI_Send(&i, 1, MPI_UNSIGNED_LONG_LONG, j, 0, MPI_COMM_WORLD);
      }
      for (j = 1; j < size; j++) {
        MPI_Recv(&aux, 1, MPI_UNSIGNED_LONG_LONG, j, 0, MPI_COMM_WORLD, &st);
        count += aux;
      }
      MPI_Recv(&number, 1, MPI_UNSIGNED_LONG_LONG, 1, 0, MPI_COMM_WORLD, &st);
      printf("%llu: %llu\n", number, count);
      fflush(stdout);
    }
    t2 = omp_get_wtime();
    printf("Done in %lf s\n", t2-t1);
    fflush(stdout);
  } else {
    for(i = 0; i < N; i++) {
      MPI_Recv(&aux, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &st);
      primes = 0;
      prevNumber = powl(2, aux - 1);
      number = powl(2, aux);
      for (j = prevNumber + rank; j <= number; j += size - 1) {
        if (is_prime_optimized(j)) {
          primes++;
        }
      }
      MPI_Send(&primes, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
      if (rank == 1) MPI_Send(&number, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }
  }

  MPI_Finalize();
}
