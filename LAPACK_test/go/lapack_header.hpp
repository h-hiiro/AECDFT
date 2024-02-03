
#ifdef __cplusplus
extern "C" {
#endif
    void dgemm_calc(double** A, double** B, double** C, int N);
    void dgemm_(
        char *transa,
        char *transb,
        int *m,
        int *n,
        int *k,
        double *alpha,
        double *a,
        int *lda,
        double *b,
        int *ldb,
        double *beta,
        double *c,
        int *ldc);
    double** alloc_matrix(int N);
    void free_matrix(double** mat);
#ifdef __cplusplus
}
#endif
