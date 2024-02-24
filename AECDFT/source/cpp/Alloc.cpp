#include "LibAECDFT.hpp"

double* alloc_dvector(int N){
    double* alloc=new double[N];
    return alloc;
}

void free_dvector(double* v){
    delete[] v;
}