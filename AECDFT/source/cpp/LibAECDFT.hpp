#ifdef __cplusplus
extern "C"
{
#endif
    double *alloc_dvector(int N);
    void free_dvector(double *v);
    void fill_grid(int N, double* X, double* R, double XMin, double DX);

#ifdef __cplusplus
}
#endif