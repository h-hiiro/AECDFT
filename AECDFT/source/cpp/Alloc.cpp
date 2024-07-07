#include "LibAECDFT.hpp"
#include <cstdio>

// One-dimensional vector
double *alloc_dvector(int N) {
  double *alloc = new double[N];
  return alloc;
}

// One-dimensional vector
int *alloc_ivector(int N) {
  int *alloc = new int[N];
  return alloc;
}

void free_dvector(double *V) { delete[] V; }
void free_ivector(int *V) { delete[] V; }

// Two-dimensional matrix
double **alloc_dmatrix(int N) {
  double *alloc = new double[N * N];
  double **array = new double *[N];
  int i;
  for (i = 0; i < N; i++) {
    array[i] = &alloc[i * N];
  }
  return array;
}
void free_dmatrix(double **M) {
  delete[] M[0];
  delete[] M;
}

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

void print_dvector3(int N, double *V1, double *V2, double *V3, char *format) {
  printf("[Print_dvector2]\n");
  printf("Vector size: %d\n", N);
  int i;
  char *buffer = new char[1024];
  for (i = 0; i < N; i++) {
    sprintf(buffer, "Element[%%d]= %s %s %s\n", format, format, format);
    printf(buffer, i, V1[i], V2[i], V3[i]);
  }
  printf("Vector size: %d\n\n", N);
  delete[] buffer;
}

void set_value(double *V, int i, double x) { V[i] = x; }