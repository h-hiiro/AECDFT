#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
    // Alloc.cpp
    double* alloc_dvector(int N);
    void free_dvector(double* V);
    void print_dvector(int N, double* V, char* format);
    void print_dvector2(int N, double* V1, double* V2, char* format);
    // Handle_grid.cpp
    void fill_grid(int N, double* X, double* R, double XMin, double DX);
    double* scale_grid(int N, double* grid, double scale);
    // Calc_TF.cpp
    bool calc_TFPotential(int N, double* X, double* VTF, double InitialDiff_left, double InitialDiff_right, double Threshold);
    double TF_evolution(int N, double *X, double *VTF, double initial_diff);
    void calc_TFDensity(int N, double* R, double* VTF, int Z, double* V, double* rho);
#ifdef __cplusplus
}
#endif