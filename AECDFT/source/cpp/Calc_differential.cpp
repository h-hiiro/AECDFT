#include "LibAECDFT.hpp"
#include <cmath>

#define ALPHA 7.2973525643E-3

// d/dx (L, M)^T = ([0][0] & [0][1] \n [1][0] & [1][1]) (L, M) ^T
void calc_diffs_Sch(int N, double *R, double *V, int l, double epsilon, double *diffCoeffs) {
  int i;
  for (i = 0; i < N; i++) {
    diffCoeffs[4 * i] = 0.0;
    diffCoeffs[4 * i + 1] = 1.0;
    diffCoeffs[4 * i + 2] = 2.0 * R[i] * R[i] * (V[i] - epsilon);
    diffCoeffs[4 * i + 3] = -(2.0 * l + 1.0);
  }
}

// d/dr V(x) = exp(-x) d/dx V(x)
double calc_dVdr(int N, double *X, double DX, double *V, int i) {
  double dVdx;
  if (i == 0) {
    dVdx = (V[i + 1] - V[i]) / DX;
  } else if (i == N - 1) {
    dVdx = (V[i] - V[i - 1]) / DX;
  } else {
    dVdx = (V[i + 1] - V[i - 1]) / DX / 2.0;
  }
  return dVdx * exp(-X[i]);
}

void calc_diffs_SDirac(int N, double *R, double *X, double DX, double *V, int l, double epsilon, double *diffCoeffs) {
  int i;
  for (i = 0; i < N; i++) {
    double m = 1.0 + ALPHA * ALPHA * (epsilon - V[i]) / 2.0;
    double dVdr = calc_dVdr(N, X, DX, V, i);
    diffCoeffs[4 * i] = 0.0;
    diffCoeffs[4 * i + 1] = 1.0;
    diffCoeffs[4 * i + 2] = 2.0 * m * R[i] * R[i] * (V[i] - epsilon);
    diffCoeffs[4 * i + 2] += R[i] * ALPHA * ALPHA / 2.0 / m * dVdr * l;
    diffCoeffs[4 * i + 3] = -(2.0 * l + 1.0);
    diffCoeffs[4 * i + 3] += -R[i] * ALPHA * ALPHA / 2.0 / m * dVdr;
  }
}

void calc_diffs_Dirac(int N, double *R, double *X, double DX, double *V, int l, int kappa, double epsilon, double *diffCoeffs) {
  int i;
  for (i = 0; i < N; i++) {
    double m = 1.0 + ALPHA * ALPHA * (epsilon - V[i]) / 2.0;
    double dVdr = calc_dVdr(N, X, DX, V, i);
    diffCoeffs[4 * i] = 0.0;
    diffCoeffs[4 * i + 1] = 1.0;
    diffCoeffs[4 * i + 2] = 2.0 * m * R[i] * R[i] * (V[i] - epsilon);
    diffCoeffs[4 * i + 2] += R[i] * ALPHA * ALPHA / 2.0 / m * dVdr * (l + 1 + kappa);
    diffCoeffs[4 * i + 3] = -(2.0 * l + 1.0);
    diffCoeffs[4 * i + 3] += -R[i] * ALPHA * ALPHA / 2.0 / m * dVdr;
  }
}