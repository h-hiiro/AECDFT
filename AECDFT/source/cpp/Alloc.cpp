#include "LibAECDFT.hpp"
#include <cstdio>

// One-dimensional vector
double *alloc_dvector(int N) {
  double *alloc = new double[N];
  return alloc;
}

void free_dvector(double *V) { delete[] V; }

void print_dvector(int N, double *V, char *format) {
  printf("[Print_dvector]\n");
  printf("Vector size: %d\n", N);
  int i;
  char *buffer = new char[1024];
  for (i = 0; i < N; i++) {
    sprintf(buffer, "Element[%%d]= %s\n", format);
    printf(buffer, i, V[i]);
  }
  printf("Vector size: %d\n\n", N);
  delete[] buffer;
}

void print_dvector2(int N, double *V1, double *V2, char *format) {
  printf("[Print_dvector2]\n");
  printf("Vector size: %d\n", N);
  int i;
  char *buffer = new char[1024];
  for (i = 0; i < N; i++) {
    sprintf(buffer, "Element[%%d]= %s %s\n", format, format);
    printf(buffer, i, V1[i], V2[i]);
  }
  printf("Vector size: %d\n\n", N);
  delete[] buffer;
}

void set_value(double *V, int i, double x) { V[i] = x; }