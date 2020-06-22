#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<math.h>
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
  int i;
  for (i = 2; i < n; i++) {
    if (n % i == 0) return false;
  }
  return n > 1;
}

int main(int argc, char * argv[]) {
  unsigned long long i, j, primes = 0, number, prevNumber;
  double t1, t2;

  int num_threads = atoi(argv[1]);
  omp_set_num_threads(num_threads);

  t1 = omp_get_wtime();
  for (i = 0; i < N; i++) {
    prevNumber = powl(2, i - 1);
    number = powl(2, i);
    #pragma omp parallel for reduction(+:primes) schedule(dynamic)
    for (j = prevNumber + 1; j <= number; j++) {
      if (is_prime_optimized(j)) {
        primes++;
      }
    }
    printf("%llu: %llu\n", number, primes);
  }
  t2 = omp_get_wtime();
  printf("Done in %lf s\n", t2-t1);
}
