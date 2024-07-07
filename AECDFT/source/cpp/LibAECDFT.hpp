#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
// Alloc.cpp
double *alloc_dvector(int N);
int *alloc_ivector(int N);
double **alloc_dmatrix(int N);
void free_dvector(double *V);
void free_ivector(int *V);
void free_dmatrix(double **M);
void print_dvector(int N, double *V, char *format);
void print_dvector2(int N, double *V1, double *V2, char *format);
void print_dvector3(int N, double *V1, double *V2, double *V3, char *format);
void set_value(double *V, int i, double x);
// Handle_grid.cpp
void fill_grid(int N, double *X, double *R, double XMin, double DX);
double *scale_grid(int N, double *grid, double scale);
// Calc_TF.cpp
bool calc_TFPotential(int N, double *X, double *VTF, double InitialDiff_left, double InitialDiff_right, double Threshold);
double TF_evolution(int N, double *X, double *VTF, double initial_diff);
void calc_TFDensity(int N, double *R, double *VTF, double Z, double *V, double *rho);
// Calc_potential.cpp
void calc_core(int N, double *R, double Z, double *V);
void calc_hartree(int N, double *R, double DX, double *rho, double *V);
void calc_xc_Xa(int N, double alpha, double *rho, double *V);
void calc_xc_LDA_CA(int N, double *rho, double *V);
void calc_xc_LDA_VWN(int N, double *rho, double *V);
void calc_sum(int N, double *V_core, double *V_hartree, double *V_xc, double *V_total);
// Calc_differential.cpp
void calc_diffs_Sch(int N, double *R, double *V, int l, double epsilon, double *diffCoeffs);
void calc_diffs_SDirac(int N, double *R, double *X, double DX, double *V, int l, double epsilon, double *diffCoeffs);
void calc_diffs_Dirac(int N, double *R, double *X, double DX, double *V, int l, int kappa, double epsilon, double *diffCoeffs);
// Fit_potential.coo
bool fit_potential(int N, double *R, double *V, int order, double *fit_coeffs, double *V_fit);
// Utils.cpp
double intpow(double a, int b);
#ifdef __cplusplus
}
#endif