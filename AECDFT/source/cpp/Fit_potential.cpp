#include "LibAECDFT.hpp"
#include <cstdio>
using namespace std;

extern "C" {
void dgesv_(
    int *N,
    int *NRHS,
    double *A,
    int *LDA,
    int *IPIV,
    double *B,
    int *LDB,
    int *INFO);
}
// the first {order} points are used to determine the fitting polynomial coefficients
bool fit_potential(int N, double *R, double *V, int order, double *fit_coeffs, double *V_fit) {
  // matrix is transposed
  double **matrix = alloc_dmatrix(order);
  double *vector = alloc_dvector(order);

  int *ipiv = alloc_ivector(order);

  // set matrix and vector values
  int i, j;
  for (i = 0; i < order; i++) {
    for (j = 0; j < order; j++) {
      matrix[j][i] = intpow(R[i], j);
    }
    vector[i] = V[i];
  }
  int ONE = 1;
  int INFO;
  dgesv_(&order, &ONE, &matrix[0][0], &order, &ipiv[0], &vector[0], &order, &INFO);
  if (INFO != 0) {
    printf("error in dgesv\n");
    return false;
  }

  // ok
  for (i = 0; i < order; i++) {
    fit_coeffs[i] = vector[i];
  }
  for (i = 0; i < N; i++) {
    V_fit[i] = 0;
    for (j = 0; j < order; j++) {
      V_fit[i] += fit_coeffs[j] * intpow(R[i], j);
    }
  }

  free_dmatrix(matrix);
  free_dvector(vector);
  free_ivector(ipiv);
  return true;
}