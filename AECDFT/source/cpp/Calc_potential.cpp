#include "LibAECDFT.hpp"
#include <cmath>
#include <cstdio>

void calc_core(int N, double *R, double Z, double *V) {
  int i;
  for (i = 0; i < N; i++) {
    V[i] = -Z / R[i];
  }
}

// dr = r * dx
void calc_hartree(int N, double *R, double DX, double *rho, double *V) {
  int i, j;
  double V_inside, V_outside;
  for (i = 0; i < N; i++) {
    // inside
    V_inside = 0;
    for (j = 0; j <= i; j++) {
      V_inside += pow(R[j], 3) * rho[j] * DX;
    }
    V_inside *= 4.0 * M_PI / R[i];

    // outside
    V_outside = 0;
    for (j = i + 1; j < N; j++) {
      V_outside += pow(R[j], 2) * rho[j] * DX;
    }
    V_outside *= 4.0 * M_PI;

    // sum
    V[i] = V_inside + V_outside;
  }
}

void calc_xc_Xa(int N, double alpha, double *rho, double *V) {
  int i;
  for (i = 0; i < N; i++) {
    V[i] = -3.0 / 2.0 * alpha * pow(3.0 * rho[i] / M_PI, 1.0 / 3.0);
  }
}

void calc_xc_LDA_CA(int N, double *rho, double *V) {
  int i;
  for (i = 0; i < N; i++) {
    double rs = pow(3.0 / 4.0 / M_PI, 1.0 / 3.0) * pow(rho[i], -1.0 / 3.0);
    // exchange
    double Ex = -0.4582 / rs;
    double dEx = 0.4582 / pow(rs, 2.0);
    // correlation
    double Ec, dEc;
    if (1.0 <= rs) {
      double denominator = 1.0 + 1.0529 * sqrt(rs) + 0.3334 * rs;
      Ec = -0.1423 / denominator;
      dEc = 0.1423 / pow(denominator, 2.0) * (1.0529 * 0.5 / sqrt(rs) + 0.3334);
    } else {
      Ec = -0.0480 + 0.0311 * log(rs) - 0.0116 * rs + 0.0020 * rs * log(rs);
      dEc = 0.0311 / rs + 0.0020 * log(rs) - 0.0096;
    }
    V[i] = Ex + Ec - 1.0 / 3.0 * rs * (dEx + dEc);
  }
}

void calc_xc_LDA_VWN(int N, double *rho, double *V) {
  int i;
  for (i = 0; i < N; i++) {
    double rs = pow(3.0 / 4.0 / M_PI, 1.0 / 3.0) * pow(rho[i], -1.0 / 3.0);
    // exchange
    double Ex = -0.4582 / rs;
    double dEx = 0.4582 / pow(rs, 2.0);
    // correlation
    double A = 0.0310907;
    double b = 3.72744;
    double c = 12.9352;
    double x0 = -0.10498;
    double x = sqrt(rs);
    double Xx = pow(x, 2) + b * x + c;
    double Xx0 = pow(x0, 2) + b * x0 + c;
    double Q = sqrt(4 * c - pow(b, 2));

    double Ec =
        A * (log(pow(x, 2) / Xx) + 2.0 * b / Q * atan(Q / (2.0 * x + b)) -
             b * x0 / Xx0 *
                 (log(pow(x - x0, 2) / Xx) +
                  2.0 * (b + 2.0 * x0) / Q * atan(Q / (2.0 * x + b))));

    double dEc_denom = 4.0 * pow(x, 2) + 4 * b * x + pow(b, 2) + pow(Q, 2);
    double dEc = A / 2.0 / pow(x, 2) *
                 ((b * x + 2.0 * c) / Xx - 4.0 * b * x / dEc_denom -
                  b * x * x0 / Xx0 *
                      ((2.0 * x * x0 + b * (x + x0) + 2.0 * c) / (x - x0) / Xx -
                       4.0 * (b + 2.0 * x0) / dEc_denom));

    V[i] = Ex + Ec - 1.0 / 3.0 * rs * (dEx + dEc);
  }
}

void calc_sum(int N, double *V_core, double *V_hartree, double *V_xc,
              double *V_total) {
  int i;
  for (i = 0; i < N; i++) {
    V_total[i] = V_core[i] + V_hartree[i] + V_xc[i];
  }
}