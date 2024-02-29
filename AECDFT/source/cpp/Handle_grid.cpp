#include <cmath>
#include <cstdio>
#include "LibAECDFT.hpp"

using namespace std;
void fill_grid(int N, double* X, double* R, double XMin, double DX){
    int i;
    for(i=0; i<N; i++){
        X[i]=XMin+DX*i;
        R[i]=exp(X[i]);
        // printf("%.3f %.3f\n", X[i], R[i]);
    }
}

double* scale_grid(int N, double *grid, double scale)
{
    int i;
    double *grid_scaled = alloc_dvector(N);
    for (i = 0; i < N; i++)
    {
        grid_scaled[i] = grid[i] * scale;
    }
    return grid_scaled;
}