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