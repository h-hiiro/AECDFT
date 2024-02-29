#include <cstdio>
#include <cmath>
#include "LibAECDFT.hpp"

bool calc_TFPotential(int N, double *X, double *VTF, double InitialDiff_left, double InitialDiff_right, double Threshold)
{
    double diff_left = InitialDiff_left;
    double diff_right = InitialDiff_right;
    double diff_center = 0.5 * (diff_left + diff_right);

    double edge_left = TF_evolution(N, X, VTF, diff_left);
    double edge_right = TF_evolution(N, X, VTF, diff_right);
    if (!(edge_left < 0 && 0 < edge_right))
    {
        printf("[C.calc_TFPotential] initial edge error: %f, %f\n", edge_left, edge_right);
        return false;
    }

    double edge_center = TF_evolution(N, X, VTF, diff_center);
    while (edge_center < 0 || edge_center > Threshold)
    {
        if (edge_center < 0)
        {
            diff_left = diff_center;
            edge_left = TF_evolution(N, X, VTF, diff_left);
        }
        else
        {
            diff_right = diff_center;
            edge_right = TF_evolution(N, X, VTF, diff_right);
        }
        diff_center = 0.5 * (diff_left + diff_right);
        edge_center = TF_evolution(N, X, VTF, diff_center);
        printf("[C.calc_TFPotential] Diff %12.8f Edge %10.6f\n", diff_center, edge_center);
    }
    return true;
}

double TF_evolution(int N, double *X, double *VTF, double initial_diff)
{
    VTF[0] = 1.0;
    double diff = initial_diff;
    double dx = X[1] - X[0];
    VTF[1] = VTF[0] + diff * dx;

    int i;
    for (i = 2; i < N; i++)
    {
        dx = X[i] - X[i - 1];
        double ddiff;
        if (VTF[i - 1] > 0)
        {
            ddiff = pow(VTF[i - 1], 1.5) / sqrt(X[i - 1]);
            diff += ddiff * dx;
        }
        else
        {
            ddiff = 0.0;
            diff = 0.0;
        }
        VTF[i] = VTF[i - 1] + diff * dx;
    }
    return VTF[N - 1];
}

void calc_TFDensity(int N, double* R, double* VTF, int Z, double* V, double* rho){
    int i;
    double rho_total=0.0;
    for(i=0; i<N; i++){
        V[i]=-double(Z)/R[i]*VTF[i];
        double kF=sqrt(-2.0*V[i]);
        rho[i]=pow(kF, 3.0)/3.0/pow(M_PI, 2.0);
        if(i>0){
            rho_total+=4.0*M_PI*pow(R[i], 2.0)*rho[i]*(R[i]-R[i-1]);
        }
    }
    printf("[C.calc_TFDensity] Rho_total %.3f Z %d\n", rho_total, Z);
}