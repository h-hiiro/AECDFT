#include <cstdio>
#include "lapack_header.hpp"


double** alloc_matrix(int N){
    double** matrix=new double*[N];
    double* alloc=new double[N*N];
    for(int i=0; i<N; i++){
        matrix[i]=&alloc[i*N];
    }
    return matrix;
}
void free_matrix(double** mat){
    delete[] mat[0];
    delete[] mat;
}

void dgemm_calc(double** A, double** B, double** C, int N)
{
    int i,j;
    
    char notrans='N';
    double alpha=1.0;
    double beta=0.0;

    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            A[i][j]=i+j;
            B[i][j]=i-j;
        }
    }
    dgemm_(&notrans, &notrans, &N, &N, &N, &alpha, &A[0][0], &N, &B[0][0], &N, &beta, &C[0][0], &N);
}