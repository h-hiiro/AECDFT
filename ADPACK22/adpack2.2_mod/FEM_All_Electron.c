/**********************************************************************
  FEM_All_Electron.c:

   FEM_All_Electron.c is a subroutine to perform the self-consistent
   calculation, using a finite element basis, of an atomic Kohn-Sham 
   equation including all electron.s

  Log of FEM_All_Electron.c:

     10/Dec/2007  Released by T.Ozaki

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "adpack.h"
 
#ifndef ___INTEGER_definition___
typedef int INTEGER; /* for fortran integer */
#define ___INTEGER_definition___ 
#endif

#ifndef ___DOUBLE_definition___
typedef long double DOUBLE; 
#define ___DOUBLE_definition___ 
#endif


#define  measure_time   0

#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))


static void GLEQ(long int n, long double **a, long double *x);

static void diagonalize(INTEGER N0, long int NumMul,
                 long double **H, long double **S, 
                 long double *E, long double **V);


static void EigenSolver(long int SCF_iter, long int reuse_flag,
                        long int N0, long int L, long int NumMul,
                        long double **H, long double **S, 
                        long double *E, long double **V);


static void Mat_Vecs(long double **A, long int NumMul, long int N0, long double **v0, long double **v1);
static void Mat_Vec(long double **A, long int N0, long double *v0, long double *v1);

static void InvMat_Vec(long double **SL, long double **SU,
                       long int N0, long double *v0, long double *v1);

static long double rnd(long double width);

static void Set_Hamiltonian( long int N, long int L, long double *Rho, 
                             long double *VHartree, long double *Vxc,
                             long double **H,
                             long double **Hkin,
                             long double **Hee,
                             long double **Hec,
                             long double **Hxc,
                             long double **S);


static void Eigen_HH(double **ac, double *ko, int n, int EVmax);
static void lapack_dstevx1(INTEGER N, INTEGER EVmax, double *D, double *E, double *W, double **ev);


static long double Calc_Rho(long int N, long double **EVAL, long double ***EVEC, 
                            long double *Rho, long double ***DM);
static void Calc_VHartree(long int N, long double *Rho, long double *VHartree);
static long double Mixing_Rho(long int N, long int SCF, long double **Rho, long double **Drho);
static void Calc_Vxc(long int N, long double *Rho, long double *Vxc, int XC_flag);

static void Out_AllFEMLOG(long double *Ukin,    long double *Uee,    long double *Uec,
                          long double *Uxc,     long double *Uele,   long double *Utot,
                          long double *Ux,      long double *Ucorr,  long double *Ukin_x,
                          long double *Ukin_c,  long double *Virial1,long double *Virial2,
                          long double **EVAL,   long double ***EVEC, long double *Rho);
 
void dstevx_(char *JOBZ, char *RANGE, INTEGER *N, double *D, double *E, 
	     double *VL, double *VU, INTEGER *IL, INTEGER *IU, double *ABSTOL,
	     INTEGER *M, double *W, double *Z, INTEGER *LDZ, double *WORK, 
	     INTEGER *IWORK, INTEGER *IFAIL, INTEGER *INFO);

void dsygvx_(INTEGER *itype, char *jobz, char *range, char *uplo, INTEGER *n, 
	     double *a, INTEGER *lda, double *b, 
	     INTEGER *ldb, double *vl, double *vu, INTEGER *il, INTEGER *iu, 
	     double *abstol, INTEGER *m, double *w, double *z__, 
	     INTEGER *ldz, double *work, INTEGER *lwork, INTEGER *iwork, 
	     INTEGER *ifail, INTEGER *info);



void FEM_All_Electron()
{
  int i,j,n,l;
  int XC_flag;
  long int SCF_OK,SCF_iter;
  long int NumMul;
  long int reuse_flag;
  long int N,L,hsize;
  long int *NumEachL;
  long double **H,**S;
  long double ***Hkin;
  long double **Hee;
  long double **Hec;
  long double **Hxc;
  long double ***DM;
  long double **Rho;
  long double **Drho;
  long double *VHartree,*Vxc;
  long double ***EVEC,**EVAL;
  long double dUele,Uele0;
  long double Utot,Ukin,Ukin_xc;
  long double Uee,Uec,Uxc,Uele;
  long double Ux,Ucorr,Ukin_x,Ukin_c;
  long double Virial1,Virial2;
  long double NormRD;

  /**********************************
          allocation of arrays
  **********************************/

  N = (long int)Grid_Num;
  
  H = (long double**)malloc(sizeof(long double*)*2*N);
  for (i=0; i<2*N; i++){
    H[i] = (long double*)malloc(sizeof(long double)*6);
    for (j=0; j<6; j++){
      H[i][j] = 0.0L;
    }
  }

  Hkin = (long double***)malloc(sizeof(long double**)*(Occupied_Lmax+1));
  for (l=0; l<=Occupied_Lmax; l++){
    Hkin[l] = (long double**)malloc(sizeof(long double*)*2*N);
    for (i=0; i<2*N; i++){
      Hkin[l][i] = (long double*)malloc(sizeof(long double)*6);
      for (j=0; j<6; j++){
	Hkin[l][i][j] = 0.0L;
      }
    }
  }

  Hee = (long double**)malloc(sizeof(long double*)*2*N);
  for (i=0; i<2*N; i++){
    Hee[i] = (long double*)malloc(sizeof(long double)*6);
    for (j=0; j<6; j++){
      Hee[i][j] = 0.0L;
    }
  }

  Hec = (long double**)malloc(sizeof(long double*)*2*N);
  for (i=0; i<2*N; i++){
    Hec[i] = (long double*)malloc(sizeof(long double)*6);
    for (j=0; j<6; j++){
      Hec[i][j] = 0.0L;
    }
  }

  Hxc = (long double**)malloc(sizeof(long double*)*2*N);
  for (i=0; i<2*N; i++){
    Hxc[i] = (long double*)malloc(sizeof(long double)*6);
    for (j=0; j<6; j++){
      Hxc[i][j] = 0.0L;
    }
  }

  DM = (long double***)malloc(sizeof(long double**)*(Occupied_Lmax+1));
  for (l=0; l<=Occupied_Lmax; l++){
    DM[l] = (long double**)malloc(sizeof(long double*)*2*N);
    for (i=0; i<2*N; i++){
      DM[l][i] = (long double*)malloc(sizeof(long double)*6);
      for (j=0; j<6; j++){
	DM[l][i][j] = 0.0L;
      }
    }
  }

  S = (long double**)malloc(sizeof(long double*)*2*N);
  for (i=0; i<2*N; i++){
    S[i] = (long double*)malloc(sizeof(long double)*6);
    for (j=0; j<6; j++){
      S[i][j] = 0.0L;
    }
  }

  hsize = Num_Mixing_pDM + 3;

  Rho = (long double**)malloc(sizeof(long double*)*hsize);
  for (i=0; i<hsize; i++){
    Rho[i] = (long double*)malloc(sizeof(long double)*2*N);
    for (j=0; j<2*N; j++) Rho[i][j] = 0.0L;
  }

  Drho = (long double**)malloc(sizeof(long double*)*hsize);
  for (i=0; i<hsize; i++){
    Drho[i] = (long double*)malloc(sizeof(long double)*2*N);
    for (j=0; j<2*N; j++) Drho[i][j] = 0.0L;
  }

  VHartree = (long double*)malloc(sizeof(long double)*2*N);
  Vxc = (long double*)malloc(sizeof(long double)*2*N);
  for (i=0; i<2*N; i++){
    VHartree[i] = 0.0L;
    Vxc[i] = 0.0L;
  }

  NumMul = 9;
  if ( (2*N)<NumMul ) NumMul = 2*N;

  EVEC = (long double***)malloc(sizeof(long double**)*(Occupied_Lmax+1));
  for (L=0; L<(Occupied_Lmax+1); L++){
    EVEC[L] = (long double**)malloc(sizeof(long double*)*NumMul);
    for (i=0; i<NumMul; i++){
      EVEC[L][i] = (long double*)malloc(sizeof(long double)*2*N);
      for (j=0; j<2*N; j++) EVEC[L][i][j] = 0.0L;
    }
  }

  EVAL = (long double**)malloc(sizeof(long double*)*(Occupied_Lmax+1));
  for (L=0; L<(Occupied_Lmax+1); L++){
    EVAL[L] = (long double*)malloc(sizeof(long double)*NumMul);
  }

  NumEachL = (long int*)malloc(sizeof(long int)*(Occupied_Lmax+1));

  /************************************
    calculate the number of states 
    for each L-component
  *************************************/

  for (l=0; l<(Occupied_Lmax+1); l++) NumEachL[l] = 0;
  for (n=1; n<=max_ocupied_N; n++){
    for (l=0; l<n; l++){
      if (0.0<OcpN[0][0][n][l])  NumEachL[l]++;
    }
  }

  /************************************
                 SCF loop
  *************************************/

  SCF_OK = 0;
  SCF_iter = 1;
  XC_flag = 1;  /* exchange-correlation potential */
  Uele0 = 1000.0L;
  dUele = 1000.0L;

  do {

    if  (fabsl(dUele)<(1.0e-3L*(long double)AtomNum)) 
      reuse_flag = 1;
    else
      reuse_flag = 0;

    for (L=0; L<=Occupied_Lmax; L++){  

      Set_Hamiltonian(N,L,Rho[0],VHartree,Vxc,H,Hkin[L],Hee,Hec,Hxc,S);

      if (NumEachL[L]!=0){ 

        EigenSolver(SCF_iter,reuse_flag,N,L,NumEachL[L],H,S,EVAL[L],EVEC[L]);

        /*
        diagonalize(N,NumMul,H,S,EVAL[L],EVEC[L]);
        */
      }
    }

    Uele = Calc_Rho(N,EVAL,EVEC,Rho[0],DM);
    NormRD = Mixing_Rho(N,SCF_iter,Rho,Drho);
    Calc_VHartree(N,Rho[0],VHartree);
    Calc_Vxc(N,Rho[0],Vxc,XC_flag);

    dUele = Uele - Uele0;
    Uele0 = Uele; 
    Ukin= 0.0L; 
    Uee = 0.0L; 
    Uec = 0.0L; 
    Uxc = 0.0L; 

    for (L=0; L<=Occupied_Lmax; L++){  
      for (i=0; i<2*N; i++){
        for (j=0; j<6; j++){
	  Ukin  += DM[L][i][j]*Hkin[L][i][j];
	  Uee   += 0.5L*DM[L][i][j]*Hee[i][j];
	  Uec   += DM[L][i][j]*Hec[i][j];
	  Uxc   += DM[L][i][j]*Hxc[i][j];
        }
      }
    }

    printf("SCF=%4d Uele=%20.20Lf NormRD=%40.30Lf\n",SCF_iter,Uele,NormRD);
     
    if (SCF_MAX<SCF_iter || NormRD<SCF_criterion) SCF_OK = 1;

    SCF_iter++;

  } while (SCF_OK==0);

  /**********************************
      calculate the total energy
  **********************************/

  XC_flag = 0; /* energy density */  
  Calc_Vxc(N,Rho[0],Vxc,XC_flag);

  for (L=0; L<=Occupied_Lmax; L++){  
    Set_Hamiltonian(N,L,Rho[0],VHartree,Vxc,H,Hkin[L],Hee,Hec,Hxc,S);
  }

  Ukin = 0.0L; 
  Uee  = 0.0L; 
  Uec  = 0.0L; 
  Uxc  = 0.0L; 

  for (L=0; L<=Occupied_Lmax; L++){  
    for (i=0; i<2*N; i++){
      for (j=0; j<6; j++){
        Ukin  += DM[L][i][j]*Hkin[L][i][j];
        Uee   += 0.5L*DM[L][i][j]*Hee[i][j];
        Uec   += DM[L][i][j]*Hec[i][j];
        Uxc   += DM[L][i][j]*Hxc[i][j];
      }
    }
  }

  /***************************************************
                  The exchange energy
  ***************************************************/

  XC_flag = 4;
  Calc_Vxc(N,Rho[0],Vxc,XC_flag);

  for (L=0; L<=Occupied_Lmax; L++){  
    Set_Hamiltonian(N,L,Rho[0],VHartree,Vxc,H,Hkin[L],Hee,Hec,Hxc,S);
  }

  Ux = 0.0L;

  for (L=0; L<=Occupied_Lmax; L++){  
    for (i=0; i<2*N; i++){
      for (j=0; j<6; j++){
        Ux += DM[L][i][j]*Hxc[i][j];
      }
    }
  }

  /***************************************************
                The correlation energy
  ***************************************************/

  XC_flag = 5;
  Calc_Vxc(N,Rho[0],Vxc,XC_flag);

  for (L=0; L<=Occupied_Lmax; L++){  
    Set_Hamiltonian(N,L,Rho[0],VHartree,Vxc,H,Hkin[L],Hee,Hec,Hxc,S);
  }

  Ucorr = 0.0L;

  for (L=0; L<=Occupied_Lmax; L++){  
    for (i=0; i<2*N; i++){
      for (j=0; j<6; j++){
        Ucorr += DM[L][i][j]*Hxc[i][j];
      }
    }
  }

  /***************************************************
      The kinetic part in the exchange energy
  ***************************************************/

  XC_flag = 6; /* energy density of the kinetic part */  
  Calc_Vxc(N,Rho[0],Vxc,XC_flag);

  for (L=0; L<=Occupied_Lmax; L++){  
    Set_Hamiltonian(N,L,Rho[0],VHartree,Vxc,H,Hkin[L],Hee,Hec,Hxc,S);
  }

  Ukin_x = 0.0L;

  for (L=0; L<=Occupied_Lmax; L++){  
    for (i=0; i<2*N; i++){
      for (j=0; j<6; j++){
        Ukin_x += DM[L][i][j]*Hxc[i][j];
      }
    }
  }

  /***************************************************
      The kinetic part in the correlation energy
  ***************************************************/

  XC_flag = 7; /* energy density of the kinetic part */  
  Calc_Vxc(N,Rho[0],Vxc,XC_flag);

  for (L=0; L<=Occupied_Lmax; L++){  
    Set_Hamiltonian(N,L,Rho[0],VHartree,Vxc,H,Hkin[L],Hee,Hec,Hxc,S);
  }

  Ukin_c = 0.0L;

  for (L=0; L<=Occupied_Lmax; L++){  
    for (i=0; i<2*N; i++){
      for (j=0; j<6; j++){
        Ukin_c += DM[L][i][j]*Hxc[i][j];
      }
    }
  }

  /********************************
     show the energy contributions
  *********************************/

  Utot = Ukin+ Uee + Uec + Uxc;

  printf("\n");
  printf("<ALL>  **** Energies of atom ****\n");
  printf("<ALL>  Etot    = %22.15Lf (Hartree)\n",Utot);
  printf("<ALL>  Etot    = Ekin + EHart + Eec + Exc\n\n");

  printf("<ALL>  Ekin    = %22.15Lf (Hartree)\n",Ukin);
  printf("<ALL>  EHart   = %22.15Lf (Hartree)\n",Uee);
  printf("<ALL>  Eec     = %22.15Lf (Hartree)\n",Uec);
  printf("<ALL>  Exc     = %22.15Lf (Hartree)\n\n",Ux+Ucorr);
   
  printf("<ALL>  Exc     = Ex + Ecorr = (Ex-Ekin_x) + (Ecorr-Ekin_c) + Ekin_x + Ekin_c\n");
  printf("<ALL>  Ex      = %22.15Lf (Hartree)\n",Ux);
  printf("<ALL>  Ecorr   = %22.15Lf (Hartree)\n",Ucorr);
  printf("<ALL>  Ekin_x  = %22.15Lf (Hartree)\n",Ukin_x);
  printf("<ALL>  Ekin_c  = %22.15Lf (Hartree)\n\n",Ukin_c);
  printf("<ALL>  Eeigen  = %22.15Lf (Hartree)\n\n",Uele);

  Virial1 = 2.0*(Ukin+Ukin_x+Ukin_c)+(Uee+Uxc+Uec-Ukin_x-Ukin_c);
  Virial2 = (Uee+Uxc+Uec-Ukin_x-Ukin_c)/(Ukin+Ukin_x+Ukin_c);

  printf("<ALL>  Virial theorem  2*(Ekin+Ekin_x+Ekin_c)+(EHart+Eec+Exc-Ekin_x-Ekin_c) = %+18.15Lf\n",Virial1);
  printf("<ALL>  Virial theorem (EHart+Eec+Exc-Ekin_x-Ekin_c)/(Ekin+Ekin_x+Ekin_c)    = %+18.15Lf\n\n",Virial2); 

  Out_AllFEMLOG(&Ukin,&Uee,&Uec,&Uxc,&Uele,&Utot,&Ux,&Ucorr,&Ukin_x,&Ukin_c,&Virial1,&Virial2,EVAL,EVEC,Rho[0]);

  /**********************************
            freeing of arrays 
  **********************************/

  for (i=0; i<2*N; i++){
    free(H[i]);
  }
  free(H);

  for (l=0; l<=Occupied_Lmax; l++){
    for (i=0; i<2*N; i++){
      free(Hkin[l][i]);
    }
    free(Hkin[l]);
  }
  free(Hkin);

  for (i=0; i<2*N; i++){
    free(Hee[i]);
  }
  free(Hee);

  for (i=0; i<2*N; i++){
    free(Hec[i]);
  }
  free(Hec);

  for (i=0; i<2*N; i++){
    free(Hxc[i]);
  }
  free(Hxc);

  for (l=0; l<=Occupied_Lmax; l++){
    for (i=0; i<2*N; i++){
      free(DM[l][i]);
    }
    free(DM[l]);
  }
  free(DM);

  for (i=0; i<2*N; i++){
    free(S[i]);
  }
  free(S);

  for (i=0; i<hsize; i++){
    free(Rho[i]);
  }
  free(Rho);

  for (i=0; i<hsize; i++){
    free(Drho[i]);
  }
  free(Drho);

  free(VHartree);
  free(Vxc);

  for (L=0; L<(Occupied_Lmax+1); L++){
    for (i=0; i<NumMul; i++){
      free(EVEC[L][i]);
    }
    free(EVEC[L]);
  }
  free(EVEC);

  for (L=0; L<(Occupied_Lmax+1); L++){
    free(EVAL[L]);
  }
  free(EVAL);

  free(NumEachL);

}



static long double Mixing_Rho(long int N, long int SCF, long double **Rho, long double **Drho)
{
  long int i,j,n,m,Np;
  long double sum,sum0,tmp,weight,d;
  long double **A,*X;

  /******************************************
      simple mixing 
  ******************************************/

  if (Mixing_switch==0){

    /* calculate Drho0 */

    for (i=0; i<2*N; i++){
      Drho[0][i] = Rho[0][i] - Rho[1][i];
    }

    /* SCF<30 */

    if (SCF<30){
      for (i=0; i<2*N; i++){
	tmp = 0.5L*Rho[0][i] + 0.5L*Rho[1][i];
	Rho[0][i] = tmp;
      }
    }

    /* SCF<60 */

    else if (SCF<60){
      for (i=0; i<2*N; i++){
	tmp = 0.20L*Rho[0][i] + 0.80L*Rho[1][i];
	Rho[0][i] = tmp;
      }
    }

    /* else */

    else{
      for (i=0; i<2*N; i++){
	tmp = 0.01L*Rho[0][i] + 0.99L*Rho[1][i];
	Rho[0][i] = tmp;
      }
    }

    /* shift Rho */

    for (n=1; 0<n; n--){
      for (i=0; i<2*N; i++){
	Rho[n][i] = Rho[n-1][i];
      }
    }

  } /* end of simple mixing */

  /******************************************
      Pulay mixing 
  ******************************************/

  else if (Mixing_switch==2){

    /* simple */

    if (SCF<(long int)Pulay_SCF){

      /* calculate Drho0 */

      for (i=0; i<2*N; i++){
	Drho[0][i] = Rho[0][i] - Rho[1][i];
      }

      /* simple mixing */

      for (i=0; i<2*N; i++){
	tmp = 0.5L*Rho[0][i] + 0.5L*Rho[1][i];
	Rho[0][i] = tmp;
      }

      /* shift Rho and Drho */

      for (n=2; 0<n; n--){
	for (i=0; i<2*N; i++){
	  Rho[n][i]  = Rho[n-1][i];
	  Drho[n][i] = Drho[n-1][i];
	}
      }

    }

    /* Pulay */
  
    else {

      /* allocate arrays */

      A = (long double**)malloc(sizeof(long double*)*(Num_Mixing_pDM+2));
      for (i=0; i<(Num_Mixing_pDM+2); i++){
	A[i] = (long double*)malloc(sizeof(long double)*(Num_Mixing_pDM+2));
      }

      X = (long double*)malloc(sizeof(long double)*(Num_Mixing_pDM+2));

      /* calculate Drho0 */

      for (i=0; i<2*N; i++){
	Drho[0][i] = Rho[0][i] - Rho[1][i];
      }

      /* calculate the norm matrix */

      Np = SCF - (long int)Pulay_SCF + 2;
      if (Num_Mixing_pDM<Np) Np = Num_Mixing_pDM;

      d = (long double)Grid_Xmax/(long double)(N-1);

      for (n=0; n<Np; n++){
	for (m=0; m<Np; m++){

	  sum = 0.0L;  
	  for (i=0; i<2*N; i++){

            weight = (long double)(i/2)*d;
            weight = weight*weight;

	    sum += Drho[n][i]*Drho[m][i]*weight;
	  }

	  A[n][m] = sum;
	} 
      } 

      for (n=0; n<Np; n++){
	A[Np][n] = 1.0L;
	A[n][Np] = 1.0L;
      }
      A[Np][Np] = 0.0L;

      for (n=0; n<Np; n++){
	A[n][Np+1] = 0.0L;
      }
      A[Np][Np+1] = 1.0L;

      /* solve the linear equation */

      GLEQ(Np,A,X);

      /* construct an optimized Rho */

      for (i=0; i<2*N; i++){

	sum = 0.0L;  
        sum0 = 0.0L;
 
	for (n=0; n<Np; n++){
	  sum  += X[n]*Rho[n+1][i];
	  sum0 += X[n]*Drho[n][i];
	}

	Rho[0][i] = sum + 0.1L*sum0;
      }

      /* shift Rho and Drho */

      for (n=Num_Mixing_pDM; 0<n; n--){
	for (i=0; i<2*N; i++){
	  Rho[n][i]  = Rho[n-1][i];
	  Drho[n][i] = Drho[n-1][i];
	}
      }

      /* deallocate arrays */

      for (i=0; i<(Num_Mixing_pDM+2); i++){
	free(A[i]);
      }
      free(A);

      free(X);

    } 

  } /* end of Pulay */

  else{
    printf("not suported\n");
    exit(0);
  }

  /* calculate NormRD */

  sum = 0.0L;
  for (i=0; i<2*N; i++){
    sum += Drho[0][i]*Drho[0][i];
  }

  return sum;

}


static void GLEQ(long int n, long double **a, long double *x)
{
  /****************************************************
                   From 0 to n,  Ax = b
              The (n+1) column of a[][] is b.
  ****************************************************/

  long int i,j,k,max_i,po;
  long double max,dum1,dum2,w;

  for (i=0; i<=n; i++){

    /****************************************************
         choose the maximum element of the subspace.
    ****************************************************/

    po = 0;
    max = fabsl(a[i][i]);

    for (j=i+1; j<=n; j++){
      if (max<fabsl(a[j][i])){
	po = 1;
	max = fabsl(a[j][i]);
	max_i = j;
      }         
    }  
  
    if (po==1){
      for (j=i; j<=(n+1); j++){
	dum1 = a[i][j];
	dum2 = a[max_i][j];
	a[i][j]     = dum2;
	a[max_i][j] = dum1;
      }          
    }

    /****************************************************
                       Gauss's method 
    ****************************************************/

    w = 1.0L/a[i][i];
    for (j=i; j<=(n+1); j++){
      a[i][j] = a[i][j]*w;
    }

    for (j=(i+1); j<=n; j++){
      w = a[j][i];
      for (k=i; k<=(n+1); k++){
	a[j][k] = a[j][k] - a[i][k]*w;
      } 
    }
  }

  /****************************************************
                       Inverting
  ****************************************************/
 
  x[n] = a[n][n+1];
  for (i=(n-1); 0<=i; i--){
    dum1 = a[i][n+1];
    for (j=n; (i+1)<=j; j--){
      dum1 = dum1 - a[i][j]*x[j];
    }
    x[i] = dum1;
  }

}






static void Calc_VHartree(long int N, long double *Rho, long double *VHartree)
{

  static long int i,k;
  static long int N2;
  static long double *Rho0,*Rho1;
  static long double d,d4,d6,x,r,p,q,tmp0,tmp1;

  N2 = 2*N;

  /* allocation of arrays */

  Rho0 = (long double*)malloc(sizeof(long double)*N);
  Rho1 = (long double*)malloc(sizeof(long double)*N);

  /* mapping of Rho into Rho0 and Rho1 */

  for (i=0; i<N; i++){
    Rho0[i] = Rho[i*2];    
  }

  Rho1[N-1] = (Rho[2*N-1] - 0.5L*Rho0[N-1])/0.125L;

  for (i=N-2; 0<=i; i--){
    Rho1[i] = (Rho[i*2+1] - 0.5L*Rho0[i] - 0.5L*Rho0[i+1] + 0.125L*Rho1[i+1])/0.125L;
  }

  for (i=0; i<2*N; i++){
    VHartree[i] = 0.0L;
  }

  /* Hatree potential at positions where FEM basis functions are located */

  d = (long double)Grid_Xmax/(long double)(N-1);
  d4 = d*d*d*d;
  d6 = d4*d*d;

  for (k=0; k<N; k++){

    p = (long double)k;
    x = (long double)k*d;    
    r = x*x;

    /* A1, B1, A2, and B2 */

    if (1<=k){

      tmp0 = 0.0L;

      for (i=0; i<=(k-1); i++){
      
        q = (long double)i; 
 
        if (i==0){
          tmp0 = Rho0[0]/72.0L + Rho1[0]/252.0L;
        }
        else {
          tmp0 +=  Rho0[i]*(9.0L*q + 56.0L*q*q*q + 42.0L*q*q*q*q*q)/42.0L
                 + Rho1[i]*(1.0L + 24.0L*q*q + 42.0L*q*q*q*q)/126.0L;
        }
      }

      VHartree[2*k] = 8.0L*PI*1.0L/r*d6*tmp0;
    }

    /* A3 and B3 */

    if (k!=0){
      tmp0 = d6*(-7.0L+54.0L*p-180.0L*p*p+336.0L*p*p*p-378.0L*p*p*p*p+252.0L*p*p*p*p*p)/504.0L;
      tmp1 =-d6*(-2.0L + 3.0L*p*(5.0L + 2.0L*p*(-8.0L + 7.0L*p*(2.0L + (-2.0L + p)*p))))/504.0L;
      VHartree[2*k] += 8.0L*PI*1.0L/r*(Rho0[k]*tmp0 + Rho1[k]*tmp1);
    }

    /* A4 and B4 */

    tmp0 = d4*(5.0L + 28.0L*p + 63.0L*p*p + 70.0L*p*p*p)/140.0L;
    tmp1 = d4*(4.0L + 21.0L*p + 42.0L*p*p + 35.0L*p*p*p)/420.0L;
    VHartree[2*k] += 8.0L*PI*(Rho0[k]*tmp0 + Rho1[k]*tmp1);

    /* A5 and B5 */

    tmp0 = 0.0L;
    for (i=k+1; i<=(N-1); i++){
      q = (long double)i; 
      tmp0 +=  Rho0[i]*(2.0L*q + 5.0L*q*q*q)/5.0L
             + Rho1[i]*(2.0L + 21.0L*q*q)/105.0L;
    }

    VHartree[2*k] += 8.0L*PI*d4*tmp0;
  }

  /* Hatree potential at positions between two FEM basis functions */

  for (k=0; k<N; k++){

    p = (long double)k;
    x = (long double)k*d + 0.5L*d;    
    r = x*x;

    /* C0 and D0 */

    if (k==0){
      tmp0 = Rho0[0]*29.0L/18432.0L + Rho1[0]*23.0L/64512.0L;
      VHartree[1] = 8.0L*PI*1.0/r*d6*tmp0;
    }

    /* C1, D1, C2, and D2 */

    else {

      tmp0 = 0.0L;
      for (i=0; i<=(k-1); i++){
      
        q = (long double)i; 
 
        if (i==0){
          tmp0 = Rho0[0]/72.0L + Rho1[0]/252.0L;
        }
        else {
          tmp0 +=  Rho0[i]*(9.0L*q + 56.0L*q*q*q + 42.0L*q*q*q*q*q)/42.0L
                 + Rho1[i]*(1.0L + 24.0L*q*q + 42.0L*q*q*q*q)/126.0L;
        }

        VHartree[2*k+1] = 8.0L*PI*1.0L/r*d6*tmp0;
      }
    }
    
    /* C3 and D3 */

    if (k!=0){
      tmp0 = d6*(-1589.0L+16326.0L*p-33120.0L*p*p+122304.0L*p*p*p
                 -38304.0L*p*p*p*p + 116928.0L*p*p*p*p*p)/129024.0L;
  
      tmp1 = d6*(186.0L-1095.0L*p+5024.0L*p*p-4704.0L*p*p*p
                 +10752.0L*p*p*p*p-1120.0L*p*p*p*p*p)/43008.0L;
      VHartree[2*k+1] += 8.0L*PI*1.0L/r*(Rho0[k]*tmp0 + Rho1[k]*tmp1);
    }

    /* C4 and D4 */

    tmp0 = d4*(115.0L + 518.0L*p + 798.0L*p*p + 420.0L*p*p*p)/4480.0L;  
    tmp1 = d4*(99.0L + 441.0L*p + 672.0L*p*p + 350.0L*p*p*p)/13440.0L;
    VHartree[2*k+1] += 8.0L*PI*(Rho0[k]*tmp0 + Rho1[k]*tmp1);

    /* C5 and D5 */

    if (k<(N-1)){
      tmp0 = d6*(133.0L+1530.0L*p+7200.0L*p*p+17472.0L*p*p*p
                +22176.0L*p*p*p*p+12096.0L*p*p*p*p*p)/129024.0L;
      tmp1 = -d6*(35.0L+405.0L*p+1920.0L*p*p+4704.0L*p*p*p
                +6048.0L*p*p*p*p + 3360.0L*p*p*p*p*p)/129024.0L;
      VHartree[2*k+1] += 8.0L*PI*1.0L/r*(Rho0[k+1]*tmp0 + Rho1[k+1]*tmp1);
    }

    /* C6 and D6 */

    if (k<(N-1)){
      tmp0 = d4*(6247.0L + 15050.0L*p + 12978.0L*p*p + 4060.0L*p*p*p)/4480.0L;
      tmp1 = d4*(2964.0L + 5523.0L*p + 3066.0L*p*p + 350.0L*p*p*p)/13440.0L;
      VHartree[2*k+1] += 8.0L*PI*(Rho0[k+1]*tmp0 + Rho1[k+1]*tmp1);
    }

    /* C7 and D7 */

    tmp0 = 0.0L;
    for (i=k+2; i<=(N-1); i++){
      q = (long double)i; 
      tmp0 +=  Rho0[i]*(2.0L*q + 5.0L*q*q*q)/5.0L
             + Rho1[i]*(2.0L + 21.0L*q*q)/105.0L;
    }
    VHartree[2*k+1] += 8.0L*PI*d4*tmp0;

  }

  /* freeing of arrays */

  free(Rho0);
  free(Rho1);
}



static void Calc_Vxc(long int N, long double *Rho, long double *Vxc, int XC_flag)
{
  static long int i,xc;
  static long double rho;
  static long double alpha,dum;

  xc = 3;  

  /****************************************
    xc = 1;
    X-alpha potential
  ****************************************/

  if (xc==1){

    alpha = 0.70L;

    /* energy density */

    if (XC_flag==0){
      for (i=0; i<2*N; i++){
	rho = Rho[i];
	dum = 3.0L/PI*rho;
	Vxc[i] = -9.0L/8.0L*alpha*powl(dum,1.0L/3.0L);
      }
    }

    /* potential */

    else if (XC_flag==1){
      for (i=0; i<2*N; i++){
	rho = Rho[i];
	dum = 3.0L/PI*rho;
	Vxc[i] = -3.0L/2.0L*alpha*powl(dum,1.0L/3.0L);
      }
    }

    /* energy density of the kinetic part */

    else if (XC_flag==2){
      for (i=0; i<2*N; i++){
	Vxc[i] = 0.0L;
      }
    }

    /* energy density - potential */

    else if (XC_flag==3){
      for (i=0; i<2*N; i++){
	rho = Rho[i];
	dum = 3.0L/PI*rho;
	Vxc[i] = -9.0L/8.0L*alpha*powl(dum,1.0L/3.0L) - (-3.0L/2.0L*alpha*powl(dum,1.0L/3.0L));
      }
    }

  }

  /****************************************
    xc = 2;
    LDA constructed by Ceperly and Alder,
    and parametrized by Perdew and Zunger
  ****************************************/

  else if (xc==2){

    static long double coe,rs,Ex,dEx,dum,Ec,dEc,tmp;

    coe = powl(3.0L/4.0L/PI,1.0L/3.0L);

    for (i=0; i<2*N; i++){

      rho = Rho[i];
      if (rho<0.0L) rho = 1.0e-40L;
      rs = coe*powl(rho,-1.0L/3.0L);
      tmp = 3.0L/4.0L*powl(9.0L/(4.0L*PI*PI),1.0L/3.0L);
      Ex = -tmp/rs;
      dEx = tmp/rs/rs;

      if (1.0L<=rs){
	dum = (1.0L+1.0529L*sqrtl(rs)+0.3334L*rs);
	Ec = -0.1423L/dum;
	dEc = 0.1423L/dum/dum*(1.0529L*0.5L/sqrtl(rs)+0.3334L);
      }
      else{
	Ec = -0.0480L+0.0311L*logl(rs)-0.0116L*rs+0.0020L*rs*logl(rs);
	dEc = 0.0311L/rs + 0.0020L*logl(rs) - 0.0096L;
      }

      /* energy density */

      if (XC_flag==0){
	Vxc[i] = Ex + Ec;
      }

      /* potential */

      else if (XC_flag==1){
	Vxc[i] = Ex + Ec - 1.0L/3.0L*rs*(dEx + dEc);
      }

      /* energy density of the kinetic part */

      else if (XC_flag==2){
	Vxc[i] = 3.0L*(Ex + Ec - 1.0L/3.0L*rs*(dEx + dEc)) - 4.0L*(Ex + Ec);
      }      

      /* energy density - potential */

      else if (XC_flag==3){
	Vxc[i] = Ex + Ec - (Ex + Ec - 1.0L/3.0L*rs*(dEx + dEc));
      }      

    }
  }

  /*********************************************************
    xc = 3;
    LDA constructed by Ceperly and Alder, and parametrized
    by Vosko, Wilk, and Nusair (VWN).
  **********************************************************/

  else if (xc==3){

    static long double coe,rs,Ex,dEx,dum,Ec,dEc;
    static long double x,x0,X,X0,Q,A,b,c,tmp;

    coe = powl(3.0L/4.0L/PI,1.0L/3.0L);

    for (i=0; i<2*N; i++){

      rho = Rho[i];
      if (rho<0.0) rho = 1.0e-40L;
      rs = coe*powl(rho,-1.0L/3.0L);

      /* the exchange part */

      tmp = 3.0L/4.0L*powl(9.0L/(4.0L*PI*PI),1.0L/3.0L);
      Ex = -tmp/rs; 
      dEx = tmp/rs/rs;

      /* the correlation part */

      A = 0.0310907L;
      b = 3.72744L;
      c = 12.9352L;
      x0 = -0.10498L;
      X0 = x0*x0 + b*x0 + c;  

      x = sqrtl(rs);  
      X = x*x + b*x + c;
      Q = sqrtl(4.0L*c-b*b);
      
      Ec = A*( logl(x*x/X)
             + 2.0L*b/Q*atanl(Q/(2.0L*x+b)) 
             - b*x0/X0*(logl((x-x0)*(x-x0)/X) 
             + 2.0L*(b+2.0L*x0)/Q*atanl(Q/(2.0L*x+b)))
             );

      dEc = (A*((2.0L*c + b*x)/(c + rs + b*x) - (4.0L*b*x)/(b*b + Q*Q + 4.0L*rs + 4.0L*b*x) 
		- (b*x*x0*((-4.0L*(b + 2.0L*x0))/(b*b + Q*Q + 4.0L*rs + 4.0L*b*x) 
                + (2.0L*c + 2.0L*x*x0 + b*(x + x0))/((c + rs + b*x)*(x - x0))))/X0))/(2.0L*rs);

      /* energy density */

      if (XC_flag==0){
	Vxc[i] = Ex + Ec;
      }

      /* potential */

      else if (XC_flag==1){
	Vxc[i] = Ex + Ec - 1.0L/3.0L*rs*(dEx + dEc);
      }

      /* energy density of the kinetic part */

      else if (XC_flag==2){
	Vxc[i] = 3.0L*(Ex + Ec - 1.0L/3.0L*rs*(dEx + dEc)) - 4.0L*(Ex + Ec);
      }      

      /* energy density - potential */

      else if (XC_flag==3){
	Vxc[i] = Ex + Ec - (Ex + Ec - 1.0L/3.0L*rs*(dEx + dEc));
      }      

      /* energy density of exchange term */

      else if (XC_flag==4){
	Vxc[i] = Ex;
      }

      /* energy density of correlation term */

      else if (XC_flag==5){
	Vxc[i] = Ec;
      }

      /* energy density of the kinetic part in the exchange term */

      else if (XC_flag==6){
	Vxc[i] = 3.0L*(Ex - 1.0L/3.0L*rs*dEx) - 4.0L*Ex;
      }

      /* energy density of the kinetic part in the correlation term */

      else if (XC_flag==7){
	Vxc[i] = 3.0L*(Ec - 1.0L/3.0L*rs*dEc) - 4.0L*Ec;
      }

    }
  }

}







static long double Calc_Rho(long int N, long double **EVAL, long double ***EVEC, 
                            long double *Rho, long double ***DM)
{
  static int n,l,L0;
  static long int i,j;
  static int *NumEachL;
  static long double s00,s01,s10,s11,Uele;

  NumEachL = (int*)malloc(sizeof(int)*(Occupied_Lmax+1));

  /********************************
    calculate the charge density 
  ********************************/

  for (l=0; l<(Occupied_Lmax+1); l++) NumEachL[l] = 0;

  Uele = 0.0L;

  for (i=0; i<2*N; i++) Rho[i] = 0.0L;

  for (n=1; n<=max_ocupied_N; n++){
    for (l=0; l<n; l++){

      if (0.0<OcpN[0][0][n][l]){

        /* calculate the eigen energy */

        L0 = NumEachL[l];
        Uele += (long double)OcpN[0][0][n][l]*EVAL[l][L0];

        /* positions where FEM basis functions are located */
        /* 0, 2, 4,..., 2N-2 */

        for (i=0; i<N; i++){
          L0 = NumEachL[l];
          Rho[i*2] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2]*EVEC[l][L0][i*2];
	}

        /* positions between two FEM basis functions */
        /*  1, 3, 5,.., 2N-3, */

        s00 = 0.500L;
        s01 = 0.125L;
        s10 = 0.500L;
        s11 =-0.125L;

        for (i=0; i<N-1; i++){
          L0 = NumEachL[l];
          Rho[i*2+1] +=
	    (long double)OcpN[0][0][n][l]*(
                        s00*s00*EVEC[l][L0][i*2  ]*EVEC[l][L0][i*2  ] 
                      + s01*s01*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+1] 
                      + s10*s10*EVEC[l][L0][i*2+2]*EVEC[l][L0][i*2+2] 
                      + s11*s11*EVEC[l][L0][i*2+3]*EVEC[l][L0][i*2+3] 
                      + s00*s01*EVEC[l][L0][i*2  ]*EVEC[l][L0][i*2+1]
                      + s01*s00*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2  ]
                      + s10*s11*EVEC[l][L0][i*2+2]*EVEC[l][L0][i*2+3]
                      + s11*s10*EVEC[l][L0][i*2+3]*EVEC[l][L0][i*2+2]
                      + s00*s10*EVEC[l][L0][i*2  ]*EVEC[l][L0][i*2+2]
                      + s10*s00*EVEC[l][L0][i*2+2]*EVEC[l][L0][i*2  ]
                      + s00*s11*EVEC[l][L0][i*2  ]*EVEC[l][L0][i*2+3]
                      + s11*s00*EVEC[l][L0][i*2+3]*EVEC[l][L0][i*2  ]
                      + s01*s11*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+3]
                      + s11*s01*EVEC[l][L0][i*2+3]*EVEC[l][L0][i*2+1]
                      + s01*s10*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+2]
                      + s10*s01*EVEC[l][L0][i*2+2]*EVEC[l][L0][i*2+1]
                 );
	}

        /* The end point */
        /* 2N-1 */ 

        i = N-1;
        L0 = NumEachL[l];
        Rho[i*2+1] +=
	    (long double)OcpN[0][0][n][l]*(
                      s00*s00*EVEC[l][L0][i*2  ]*EVEC[l][L0][i*2  ] 
                    + s01*s01*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+1] 
                    + s00*s01*EVEC[l][L0][i*2  ]*EVEC[l][L0][i*2+1]
                    + s01*s00*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2  ]
                 );


        NumEachL[l]++;        
      }
    }
  }

  for (i=0; i<2*N; i++){
    Rho[i] /= (4.0L*PI); 
  }

  /********************************
     calculate the density matrix
  ********************************/

  for (l=0; l<(Occupied_Lmax+1); l++) NumEachL[l] = 0;

  for (l=0; l<=Occupied_Lmax; l++){
    for (i=0; i<2*N; i++){
      for (j=0; j<6; j++){
	DM[l][i][j] = 0.0L;
      }
    }
  }


  for (n=1; n<=max_ocupied_N; n++){
    for (l=0; l<n; l++){

      if (0.0L<OcpN[0][0][n][l]){

        L0 = NumEachL[l];

        for (i=0; i<N; i++){
  
          /* diagonal */       

          if (i==0){
            DM[l][i*2+0][0] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+0]*EVEC[l][L0][i*2+0];
            DM[l][i*2+0][1] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+0]*EVEC[l][L0][i*2+1];
            DM[l][i*2+1][0] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+0];
            DM[l][i*2+1][1] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+1];
	  }
          else{
            DM[l][i*2+0][2] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+0]*EVEC[l][L0][i*2+0];
            DM[l][i*2+0][3] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+0]*EVEC[l][L0][i*2+1];
            DM[l][i*2+1][2] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+0];
            DM[l][i*2+1][3] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+1];
	  }

          /* off-diagonal */       
    
          if (i==0){
            DM[l][i*2+0][2] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+0]*EVEC[l][L0][i*2+2];
            DM[l][i*2+0][3] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+0]*EVEC[l][L0][i*2+3];
            DM[l][i*2+1][2] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+2];
            DM[l][i*2+1][3] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+3];

            DM[l][(i+1)*2+0][0] = DM[l][i*2+0][2];
            DM[l][(i+1)*2+1][0] = DM[l][i*2+0][3];
            DM[l][(i+1)*2+0][1] = DM[l][i*2+1][2];
            DM[l][(i+1)*2+1][1] = DM[l][i*2+1][3];
	  }
          else if (i!=(N-1)){
            DM[l][i*2+0][4] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+0]*EVEC[l][L0][i*2+2];
            DM[l][i*2+0][5] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+0]*EVEC[l][L0][i*2+3];
            DM[l][i*2+1][4] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+2];
            DM[l][i*2+1][5] += (long double)OcpN[0][0][n][l]*EVEC[l][L0][i*2+1]*EVEC[l][L0][i*2+3];

            DM[l][(i+1)*2+0][0] = DM[l][i*2+0][4];
            DM[l][(i+1)*2+1][0] = DM[l][i*2+0][5];
            DM[l][(i+1)*2+0][1] = DM[l][i*2+1][4];
            DM[l][(i+1)*2+1][1] = DM[l][i*2+1][5];
          }
	}

        NumEachL[l]++;        
      }
    }
  }


  /* freeing of array */

  free(NumEachL);

  /* return Uele */

  return Uele;
}



static void Set_Hamiltonian(long int N, long int L, long double *Rho, 
                            long double *VHartree, long double *Vxc,
                            long double **H,
			    long double **Hkin,
			    long double **Hee,
			    long double **Hec,
			    long double **Hxc,
			    long double **S)
{
  static long int i,j,l;
  static long double l2,d,d6,q,fac,fac0,fac1;
  static long double q2,q3,q4,q5;
  static long double tmp0,tmp1;
  static long double *Ve0,*Ve1;

  /* allocation of arrays */

  Ve0 = (long double*)malloc(sizeof(long double)*N);
  Ve1 = (long double*)malloc(sizeof(long double)*N);

  for (i=0; i<N; i++){
    Ve0[i] = 0.0L;
    Ve1[i] = 0.0L;
  }
 
  /* step for the x-coordinate */

  d = (long double)Grid_Xmax/(long double)(N-1);

  /**************************************************************
                          kinetic terms
  **************************************************************/
  
  /* diagonal element, i=0, for the kinetic operator */

  Hkin[0][0] = d*d*15.0L/280.0L;
  Hkin[0][1] = d*d*7.0L/560.0L;
  Hkin[1][0] = d*d*7.0L/560.0L;
  Hkin[1][1] = d*d*11.0L/3360.0L;

  /* diagonal element for the kinetic operator */

  for (i=1; i<N; i++){
    q = (long double)i; 
    Hkin[i*2+0][2] = 3.0L*d*d*q*(6.0L + 7.0L*q*q)/35.0L;
    Hkin[i*2+0][3] = d*d*(1.0L + 6.0L*q*q)/40.0L;
    Hkin[i*2+1][2] = d*d*(1.0L + 6.0L*q*q)/40.0L;
    Hkin[i*2+1][3] = d*d*q*(3.0L + 7.0L*q*q)/105.0L;
  }
  
  /* off-diagonal element for the kinetic operator */

  for (i=0; i<(N-1); i++){

    q = (long double)i; 

    if (i==0) j = 2;
    else      j = 4;

    Hkin[i*2+0][j+0] = -3.0L*d*d*(5.0L + 24.0L*q + 42.0L*q*q + 28.0L*q*q*q)/280.0L;
    Hkin[i*2+0][j+1] = d*d*(-5.0L - 12.0L*q + 14.0L*q*q*q)/560.0L;
    Hkin[i*2+1][j+0] = -d*d*(7.0L + 30.0L*q + 42.0L*q*q + 14.0L*q*q*q)/560.0L; 
    Hkin[i*2+1][j+1] = -d*d*(11.0L + 36.0L*q + 42.0L*q*q + 28.0L*q*q*q)/3360.0L;

    Hkin[(i+1)*2+0][0] = Hkin[i*2+0][j+0];
    Hkin[(i+1)*2+1][0] = Hkin[i*2+0][j+1];
    Hkin[(i+1)*2+0][1] = Hkin[i*2+1][j+0];
    Hkin[(i+1)*2+1][1] = Hkin[i*2+1][j+1];
  }

  /**************************************************************
                           l*(l+1)/(2*x^4) 
  **************************************************************/

  l2 = (long double)L*((long double)L+1.0L);

  /* diagonal element, i=0, for l*(l+1)/(2*x^4) */

  Hkin[0][0] += d*d*3.0L*l2/35.0L;
  Hkin[0][1] += d*d*7.0L*l2/420.0L;
  Hkin[1][0] += d*d*7.0L*l2/420.0L;
  Hkin[1][1] += d*d*3.0L*l2/840.0L;

  /* diagonal element for for l*(l+1)/(2*x^4) */

  for (i=1; i<N; i++){
    q = (long double)i;  
    Hkin[i*2+0][2] += 26.0L*d*d*q*l2/35.0L;
    Hkin[i*2+0][3] += d*d*l2/30.0L;
    Hkin[i*2+1][2] += d*d*l2/30.0L;
    Hkin[i*2+1][3] += 2.0L*d*d*q*l2/105.0L;
  }

  /* off-diagonal element for l*(l+1)/(2*x^4) */

  for (i=0; i<(N-1); i++){

    q = (long double)i; 

    if (i==0) j = 2;
    else      j = 4;

    Hkin[i*2+0][j+0] += 9.0L*d*d*(1.0L + 2.0L*q)*l2/140.0L;
    Hkin[i*2+0][j+1] += -d*d*(6.0L + 13.0L*q)*l2/420.0L;
    Hkin[i*2+1][j+0] += d*d*(7.0L + 13.0L*q)*l2/420.0L;
    Hkin[i*2+1][j+1] += -d*d*(1.0L + 2.0L*q)*l2/280.0L;

    Hkin[(i+1)*2+0][0] = Hkin[i*2+0][j+0];
    Hkin[(i+1)*2+1][0] = Hkin[i*2+0][j+1];
    Hkin[(i+1)*2+0][1] = Hkin[i*2+1][j+0];
    Hkin[(i+1)*2+1][1] = Hkin[i*2+1][j+1];
  }

  /**************************************************************
                            -Z/(x^2)
  **************************************************************/

  /* diagonal element, i=0, for -Z/(x^2) */

  Hec[0][0] =-d*d*d*d*11.0L/420.0L*(long double)AtomNum;
  Hec[0][1] =-d*d*d*d*8.0L/1260.0L*(long double)AtomNum;
  Hec[1][0] =-d*d*d*d*8.0L/1260.0L*(long double)AtomNum; 
  Hec[1][1] =-d*d*d*d*2.0L/1260.0L*(long double)AtomNum; 

  /* diagonal element for -Z/(x^2) */

  for (i=1; i<N; i++){
    q = (long double)i; 
    Hec[i*2+0][2] = -2.0L*d*d*d*d*q*(19.0L + 78.0L*q*q)*(long double)AtomNum/105.0L;
    Hec[i*2+0][3] = -d*d*d*d*(4.0L + 63.0L*q*q)*(long double)AtomNum/315.0L;
    Hec[i*2+1][2] = -d*d*d*d*(4.0L + 63.0L*q*q)*(long double)AtomNum/315.0L;
    Hec[i*2+1][3] = -2.0L*d*d*d*d*q*(1.0L + 2.0L*q*q)*(long double)AtomNum/105.0L;
  }

  /* off-diagonal element for -Z/(x^2) */

  for (i=0; i<(N-1); i++){

    q = (long double)i; 

    if (i==0) j = 2;
    else      j = 4;

    Hec[i*2+0][j+0] = -d*d*d*d*(19.0L + 92.0L*q + 162.0L*q*q + 108.0L*q*q*q)*(long double)AtomNum/420.0L;
    Hec[i*2+0][j+1] =  d*d*d*d*(11.0L + 57.0L*q + 108.0L*q*q + 78.0L*q*q*q)*(long double)AtomNum/1260.0L;
    Hec[i*2+1][j+0] = -d*d*d*d*(16.0L + 75.0L*q + 126.0L*q*q + 78.0L*q*q*q)*(long double)AtomNum/1260.0L;
    Hec[i*2+1][j+1] =  d*d*d*d*(1.0L + 5.0L*q + 9.0L*q*q + 6.0L*q*q*q)*(long double)AtomNum/420.0L;

    Hec[(i+1)*2+0][0] = Hec[i*2+0][j+0];
    Hec[(i+1)*2+1][0] = Hec[i*2+0][j+1];
    Hec[(i+1)*2+0][1] = Hec[i*2+1][j+0];
    Hec[(i+1)*2+1][1] = Hec[i*2+1][j+1];
  }

  /**************************************************************
                             VHartree
  **************************************************************/

  /* mapping of VHartree into Ve0 and Ve1 */

  for (i=0; i<N; i++){
    Ve0[i] = VHartree[i*2];
  }

  Ve1[N-1] = (VHartree[2*N-1]-0.5L*Ve0[N-1])/0.125L;

  for (i=N-2; 0<=i; i--){
    Ve1[i] = (VHartree[i*2+1]-0.5L*Ve0[i]-0.5L*Ve0[i+1]
             +0.125L*Ve1[i+1])/0.125L;
  }

  /* diagonal element, i=0, for VHartree */

  d6 = d*d*d*d*d*d;

  Hee[0][0] = Ve0[0]*163.0L*d6/60060.0L + Ve1[0]*61.0L*d6/90090.0L
            + Ve0[1]*157.0L*d6/36036.0L - Ve1[1]*163.0L*d6/180180.0L;

  Hee[0][1] = Ve0[0]*61.0L*d6/90090.0L + Ve1[0]*31.0L*d6/180180.0L
            + Ve0[1]*6.0L*d6/5005.0L - Ve1[1]*d6/4095.0L;

  Hee[1][0] = Ve0[0]*61.0L*d6/90090.0L + Ve1[0]*31.0L*d6/180180.0L
            + Ve0[1]*6.0L*d6/5005.0L - Ve1[1]*d6/4095.0L;

  Hee[1][1] = Ve0[0]*31.0L*d6/180180.0L + Ve1[0]*2.0L*d6/45045.0L
            + Ve0[1]*d6/3003.0L - Ve1[1]*d6/15015.0L;

  /* diagonal element, i= 1 to N-2, for VHartree */

  for (i=1; i<(N-2); i++){

    q = (long double)i;
    q2 = q*q;
    q3 = q2*q;
    q4 = q2*q2; 
    q5 = q4*q; 

    Hee[i*2+0][2]  = Ve0[i-1]*d6*(-785.0L + 6570.0L*q - 23340.0L*q2 + 44720.0L*q3-47385.0L*q4 + 23166.0L*q5)/180180.L
                   + Ve1[i-1]*d6*(-163.0L + q*(1425.0L + q*(-5300.0L+13.0L*q*(820.0L + q*(-915.0L + 473.0L*q)))))/180180.0L
                   + Ve0[i]*d6*q*(855.0L + 10660.0L*q2 + 18447.0L*q4)/15015.0L 
                   + Ve1[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L
                   + Ve0[i+1]*d6*(785.0L + 6570.0L*q + 23340.0L*q2 + 44720.0L*q3+47385.0L*q4 + 23166.0L*q5)/180180.0L
                   - Ve1[i+1]*d6*(163.0L + q*(1425.0L + q*(5300.0L+13.0L*q*(820.0L + q*(915.0L + 473.0L*q)))))/180180.0L;

    Hee[i*2+0][3]  = Ve0[i-1]*d6*(216.0L - 1765.0L*q + 6080.0L*q2 - 11180.0L*q3 
                                  +11180.0L*q4 - 5005.0L*q5)/180180.0L
                   - Ve1[i-1]*d6*(-44.0L + 375.0L*q - 1350.0L*q2 + 2600.0L*q3 
                                  -2730.0L*q4 + 1287.0L*q5)/180180.0L
                   + Ve0[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L
                   + Ve1[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L
                   + Ve0[i+1]*d6*(216.0L + 1765.0L*q + 6080.0L*q2 + 11180.0L*q3 
                                  +11180.0L*q4 + 5005.0L*q5)/180180.0L
                   - Ve1[i+1]*d6*(44.0L + 375.0L*q + 1350.0L*q2 + 2600.0L*q3
                                + 2730.0L*q4 + 1287.0L*q5)/180180.0L;

    Hee[i*2+1][2]  = Ve0[i-1]*d6*(216.0L - 1765.0L*q + 6080.0L*q2 - 11180.0L*q3 
                                  +11180.0L*q4 - 5005.0L*q5)/180180.0L
                   - Ve1[i-1]*d6*(-44.0L + 375.0L*q - 1350.0L*q2 + 2600.0L*q3 
                                  -2730.0L*q4 + 1287.0L*q5)/180180.0L
                   + Ve0[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L 
                   + Ve1[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L
                   + Ve0[i+1]*d6*(216.0L + 1765.0L*q + 6080.0L*q2 + 11180.0L*q3 
                                  +11180.0L*q4 + 5005.0L*q5)/180180.0L
                   - Ve1[i+1]*d6*(44.0L + 375.0L*q + 1350.0L*q2 + 2600.0L*q3 
                                  +2730.0L*q4 + 1287.0L*q5)/180180.0L;

    Hee[i*2+1][3]  = Ve0[i-1]*d6*(-30.0L + q*(240.0L + q*(-805.0L 
                                  + 13.0L*q*(110.0L + q*(-105.0L + 44.0L*q)))))/90090.0L
                   + Ve1[i-1]*d6*(-6.0L + q*(50.0L + q*(-175.0L
                                  + 13.0L*q*(25.0L + q*(-25.0L + 11.0L*q)))))/90090.0L
                   + Ve0[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L
                   + Ve1[i]*2.0L*d6*(2.0L + 75.0L*q2 + 195.0L*q4)/45045.0L
                   + Ve0[i+1]*d6*(30.0L + q*(240.0L + q*(805.0L + 13.0L*q*(110.0L
                                  +q*(105.0L + 44.0L*q)))))/90090.0L
                   - Ve1[i+1]*d6*(6.0L + q*(50.0L + q*(175.0L + 13.0L*q*(25.0L 
                                  + q*(25.0L + 11.0L*q)))))/90090.0L;

  }

  /* diagonal element, i=N-1, for VHartree */

  i = N - 1;
  q = (long double)i;
  q2 = q*q;
  q3 = q2*q;
  q4 = q2*q2; 
  q5 = q4*q; 

  Hee[i*2+0][2]  = Ve0[i-1]*d6*(-785.0L + 6570.0L*q - 23340.0L*q2 + 44720.0L*q3-47385.0L*q4 + 23166.0L*q5)/180180.0L
                 + Ve1[i-1]*d6*(-163.0L + q*(1425.0L + q*(-5300.0L+13*q*(820.0L + q*(-915.0L + 473.0L*q)))))/180180.0L
                 + Ve0[i]*d6*q*(855.0L + 10660.0L*q2 + 18447.0L*q4)/15015.0L 
                 + Ve1[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L;

  Hee[i*2+0][3]  = Ve0[i-1]*d6*(216.0L - 1765.0L*q + 6080.0L*q2 - 11180.0L*q3 
                                +11180.0L*q4 - 5005.0L*q5)/180180.0L
                 - Ve1[i-1]*d6*(-44.0L + 375.0L*q - 1350.0L*q2 + 2600.0L*q3 
                                -2730.0L*q4 + 1287.0L*q5)/180180.0L
                 + Ve0[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L
                 + Ve1[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L;

  Hee[i*2+1][2]  = Ve0[i-1]*d6*(216.0L - 1765.0L*q + 6080.0L*q2 - 11180.0L*q3 
                                +11180.0L*q4 - 5005.0L*q5)/180180.0L
                 - Ve1[i-1]*d6*(-44.0L + 375.0L*q - 1350.0L*q2 + 2600.0L*q3 
                                -2730.0L*q4 + 1287.0L*q5)/180180.0L
                 + Ve0[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L 
                 + Ve1[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L;

  Hee[i*2+1][3]  = Ve0[i-1]*d6*(-30.0L + q*(240.0L + q*(-805.0L 
                                + 13.0L*q*(110.0L + q*(-105.0L + 44.0L*q)))))/90090.0L
                 + Ve1[i-1]*d6*(-6.0L + q*(50.0L + q*(-175.0L
                                + 13.0L*q*(25.0L + q*(-25.0L + 11.0L*q)))))/90090.0L
                 + Ve0[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L
                 + Ve1[i]*2.0L*d6*(2.0L + 75.0L*q2 + 195.0L*q4)/45045.0L;

  /* off-diagonal element for VHartree */

  for (i=0; i<(N-1); i++){

    q = (long double)i; 
    q2 = q*q;
    q3 = q2*q;
    q4 = q2*q2; 
    q5 = q4*q; 

    if (i==0) j = 2;
    else      j = 4;

    Hee[i*2+0][j+0]  = Ve0[i]*d6*(785.0L + 6570.0L*q + 23340.0L*q2 
                                  + 44720.0L*q3 + 47385.0L*q4 + 23166.0L*q5)/180180.0L
                     + Ve1[i]*d6*(216.0L + 1765.0L*q + 6080.0L*q2 + 11180.0L*q3 
                                  + 11180.0L*q4 + 5005.0L*q5)/180180.0L
                     + Ve0[i+1]*d6*(2946.0L + 20340.0L*q + 58170.0L*q2 + 86840.0L*q3
                                    + 68445.0L*q4 + 23166.0L*q5)/180180.0L
                     - Ve1[i+1]*d6*(474.0L + 3450.0L*q + 10430.0L*q2 + 16510.0L*q3 
                                    + 13845.0L*q4 + 5005.0L*q5)/180180.0L;
	
    Hee[i*2+0][j+1]  =-Ve0[i]*d6*(163.0L + q*(1425.0L + q*(5300.0L 
                                   + 13.0L*q*(820.0L + q*(915.0L + 473.0L*q)))))/180180.0L
                     - Ve1[i]*d6*(44.0L + 375.0L*q + 1350.0L*q2 + 2600.0L*q3 
                                   + 2730.0L*q4 + 1287.0L*q5)/180180.0L
                     - Ve0[i+1]*d6*(474.0L + 3450.0L*q + 10430.0L*q2 + 16510.0L*q3 
                                   + 13845.0L*q4 + 5005.0L*q5)/180180.0L
                     + Ve1[i+1]*d6*(42.0L + q*(320.0L + q*(1015.0L + 13.0L*q*(130.0L
                                   + q*(115.0L + 44.0L*q)))))/90090.0L;

    Hee[i*2+1][j+0]  = Ve0[i]*d6*(216.0L + 1765.0L*q + 6080.0L*q2 + 11180.0L*q3 
                                   + 11180.0L*q4 + 5005.0L*q5)/180180.0L
                     + Ve1[i]*d6*(30.0L + q*(240.0L + q*(805.0L + 13.0L*q*(110.0L
                                   + q*(105.0L + 44.0L*q)))))/90090.0L
                     + Ve0[i+1]*d6*(876.0L + q*(5970.0L + q*(16800.0L + 13.0L*q*(1890.0L
                                   + q*(1450.0L + 473.0L*q)))))/180180.0L
                     - Ve1[i+1]*d6*(138.0L + 990.0L*q + 2940.0L*q2 + 4550.0L*q3 
                                   + 3705.0L*q4 + 1287.0L*q5)/180180.0L;

    Hee[i*2+1][j+1]  =-Ve0[i]*d6*(44.0L + 375.0L*q + 1350.0L*q2 + 2600.0L*q3 
                                   + 2730.0L*q4 + 1287.0L*q5)/180180.0L
                     - Ve1[i]*d6*(6.0L + q*(50.0L + q*(175.0L + 13.0L*q*(25.0L 
                                   + q*(25.0L + 11.0L*q)))))/90090.0L
                     - Ve0[i+1]*d6*(138.0L + 990.0L*q + 2940.0L*q2 + 4550.0L*q3
                                   + 3705.0L*q4 + 1287.0L*q5)/180180.0L
                     + Ve1[i+1]*d6*(12.0L + q*(90.0L + q*(280.0L + 13.0L*q*(35.0L 
 	                           + q*(30.0L + 11.0L*q)))))/90090.0L;
  
    Hee[(i+1)*2+0][0] = Hee[i*2+0][j+0];
    Hee[(i+1)*2+1][0] = Hee[i*2+0][j+1];
    Hee[(i+1)*2+0][1] = Hee[i*2+1][j+0];
    Hee[(i+1)*2+1][1] = Hee[i*2+1][j+1];
  }


  /**************************************************************
                              Vxc
  **************************************************************/

  /* mapping of Vxc into Ve0 and Ve1 */

  for (i=0; i<N; i++){
    Ve0[i] = Vxc[i*2];    
  }

  Ve1[N-1] = (Vxc[2*N-1]-0.5L*Ve0[N-1])/0.125L;

  for (i=N-2; 0<=i; i--){
    Ve1[i] = (Vxc[i*2+1]-0.5L*Ve0[i]-0.5L*Ve0[i+1]
             +0.125L*Ve1[i+1])/0.125L;
  }

  /* diagonal element, i=0, for Vxc */

  d6 = d*d*d*d*d*d;

  Hxc[0][0] = Ve0[0]*163.0L*d6/60060.0L + Ve1[0]*61.0L*d6/90090.0L
            + Ve0[1]*157.0L*d6/36036.0L - Ve1[1]*163.0L*d6/180180.0L;

  Hxc[0][1] = Ve0[0]*61.0L*d6/90090.0L + Ve1[0]*31.0L*d6/180180.0L
            + Ve0[1]*6.0L*d6/5005.0L - Ve1[1]*d6/4095.0L;

  Hxc[1][0] = Ve0[0]*61.0L*d6/90090.0L + Ve1[0]*31.0L*d6/180180.0L
            + Ve0[1]*6.0L*d6/5005.0L - Ve1[1]*d6/4095.0L;

  Hxc[1][1] = Ve0[0]*31.0L*d6/180180.0L + Ve1[0]*2.0L*d6/45045.0L
            + Ve0[1]*d6/3003.0L - Ve1[1]*d6/15015.0L;

  /* diagonal element, i= 1 to N-2, for Vxc */

  for (i=1; i<(N-2); i++){

    q = (long double)i;
    q2 = q*q;
    q3 = q2*q;
    q4 = q2*q2; 
    q5 = q4*q; 

    Hxc[i*2+0][2] = Ve0[i-1]*d6*(-785.0L + 6570.0L*q - 23340.0L*q2 + 44720.0L*q3-47385.0L*q4 + 23166.0L*q5)/180180.0L
                 + Ve1[i-1]*d6*(-163.0L + q*(1425.0L + q*(-5300.0L+13.0L*q*(820.0L + q*(-915.0L + 473.0L*q)))))/180180.0L
                 + Ve0[i]*d6*q*(855.0L + 10660.0L*q2 + 18447.0L*q4)/15015.0L 
                 + Ve1[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L
                 + Ve0[i+1]*d6*(785.0L + 6570.0L*q + 23340.0L*q2 + 44720.0L*q3+47385.0L*q4 + 23166.0L*q5)/180180.0L
                 - Ve1[i+1]*d6*(163.0L + q*(1425.0L + q*(5300.0L+13*q*(820.0L + q*(915.0L + 473.0L*q)))))/180180.0L;

    Hxc[i*2+0][3] = Ve0[i-1]*d6*(216.0L - 1765.0L*q + 6080.0L*q2 - 11180.0L*q3 
                                +11180.0L*q4 - 5005.0L*q5)/180180.0L
                 - Ve1[i-1]*d6*(-44.0L + 375.0L*q - 1350.0L*q2 + 2600.0L*q3 
                                -2730.0L*q4 + 1287.0L*q5)/180180.0L
                 + Ve0[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L
                 + Ve1[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L
                 + Ve0[i+1]*d6*(216.0L + 1765.0L*q + 6080.0L*q2 + 11180.0L*q3 
                                +11180.0L*q4 + 5005.0L*q5)/180180.0L
                 - Ve1[i+1]*d6*(44.0L + 375.0L*q + 1350.0L*q2 + 2600.0L*q3
                                + 2730.0L*q4 + 1287.0L*q5)/180180.0L;

    Hxc[i*2+1][2] = Ve0[i-1]*d6*(216.0L - 1765.0L*q + 6080.0L*q2 - 11180.0L*q3 
                                +11180.0L*q4 - 5005.0L*q5)/180180.0L
                 - Ve1[i-1]*d6*(-44.0L + 375.0L*q - 1350.0L*q2 + 2600.0L*q3 
                                -2730.0L*q4 + 1287.0L*q5)/180180.0L
                 + Ve0[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L 
                 + Ve1[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L
                 + Ve0[i+1]*d6*(216.0L + 1765.0L*q + 6080.0L*q2 + 11180.0L*q3 
                                +11180.0L*q4 + 5005.0L*q5)/180180.0L
                 - Ve1[i+1]*d6*(44.0L + 375.0L*q + 1350.0L*q2 + 2600.0L*q3 
                                +2730.0L*q4 + 1287.0L*q5)/180180.0L;

    Hxc[i*2+1][3] = Ve0[i-1]*d6*(-30.0L + q*(240.0L + q*(-805.0L 
                                + 13.0L*q*(110.0L + q*(-105.0L + 44.0L*q)))))/90090.0L
                 + Ve1[i-1]*d6*(-6.0L + q*(50.0L + q*(-175.0L
                                + 13.0L*q*(25.0L + q*(-25.0L + 11.0L*q)))))/90090.0L
                 + Ve0[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L
                 + Ve1[i]*2.0L*d6*(2.0L + 75.0L*q2 + 195.0L*q4)/45045.0L
                 + Ve0[i+1]*d6*(30.0L + q*(240.0L + q*(805.0L + 13.0L*q*(110.0L
                                +q*(105.0L + 44.0L*q)))))/90090.0L
                 - Ve1[i+1]*d6*(6.0L + q*(50.0L + q*(175.0L + 13.0L*q*(25.0L 
                                + q*(25.0L + 11.0L*q)))))/90090.0L;

  }

  /* diagonal element, i=N-1, for Vxc */

  i = N - 1;
  q = (long double)i;
  q2 = q*q;
  q3 = q2*q;
  q4 = q2*q2; 
  q5 = q4*q; 

  Hxc[i*2+0][2] = Ve0[i-1]*d6*(-785.0L + 6570.0L*q - 23340.0L*q2 + 44720.0L*q3-47385.0L*q4 + 23166.0L*q5)/180180.0L
               + Ve1[i-1]*d6*(-163.0L + q*(1425.0L + q*(-5300.0L+13.0L*q*(820.0L + q*(-915.0L + 473.0L*q)))))/180180.0L
               + Ve0[i]*d6*q*(855.0L + 10660.0L*q2 + 18447.0L*q4)/15015.0L 
               + Ve1[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L;

  Hxc[i*2+0][3] = Ve0[i-1]*d6*(216.0L - 1765.0L*q + 6080.0L*q2 - 11180.0L*q3 
                              +11180.0L*q4 - 5005.0L*q5)/180180.0L
               - Ve1[i-1]*d6*(-44.0L + 375.0L*q - 1350.0L*q2 + 2600.0L*q3 
                              -2730.0L*q4 + 1287.0L*q5)/180180.0L
               + Ve0[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L
               + Ve1[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L;

  Hxc[i*2+1][2] = Ve0[i-1]*d6*(216.0L - 1765.0L*q + 6080.0L*q2 - 11180.0L*q3 
                              +11180.0L*q4 - 5005.0L*q5)/180180.0L
               - Ve1[i-1]*d6*(-44.0L + 375.0L*q - 1350.0L*q2 + 2600.0L*q3 
                              -2730.0L*q4 + 1287.0L*q5)/180180.0L
               + Ve0[i]*d6*(61.0L + 2680.0L*q2 + 9425.0L*q4)/45045.0L 
               + Ve1[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L;

  Hxc[i*2+1][3] = Ve0[i-1]*d6*(-30.0L + q*(240.0L + q*(-805.0L 
                              + 13.0L*q*(110.0L + q*(-105.0L + 44.0L*q)))))/90090.0L
               + Ve1[i-1]*d6*(-6.0L + q*(50.0L + q*(-175.0L
                              + 13.0L*q*(25.0L + q*(-25.0L + 11.0L*q)))))/90090.0L
               + Ve0[i]*2.0L*d6*q*(75.0L + 715.0L*q2 + 572.0L*q4)/45045.0L
               + Ve1[i]*2.0L*d6*(2.0L + 75.0L*q2 + 195.0L*q4)/45045.0L;


  /* off-diagonal element for Vxc */

  for (i=0; i<(N-1); i++){

    q = (long double)i; 
    q2 = q*q;
    q3 = q2*q;
    q4 = q2*q2; 
    q5 = q4*q; 

    if (i==0) j = 2;
    else      j = 4;

    Hxc[i*2+0][j+0] = Ve0[i]*d6*(785.0L + 6570.0L*q + 23340.0L*q2 
                                + 44720.0L*q3 + 47385.0L*q4 + 23166.0L*q5)/180180.0L
                   + Ve1[i]*d6*(216.0L + 1765.0L*q + 6080.0L*q2 + 11180.0L*q3 
                                + 11180.0L*q4 + 5005.0L*q5)/180180.0L
                   + Ve0[i+1]*d6*(2946.0L + 20340.0L*q + 58170.0L*q2 + 86840.0L*q3
                                  + 68445.0L*q4 + 23166.0L*q5)/180180.0L
                   - Ve1[i+1]*d6*(474.0L + 3450.0L*q + 10430.0L*q2 + 16510.0L*q3 
                                  + 13845.0L*q4 + 5005.0L*q5)/180180.0L;
	
    Hxc[i*2+0][j+1] =-Ve0[i]*d6*(163.0L + q*(1425.0L + q*(5300.0L 
                                 + 13.0L*q*(820.0L + q*(915.0L + 473.0L*q)))))/180180.0L
                   - Ve1[i]*d6*(44.0L + 375.0L*q + 1350.0L*q2 + 2600.0L*q3 
                                 + 2730.0L*q4 + 1287.0L*q5)/180180.0L
                   - Ve0[i+1]*d6*(474.0L + 3450.0L*q + 10430.0L*q2 + 16510.0L*q3 
                                 + 13845.0L*q4 + 5005.0L*q5)/180180.0L
                   + Ve1[i+1]*d6*(42.0L + q*(320.0L + q*(1015.0L + 13.0L*q*(130.0L
                                 + q*(115.0L + 44.0L*q)))))/90090.0L;

    Hxc[i*2+1][j+0] = Ve0[i]*d6*(216.0L + 1765.0L*q + 6080.0L*q2 + 11180.0L*q3 
                                 + 11180.0L*q4 + 5005.0L*q5)/180180.0L
                   + Ve1[i]*d6*(30.0L + q*(240.0L + q*(805.0L + 13.0L*q*(110.0L
                                 + q*(105.0L + 44.0L*q)))))/90090.0L
                   + Ve0[i+1]*d6*(876.0L + q*(5970.0L + q*(16800.0L + 13.0L*q*(1890.0L
                                 + q*(1450.0L + 473.0L*q)))))/180180.0L
                   - Ve1[i+1]*d6*(138.0L + 990.0L*q + 2940.0L*q2 + 4550.0L*q3 
                                 + 3705.0L*q4 + 1287.0L*q5)/180180.0L;

    Hxc[i*2+1][j+1] =-Ve0[i]*d6*(44.0L + 375.0L*q + 1350.0L*q2 + 2600.0L*q3 
                                 + 2730.0L*q4 + 1287.0L*q5)/180180.0L
                   - Ve1[i]*d6*(6.0L + q*(50.0L + q*(175.0L + 13.0L*q*(25.0L 
                                 + q*(25.0L + 11.0L*q)))))/90090.0L
                   - Ve0[i+1]*d6*(138.0L + 990.0L*q + 2940.0L*q2 + 4550.0L*q3
                                 + 3705.0L*q4 + 1287.0L*q5)/180180.0L
                   + Ve1[i+1]*d6*(12.0L + q*(90.0L + q*(280.0L + 13.0L*q*(35.0L 
 	                         + q*(30.0L + 11.0L*q)))))/90090.0L;
  
    Hxc[(i+1)*2+0][0] = Hxc[i*2+0][j+0];
    Hxc[(i+1)*2+1][0] = Hxc[i*2+0][j+1];
    Hxc[(i+1)*2+0][1] = Hxc[i*2+1][j+0];
    Hxc[(i+1)*2+1][1] = Hxc[i*2+1][j+1];
  }

  /**************************************************************
                  H = Hkin + Hee + Hec + Hxc
  **************************************************************/
  
  for (i=0; i<2*N; i++){
    for (j=0; j<6; j++){
      H[i][j] = Hkin[i][j] + Hee[i][j] + Hec[i][j] + Hxc[i][j]; 
    }
  }  

  /**************************************************************
                         overlap integral
  **************************************************************/

  /* diagonal element, i=0, for overlap integral */

  S[0][0] = d*d*d*d*d*d*49.0L/6930.0L;
  S[0][1] = d*d*d*d*d*d*13.0L/6930.0L;
  S[1][0] = d*d*d*d*d*d*13.0L/6930.0L;
  S[1][1] = d*d*d*d*d*d*7.0L/13860.0L;

  /* diagonal element for overlap integral */

  for (i=1; i<N; i++){
    q = (long double)i; 
    S[i*2+0][2] = (2.0L*d*d*d*d*d*d*q*(225.0L + 2090.0L*q*q + 2574.0L*q*q*q*q))/3465.0L;
    S[i*2+0][3] = (d*d*d*d*d*d*(13.0L + 440.0L*q*q + 1155.0L*q*q*q*q))/3465.0L;
    S[i*2+1][2] = (d*d*d*d*d*d*(13.0L + 440.0L*q*q + 1155.0L*q*q*q*q))/3465.0L;
    S[i*2+1][3] = (2.0L*d*d*d*d*d*d*q*(15.0L + 110.0L*q*q + 66.0L*q*q*q*q))/3465.0L;
  }

  /* off-diagonal element for overlap integral */

  for (i=0; i<(N-1); i++){

    q = (long double)i; 

    if (i==0) j = 2;
    else      j = 4;

    S[i*2+0][j+0] = (d*d*d*d*d*d*(1.0L + 2.0L*q)*(287.0L + 22.0L*q*(1.0L + q)*(68.0L + 81.0L*q*(1.0L + q))))/13860.0L;
    S[i*2+0][j+1] = -(d*d*d*d*d*d*(49.0L + 375.0L*q + 1210.0L*q*q + 2090.0L*q*q*q + 1980.0L*q*q*q*q + 858.0L*q*q*q*q*q))/13860.0L;
    S[i*2+1][j+0] = (d*d*d*d*d*d*(84.0L + 595.0L*q + 1760.0L*q*q + 2750.0L*q*q*q + 2310.0L*q*q*q*q + 858.0L*q*q*q*q*q))/13860.0L;
    S[i*2+1][j+1] = -(d*d*d*d*d*d*(1.0L + 2.0L*q)*(14.0L + 11.0L*q*(1.0L + q)*(7.0L + 9.0L*q*(1.0L + q))))/13860.0L;

    S[(i+1)*2+0][0] = S[i*2+0][j+0];
    S[(i+1)*2+1][0] = S[i*2+0][j+1];
    S[(i+1)*2+0][1] = S[i*2+1][j+0];
    S[(i+1)*2+1][1] = S[i*2+1][j+1];

  }

  /* freeing of arrays */

  free(Ve0);
  free(Ve1);
}



static void diagonalize(INTEGER N0, long int NumMul,
			long double **H, long double **S, 
			long double *E, long double **V)
{
  int i,j,i1,j1,ii,jj;

  char  *JOBZ="V";
  char  *RANGE="I";
  char  *UPLO="L";

  INTEGER ITYPE;
  double VL,VU; /* dummy */
  INTEGER IL,IU; 
  double ABSTOL=1.0e-15;
  double *Z;
  double *W;
  double *A;
  double *B;
  double *WORK;
  INTEGER N;
  INTEGER LDZ;
  INTEGER LDA;
  INTEGER LDB;
  INTEGER M;
  INTEGER LWORK;
  INTEGER *IWORK;
  INTEGER *IFAIL;
  INTEGER INFO;

  ITYPE = 1;

  IL = 1;
  IU = NumMul;

  N = 2*N0; 

  LDA = N;
  LDB = N;
  LDZ = N;
  LWORK = 8*N;

  A = (double*)malloc(sizeof(double)*(N+4)*(N+4));
  B = (double*)malloc(sizeof(double)*(N+4)*(N+4));
  Z = (double*)malloc(sizeof(double)*LDZ*N);
  W = (double*)malloc(sizeof(double)*N);
  WORK = (double*)malloc(sizeof(double)*LWORK);
  IWORK = (INTEGER*)malloc(sizeof(INTEGER)*5*N);
  IFAIL = (INTEGER*)malloc(sizeof(INTEGER)*N);

  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      A[i*N+j] = 0.0;
      B[i*N+j] = 0.0;
    }
  }

  i = 0;
  for (i1=0; i1<2; i1++) {
    ii = i*2 + i1;
    for (j=0; j<4; j++) {
      jj = j;
      A[ii*N+jj] = (double)H[ii][j];
      B[ii*N+jj] = (double)S[ii][j];
    }
  }

  for (i=1; i<(N0-1); i++) {
    for (i1=0; i1<2; i1++) {
      ii = i*2 + i1;
      for (j=0; j<6; j++) {
        jj = (i-1)*2 + j;
        A[ii*N+jj] = (double)H[ii][j];
        B[ii*N+jj] = (double)S[ii][j];
      }
    }
  }

  i = N0-1;
  for (i1=0; i1<2; i1++) {
    ii = i*2 + i1;
    for (j=0; j<4; j++) {
      jj = (i-1)*2 + j;
      A[ii*N+jj] = (double)H[ii][j];
      B[ii*N+jj] = (double)S[ii][j];
    }
  }

  dsygvx_( &ITYPE, JOBZ, RANGE, UPLO, &N, A, &LDA, B, &LDB, &VL, &VU, &IL, &IU, &ABSTOL,
           &M, W, Z, &LDZ, WORK, &LWORK, IWORK, IFAIL, &INFO );

  /* store eigenvectors */

  for (i=0; i<NumMul; i++) {
    for (j=0; j<N; j++) {
      V[i][j]= Z[i*N+j];
    }
  }

  /* store eigenvalues */

  for (i=0; i<NumMul; i++) {

    /*
    printf("%4d %20.16f\n",i,W[i]);
    */

    E[i] = (long double)W[i];
  }


  if (INFO>0) {
    printf("\n error in dstevx_, info=%d\n\n",INFO);fflush(stdout);
  }
  if (INFO<0) {
    printf("info=%d in dstevx_\n",INFO);fflush(stdout);
    exit(0);
  }

  free(A);
  free(B);
  free(Z);
  free(W);
  free(WORK);
  free(IWORK);
  free(IFAIL);
}








static void EigenSolver(long int SCF_iter, long int reuse_flag,
                        long int N0, long int L, long int NumMul,
                        long double **H, long double **S, 
                        long double *E, long double **V)
{
  static long int i,j,k,i0,pos_emin,po,rl,rlmax,rl0;
  static long int iter,ip,convergence_flag,rlmax0;
  static long double sum,tmp,w,ep,ep0,emin,e;
  static long double criterion=1.0e-16L;
  static long double Eshift;
  static long double **SL,**SU;
  static long double **SL0,**SU0;
  static long double *vtmp,**v0;
  static long double *vec0,*vec1,*vec2,*vec3,*vec4;
  static long double **A0,*al,*be;
  static long double **Lan_Vecs;
  static double **A1,*ko;
  static double *D0,*D1,*Lan_ko,**ac;
  static char nanchar[300];

  /* set rlmax */

  if      (7<=NumMul) rlmax0 = 500; 
  else if (NumMul==6) rlmax0 = 450;
  else if (NumMul==5) rlmax0 = 400;
  else if (NumMul==4) rlmax0 = 300;
  else if (NumMul==3) rlmax0 = 200;
  else if (NumMul==2) rlmax0 = 100; 
  else if (NumMul==1) rlmax0 = 100;
  else                rlmax0 = 100;

  rlmax = min(N0/10,rlmax0);
  if (rlmax<100) rlmax = 100;
  if ( (2*N0)<rlmax ) rlmax = 2*N0;

  /* allocation of arrays */

  SL = (long double**)malloc(sizeof(long double*)*4);
  for (i=0; i<4; i++){
    SL[i] = (long double*)malloc(sizeof(long double)*(2*N0+4));
    for (j=0; j<(2*N0+4); j++) SL[i][j] = 0.0L;
  }

  SU = (long double**)malloc(sizeof(long double*)*4);
  for (i=0; i<4; i++){
    SU[i] = (long double*)malloc(sizeof(long double)*(2*N0+4));
    for (j=0; j<(2*N0+4); j++) SU[i][j] = 0.0L;
  }

  SL0 = (long double**)malloc(sizeof(long double*)*4);
  for (i=0; i<4; i++){
    SL0[i] = (long double*)malloc(sizeof(long double)*(2*N0+4));
    for (j=0; j<(2*N0+4); j++) SL0[i][j] = 0.0L;
  }

  SU0 = (long double**)malloc(sizeof(long double*)*4);
  for (i=0; i<4; i++){
    SU0[i] = (long double*)malloc(sizeof(long double)*(2*N0+4));
    for (j=0; j<(2*N0+4); j++) SU0[i][j] = 0.0L;
  }

  v0 = (long double**)malloc(sizeof(long double*)*NumMul);
  for (i=0; i<NumMul; i++){
    v0[i] = (long double*)malloc(sizeof(long double)*(2*N0+4));
  }

  vtmp = (long double*)malloc(sizeof(long double)*(2*N0+4));

  vec0 = (long double*)malloc(sizeof(long double)*(2*N0+4));
  vec1 = (long double*)malloc(sizeof(long double)*(2*N0+4));
  vec2 = (long double*)malloc(sizeof(long double)*(2*N0+4));
  vec3 = (long double*)malloc(sizeof(long double)*(2*N0+4));
  vec4 = (long double*)malloc(sizeof(long double)*(2*N0+4));

  al = (long double*)malloc(sizeof(long double)*(rlmax+4));
  be = (long double*)malloc(sizeof(long double)*(rlmax+4));

  D0 = (double*)malloc(sizeof(double)*(rlmax+4));
  D1 = (double*)malloc(sizeof(double)*(rlmax+4));
  Lan_ko = (double*)malloc(sizeof(double)*(rlmax+4));

  for (i=0; i<(rlmax+4); i++){
    al[i] = 0.0L;
    be[i] = 0.0L;
    D0[i] = 0.0;
    D1[i] = 0.0;
    Lan_ko[i] = 0.0;
  }

  ac = (double**)malloc(sizeof(double*)*(rlmax+4));
  for (i=0; i<(rlmax+4); i++){
    ac[i] = (double*)malloc(sizeof(double)*(rlmax+4));
  }

  Lan_Vecs = (long double**)malloc(sizeof(long double*)*(rlmax+4));
  for (i=0; i<(rlmax+4); i++){
    Lan_Vecs[i] = (long double*)malloc(sizeof(long double)*(2*N0+4));
  }

  A0 = (long double**)malloc(sizeof(long double*)*(NumMul+3));
  for (i=0; i<(NumMul+3); i++){
    A0[i] = (long double*)malloc(sizeof(long double)*(NumMul+3));
  }

  A1 = (double**)malloc(sizeof(double*)*(NumMul+3));
  for (i=0; i<(NumMul+3); i++){
    A1[i] = (double*)malloc(sizeof(double)*(NumMul+3));
  }
  
  ko = (double*)malloc(sizeof(double)*(NumMul+3));

  /***************************************************************************
           find appoximate eigenvalues by a shifte-and-inverse Lanczos
  ***************************************************************************/

  if (reuse_flag==0){

    /*****************************************
        set SL and SU = H-Eshift*S
    *****************************************/

    if (SCF_iter==1){
      tmp = (long double)AtomNum/((long double)L+1.0L); 
      Eshift = -0.5L*tmp*tmp + 1.0e-1L;
    }
    else{
      Eshift = E[0] + 1.0e-1L;
    }

    SL[0][0] = H[0][0] - Eshift*S[0][0];
    SL[0][1] = H[1][1] - Eshift*S[1][1];
    SU[0][0] = H[0][0] - Eshift*S[0][0];
    SU[0][1] = H[1][1] - Eshift*S[1][1];

    for (i=1; i<N0; i++){
      SL[0][i*2+0] = H[i*2+0][2] - Eshift*S[i*2+0][2];
      SL[0][i*2+1] = H[i*2+1][3] - Eshift*S[i*2+1][3];
      SU[0][i*2+0] = SL[0][i*2+0];
      SU[0][i*2+1] = SL[0][i*2+1];
    }

    SL[1][0] = H[1][0] - Eshift*S[1][0];
    SU[1][0] = H[1][0] - Eshift*S[1][0];

    for (i=1; i<N0; i++){
      SL[1][i*2-1] = H[i*2+0][1] - Eshift*S[i*2+0][1];
      SL[1][i*2+0] = H[i*2+1][2] - Eshift*S[i*2+1][2];
      SU[1][i*2-1] = SL[1][i*2-1];
      SU[1][i*2+0] = SL[1][i*2+0];
    }

    for (i=1; i<N0; i++){
      SL[2][i*2-2] = H[i*2+0][0] - Eshift*S[i*2+0][0];
      SL[2][i*2-1] = H[i*2+1][1] - Eshift*S[i*2+1][1];
      SU[2][i*2-2] = SL[2][i*2-2];
      SU[2][i*2-1] = SL[2][i*2-1];
    }

    SL[3][0] = H[3][0] - Eshift*S[3][0];
    SU[3][0] = H[3][0] - Eshift*S[3][0];

    for (i=2; i<N0; i++){
      SL[3][i*2-3] = 0.0L;
      SL[3][i*2-2] = H[i*2+1][0] - Eshift*S[i*2+1][0];
      SU[3][i*2-3] = SL[3][i*2-3];
      SU[3][i*2-2] = SL[3][i*2-2];
    }

    /************************************
     LU factorization of (H-Eshift*S)
    ************************************/

    for (k=0; k<(2*N0-1); k++){

      w = 1.0L/SL[0][k];

      for (i=(k+1); i<=min((k+3),2*N0-1); i++){

	SL[i-k][k] = w*SL[i-k][k];
      
	SU[0][i] -= SL[i-k][k]*SU[i-k][k];
	SL[0][i] -= SL[i-k][k]*SU[i-k][k];

	for (j=max(k+1,i-3); j<i; j++){
	  SL[i-j][j] -= SL[i-k][k]*SU[j-k][k];
	}

	for (j=i+1; j<=min(min((k+3),2*N0-1),i+3); j++){
	  SU[j-i][i] -= SL[i-k][k]*SU[j-k][k];
	}

      }
    }

    for (k=0; k<2*N0; k++){
      SL[0][k] = 1.0L;
    }  

    /*****************************************
                set SL0 and SU0
    *****************************************/

    SL0[0][0] = S[0][0];
    SL0[0][1] = S[1][1];
    SU0[0][0] = S[0][0];
    SU0[0][1] = S[1][1];

    for (i=1; i<N0; i++){
      SL0[0][i*2+0] = S[i*2+0][2];
      SL0[0][i*2+1] = S[i*2+1][3];
      SU0[0][i*2+0] = SL0[0][i*2+0];
      SU0[0][i*2+1] = SL0[0][i*2+1];
    }

    SL0[1][0] = S[1][0];
    SU0[1][0] = S[1][0];

    for (i=1; i<N0; i++){
      SL0[1][i*2-1] = S[i*2+0][1];
      SL0[1][i*2+0] = S[i*2+1][2];
      SU0[1][i*2-1] = SL0[1][i*2-1];
      SU0[1][i*2+0] = SL0[1][i*2+0];
    }

    for (i=1; i<N0; i++){
      SL0[2][i*2-2] = S[i*2+0][0];
      SL0[2][i*2-1] = S[i*2+1][1];
      SU0[2][i*2-2] = SL0[2][i*2-2];
      SU0[2][i*2-1] = SL0[2][i*2-1];
    }

    SL0[3][0] = S[3][0];
    SU0[3][0] = S[3][0];

    for (i=2; i<N0; i++){
      SL0[3][i*2-3] = 0.0L;
      SL0[3][i*2-2] = S[i*2+1][0];
      SU0[3][i*2-3] = SL0[3][i*2-3];
      SU0[3][i*2-2] = SL0[3][i*2-2];
    }

    /*****************************************
        Cholesky factorization of S
    *****************************************/

    for (i=0; i<2*N0; i++){
      for (j=i; j<=min((i+3),2*N0-1); j++){

	sum = SU0[j-i][i];

	for (k=max(max(i-3,j-3),0); k<i; k++){
	  sum -= SL0[i-k][k]*SL0[j-k][k];
	}

	if (i==j){
	  if (sum<0.0L){
	    printf("error i=%2d sum=%20.15Lf\n",i,sum);
	  }

	  SL0[0][i] = sqrtl(fabsl(sum));
	} 
	else{
	  SL0[j-i][i] = sum/SL0[0][i];
	}

      }
    }

    for (i=0; i<2*N0; i++){
      SU0[0][i] = SL0[0][i]; 
      SU0[1][i] = SL0[1][i]; 
      SU0[2][i] = SL0[2][i]; 
      SU0[3][i] = SL0[3][i]; 
    }

    /************************************************
      generate an initial S^{-1}-normalized vector
    ************************************************/

    for (i=0; i<2*N0; i++){
      vec2[i] = powl(((long double)i+1.0L),3.0L);
    }

    InvMat_Vec(SL0,SU0,N0,vec2,vec1);

    tmp = 0.0L;
    for (i=0; i<2*N0; i++){
      tmp += vec2[i]*vec1[i];
    }

    tmp = 1.0L/sqrtl(fabsl(tmp));

    for (i=0; i<2*N0; i++){
      vec0[i] = vec2[i]*tmp;
      Lan_Vecs[0][i] = vec0[i];
    }

    for (i=0; i<2*N0; i++){
      vec4[i] = 0.0L;
    }

    /************************************************
        perform the shift-invert Lanczos process
    ************************************************/

    for (rl=0; rl<rlmax; rl++){

      /* store vec0 */

      for (i=0; i<2*N0; i++){
	vec3[i] = vec0[i];
      }

      /************************************
       (H-Eshift*S)^{-1} * v0
      ************************************/

      InvMat_Vec(SL,SU,N0,vec0,vec2);

      /************************************
      calculate al
      ************************************/

      tmp = 0.0L;
      for (i=0; i<2*N0; i++){
	tmp += vec0[i]*vec2[i];
      }

      al[rl] = tmp;

      /************************************
      v1 = S*v2 
      ************************************/

      Mat_Vec(S,N0,vec2,vec1); 

      /**************************************
          calculate |r_n> -> vec2 
      **************************************/

      for (i=0; i<2*N0; i++){
	vec2[i] = vec1[i] - be[rl]*vec4[i] - al[rl]*vec3[i]; 
      }

      /*************************************
      S^{-1}-orthogonalization of vec2 
      for Lan_Vecs
      *************************************/

      /* a modified Gram-Schmidt method */

      if (rl%6==0){

	for (rl0=0; rl0<=rl; rl0++){

	  InvMat_Vec(SL0,SU0,N0,vec2,vec1);

	  tmp = 0.0L;  
	  for (i=0; i<2*N0; i++){
	    tmp += Lan_Vecs[rl0][i]*vec1[i];
	  }   

	  for (i=0; i<2*N0; i++){
	    vec2[i] -= tmp*Lan_Vecs[rl0][i];
	  }   
	}
      }

      /* a classical Gram-Schmidt method */

      else{

	InvMat_Vec(SL0,SU0,N0,vec2,vec1);

	for (i=0; i<2*N0; i++){
	  vec0[i] = 0.0L;
	}   

	for (rl0=0; rl0<=rl; rl0++){
       
	  tmp = 0.0L;  
	  for (i=0; i<2*N0; i++){
	    tmp += Lan_Vecs[rl0][i]*vec1[i];
	  }

	  for (i=0; i<2*N0; i++){
	    vec0[i] += tmp*Lan_Vecs[rl0][i];
	  }   
	}    

	for (i=0; i<2*N0; i++){
	  vec2[i] -= vec0[i];
	} 
      }
   
      /************************************
      v1 = S^{-1}*v2 
      ************************************/

      InvMat_Vec(SL0,SU0,N0,vec2,vec1);
    
      /**************************************
      calculate be
      **************************************/

      tmp = 0.0L;
      for (i=0; i<2*N0; i++){
	tmp += vec2[i]*vec1[i];
      }

      be[rl+1] = sqrtl(fabsl(tmp));

      /**************************************
              calculate a new vector
      **************************************/

      tmp = 1.0L/be[rl+1];

      for (i=0; i<2*N0; i++){
	vec0[i] = vec2[i]*tmp;
	Lan_Vecs[rl+1][i] = vec0[i];
	vec4[i] = vec3[i];
      }

    } /* rl */

    /*******************************************************
           diagonalize the tri-diagonalized matrix 
    *******************************************************/

    /*
    for (rl=0; rl<rlmax; rl++){
      printf("rl=%2d al=%30.24Lf be=%30.24Lf\n",rl,al[rl],be[rl]);
    }
    */

    for (rl=0; rl<rlmax; rl++){
      D0[rl] = (double)al[rl]; 
      D1[rl] = (double)be[rl+1];
    }

    lapack_dstevx1((double)rlmax,(double)rlmax,D0,D1,Lan_ko,ac);

    for (rl=1; rl<=rlmax; rl++){
      Lan_ko[rl] = 1.0L/Lan_ko[rl] + (double)Eshift; 
    }

    for (rl=1; rl<=rlmax; rl++){
      D0[rl] = (double)rl; 
    }

    qsort_double(rlmax, Lan_ko, D0);

    for (rl=1; rl<=rlmax; rl++){
      Lan_ko[rl-1] = Lan_ko[rl];
    }

    /*
    for (rl=0; rl<rlmax; rl++){
      printf("rl=%2d Lan_ko=%18.15f\n",rl,Lan_ko[rl]);
    }
    */

    /************************************************
               calculate the eigenvectors
             for the approximate eigenvalues
    ************************************************/

    for (i=0; i<NumMul; i++){

      j = (long int)D0[i+1];

      for (k=0; k<2*N0; k++) vtmp[k] = 0.0L;

      for (rl=0; rl<rlmax; rl++){
	tmp = (long double)ac[j][rl+1]; 
	for (k=0; k<2*N0; k++){
	  vtmp[k] += Lan_Vecs[rl][k]*tmp; 
	}
      }

      InvMat_Vec(SL0,SU0,N0,vtmp,v0[i]);
    }  

  } /* if (reuse_flag==0) */

  /*********************************************
   set the previous eigenvalues and eigenvectors
  *********************************************/

  else{

    for (ip=0; ip<NumMul; ip++){
      Lan_ko[ip] = (double)E[ip];
    }

    for (ip=0; ip<NumMul; ip++){
      for (k=0; k<2*N0; k++){
        v0[ip][k] = V[ip][k];
      }
    }
  }

  /*************************************************************************
      purify the approximate eigenvalues by the shifted inverse iteration
  *************************************************************************/

  for (ip=0; ip<NumMul; ip++){

    iter = 1;
    convergence_flag = 0;
    ep = (long double)Lan_ko[ip] + criterion/10.0L;

    /************************************
                purification
    ************************************/

    /* set the initial vector */

    for (k=0; k<2*N0; k++) vec0[k] = v0[ip][k];

    do {

      /*****************************************
             set SL and SU = H-ep*S
      *****************************************/

      SL[0][0] = H[0][0] - ep*S[0][0];
      SL[0][1] = H[1][1] - ep*S[1][1];
      SU[0][0] = H[0][0] - ep*S[0][0];
      SU[0][1] = H[1][1] - ep*S[1][1];

      for (i=1; i<N0; i++){
	SL[0][i*2+0] = H[i*2+0][2] - ep*S[i*2+0][2];
	SL[0][i*2+1] = H[i*2+1][3] - ep*S[i*2+1][3];
	SU[0][i*2+0] = SL[0][i*2+0];
	SU[0][i*2+1] = SL[0][i*2+1];
      }

      SL[1][0] = H[1][0] - ep*S[1][0];
      SU[1][0] = H[1][0] - ep*S[1][0];

      for (i=1; i<N0; i++){
	SL[1][i*2-1] = H[i*2+0][1] - ep*S[i*2+0][1];
	SL[1][i*2+0] = H[i*2+1][2] - ep*S[i*2+1][2];
	SU[1][i*2-1] = SL[1][i*2-1];
	SU[1][i*2+0] = SL[1][i*2+0];
      }

      for (i=1; i<N0; i++){
	SL[2][i*2-2] = H[i*2+0][0] - ep*S[i*2+0][0];
	SL[2][i*2-1] = H[i*2+1][1] - ep*S[i*2+1][1];
	SU[2][i*2-2] = SL[2][i*2-2];
	SU[2][i*2-1] = SL[2][i*2-1];
      }

      SL[3][0] = H[3][0] - ep*S[3][0];
      SU[3][0] = H[3][0] - ep*S[3][0];

      for (i=2; i<N0; i++){
	SL[3][i*2-3] = 0.0L;
	SL[3][i*2-2] = H[i*2+1][0] - ep*S[i*2+1][0];
	SU[3][i*2-3] = SL[3][i*2-3];
	SU[3][i*2-2] = SL[3][i*2-2];
      }

      /************************************
        LU factorization of (H-ep*S)
      ************************************/

      for (k=0; k<(2*N0-1); k++){

	w = 1.0L/SL[0][k];

	for (i=(k+1); i<=min((k+3),2*N0-1); i++){

	  SL[i-k][k] = w*SL[i-k][k];
      
	  SU[0][i] -= SL[i-k][k]*SU[i-k][k];
	  SL[0][i] -= SL[i-k][k]*SU[i-k][k];

	  for (j=max(k+1,i-3); j<i; j++){
	    SL[i-j][j] -= SL[i-k][k]*SU[j-k][k];
	  }

	  for (j=i+1; j<=min(min((k+3),2*N0-1),i+3); j++){
	    SU[j-i][i] -= SL[i-k][k]*SU[j-k][k];
	  }

	}
      }

      for (k=0; k<2*N0; k++){
	SL[0][k] = 1.0L;
      }  

      /************************************
                 v2 = S*v0
      ************************************/

      for (k=0; k<2*N0; k++){
        vec3[k] = vec0[k];
      }
   
      Mat_Vec(S,N0,vec0,vec2);

      /************************************
           v1 = (H-ep*S)^{-1} * v2
      ************************************/

      InvMat_Vec(SL,SU,N0,vec2,vec1);

      /**********************************
           calculate A0 = <v1|v2>
      **********************************/

      tmp = 0.0L;
      for (k=0; k<2*N0; k++){
	tmp += vec1[k]*vec2[k]; 
      }

      e = 1.0L/tmp;

      /**********************************
          S-normalize v1 -> v0
      **********************************/

      Mat_Vec(S,N0,vec1,vec2); 

      tmp = 0.0L;
      for (k=0; k<2*N0; k++){
	tmp += vec1[k]*vec2[k];
      }
  
      tmp = 1.0L/sqrtl(fabsl(tmp));
  
      for (k=0; k<2*N0; k++){
	vec0[k] = vec1[k]*tmp;
      }

      /**********************************
            check the convergence
      **********************************/

      /*
      printf("ip=%2d iter=%2d  e=%40.30Lf\n",ip,iter,e); fflush(0);
      */

      if (fabsl(e)<criterion){

        E[ip] = e + ep; 

	/*
        printf("L=%2d ip=%2d iter=%2d  e=%40.30Lf\n",L,ip,iter,E[ip]); fflush(0);
	*/

        for (i=0; i<2*N0; i++){
          V[ip][i] = vec0[i];
	}

        convergence_flag = 1;
      } 
      else if (10<iter){

        E[ip] = e + ep;

	/*
        printf("not enough convergence, L=%2d ip=%2d e=%30.22Lf\n",L,ip,e);
	*/

	/*
        printf("L=%2d ip=%2d iter=%2d  e=%40.30Lf\n",L,ip,iter,E[ip]); fflush(0);
	*/

        for (i=0; i<2*N0; i++){
          V[ip][i] = vec0[i];
	}

        convergence_flag = 1;
      }

      /**********************************
            update ep
      **********************************/

      else {

	/*
        printf("L=%2d ip=%2d ep=%30.22Lf e=%30.22Lf\n",L,ip,ep,e);
	*/

        if (0.5L<fabsl(e))
          ep += 0.1L*e + criterion/10.0L;
        else if (0.1<fabsl(e))
          ep += 0.3L*e + criterion/10.0L;
        else if (0.05<fabsl(e))
          ep += 0.5L*e + criterion/10.0L;
        else
          ep += e + criterion/10.0L;

      }

      /**********************************
              increment of iter
      **********************************/

      iter++;

    }
    while (convergence_flag==0);  

  } /* ip */

  /* freeing of arrays */

  for (i=0; i<4; i++){
    free(SL[i]);
  }
  free(SL);

  for (i=0; i<4; i++){
    free(SU[i]);
  }
  free(SU);

  for (i=0; i<4; i++){
    free(SL0[i]);
  }
  free(SL0);

  for (i=0; i<4; i++){
    free(SU0[i]);
  }
  free(SU0);

  free(vtmp);

  for (i=0; i<NumMul; i++){
    free(v0[i]);
  }
  free(v0);

  free(vec0);
  free(vec1);
  free(vec2);
  free(vec3);
  free(vec4);

  free(al);
  free(be);

  free(D0);
  free(D1);
  free(Lan_ko);

  for (i=0; i<(rlmax+4); i++){
    free(ac[i]);
  }
  free(ac);

  for (i=0; i<(rlmax+4); i++){
    free(Lan_Vecs[i]);
  }
  free(Lan_Vecs);

  for (i=0; i<(NumMul+3); i++){
    free(A0[i]);
  }
  free(A0);

  for (i=0; i<(NumMul+3); i++){
    free(A1[i]);
  }
  free(A1);

  free(ko);

}





static void Mat_Vecs(long double **A, long int NumMul, long int N0, long double **v0, long double **v1)
{
  static long int k,i,j,i0;
  static long double sum;

  /* v1 = H*v0 */

  for (k=0; k<NumMul; k++){

    for (i=0; i<2*N0; i++){

      i0 = (long int)(i/2);
      sum = 0.0L;

      if (i<=1){
	for (j=0; j<4; j++){
	  sum += A[i][j]*v0[k][j];
	}
      }	     

      else if ( (2*N0-3)<i ){
	for (j=0; j<4; j++){
	  sum += A[i][j]*v0[k][2*i0-2+j];
	}
      }

      else {
	for (j=0; j<6; j++){
	  sum += A[i][j]*v0[k][2*i0-2+j];
	}
      }

      v1[k][i] = sum;
    }
  }
}


static void Mat_Vec(long double **A, long int N0, long double *v0, long double *v1)
{
  static long int i,j,i0;
  static long double sum;

  /* v1 = A*v0 */

  for (i=0; i<2*N0; i++){

    i0 = (long int)(i/2);
    sum = 0.0L;

    if (i<=1){
      for (j=0; j<4; j++){
	sum += A[i][j]*v0[j];
      }
    }	     

    else if ( (2*N0-3)<i ){
      for (j=0; j<4; j++){
	sum += A[i][j]*v0[2*i0-2+j];
      }
    }

    else {
      for (j=0; j<6; j++){
	sum += A[i][j]*v0[2*i0-2+j];
      }
    }

    v1[i] = sum;
  }

}


static void InvMat_Vec(long double **SL, long double **SU,
                       long int N0, long double *v0, long double *v1)
{
  static long int i,j,i0;
  static long double *v2;

  /* allocation of v2*/
  v2 = (long double*)malloc(sizeof(long double)*(2*N0+4));

  /************************************
             v2 =  L^{-1}*v0
          solve L*v2 = v0 for v2
  ************************************/

  v2[0] = v0[0]/SL[0][0];
  v2[1] = (v0[1]-SL[1][0]*v2[0])/SL[0][1];
  v2[2] = (v0[2]-SL[1][1]*v2[1]-SL[2][0]*v2[0])/SL[0][2];

  for (i=3; i<2*N0; i++){
    v2[i] = (v0[i]-SL[1][i-1]*v2[i-1]-SL[2][i-2]*v2[i-2]-SL[3][i-3]*v2[i-3])/SL[0][i];
  }

  /**************************************
             v1 = U^{-1}*v2
         solve U*v1 = v2 for v1
  **************************************/

  v1[2*N0-1] = v2[2*N0-1]/SU[0][2*N0-1];
  v1[2*N0-2] = (v2[2*N0-2]-SU[1][2*N0-2]*v1[2*N0-1])/SU[0][2*N0-2];
  v1[2*N0-3] = (v2[2*N0-3]-SU[1][2*N0-3]*v1[2*N0-2]-SU[2][2*N0-3]*v1[2*N0-1])/SU[0][2*N0-3];

  for (j=(2*N0-4); 0<=j; j--){
    v1[j] = (v2[j]-SU[1][j]*v1[j+1]-SU[2][j]*v1[j+2]-SU[3][j]*v1[j+3])/SU[0][j];
  }

  /* freeing of v2 */
  free(v2);
}









static long double rnd(long double width)
{
  /****************************************************
       This rnd() function generates random number
                -width/2 to width/2
  ****************************************************/

  long double result;

  result = (long double)rand();

  while (width<result){
    result = result/2.0L;
  }
  result = result - width*0.75L;
  return result;
}












static void Out_AllFEMLOG(long double *Ukin,    long double *Uee,    long double *Uec,
                          long double *Uxc,     long double *Uele,   long double *Utot,
                          long double *Ux,      long double *Ucorr,  long double *Ukin_x,
                          long double *Ukin_c,  long double *Virial1,long double *Virial2,
                          long double **EVAL,   long double ***EVEC, long double *Rho)
{
  static long int i,n,l,SCF,num;
  static char file0[ASIZE8] = ".alog";
  static int *NumEachL;
  static int NL2Num[10][10];
  static long double x,r,d;
  char *s_vec[20];
  FILE *fp;

  /* allocation of array */
  NumEachL = (int*)malloc(sizeof(int)*(Occupied_Lmax+1));
  for (l=0; l<(Occupied_Lmax+1); l++) NumEachL[l] = 0;

  for (n=1; n<=max_ocupied_N; n++){
    for (l=0; l<n; l++){
      if (0.0<OcpN[0][0][n][l]){
        num = NumEachL[l];
        NL2Num[n][l] = num; 
        NumEachL[l]++;        
      }
      else {
        NL2Num[n][l] = -1;
      }
    }
  }

  /* sava a file */

  fnjoint(filepath,filename,file0);

  if ((fp = fopen(file0,"w")) != NULL){

    fprintf(fp,"***************************************************\n");
    fprintf(fp,"                    Input file\n"                     );
    fprintf(fp,"***************************************************\n\n");

    fprintf(fp," System.CurrentDirectory    %s\n",filepath);
    fprintf(fp," System.Name                 %s\n",filename);

    fprintf(fp," <<< Calculation type >>>\n");
    s_vec[0]="sch"; s_vec[1]="sdirac"; s_vec[2]="dirac";
    if (Equation_Type==0)
      fprintf(fp," eq.type                     %s\n",s_vec[Equation_Type]);
    else
      fprintf(fp," eq.type                     %s%i\n",s_vec[Equation_Type],TwoComp_frag+1);

    s_vec[0]="ALL"; s_vec[1]="VPS"; s_vec[2]="PAO"; s_vec[3]="FCORE"; s_vec[4]="FEMLDA"; s_vec[5]="FEMHF"; s_vec[6]="FEMLDA0";

    fprintf(fp," calc.type                   %s\n",s_vec[Calc_Type]);
    s_vec[0]="LDA"; s_vec[1]="GGA";
    fprintf(fp," xc.type                     %s\n",s_vec[XC_switch]);

    fprintf(fp," <<< Atom >>>\n");
    fprintf(fp," AtomSpecies                 %i\n",(int)AtomNum);
    fprintf(fp," max.ocupied.N               %i\n",max_ocupied_N);
    fprintf(fp," total.electron              %6.4f\n",total_electron0);
    fprintf(fp," valence.electron            %6.4f\n",valence_electron);

    fprintf(fp,"\n");
    fprintf(fp," <ocupied.electrons\n");

    for (n=1; n<=max_ocupied_N; n++){
      fprintf(fp," %i ",n);
      for (l=0; l<n; l++){
	if (Equation_Type==2){
	  fprintf(fp," %6.4f ",OcpN[0][0][n][l]+OcpN[0][1][n][l]);
	}
	else{
	  fprintf(fp," %6.4f ",OcpN[0][0][n][l]);
	}
      }
      fprintf(fp,"\n");
    }
    fprintf(fp," ocupied.electrons>\n");

    fprintf(fp," grid.xmax                  %6.3f    # rmax=xmax^2\n",Grid_Xmax);
    fprintf(fp," grid.num                    %i\n",Grid_Num);

    fprintf(fp," <<< SCF >>>\n");
    fprintf(fp," scf.maxIter                 %i\n",SCF_MAX);
    s_vec[0]="Simple"; s_vec[1]="GR-Pulay"; s_vec[2]="Pulay"; 
    fprintf(fp," scf.Mixing.Type             %s\n",s_vec[Mixing_switch]);
    fprintf(fp," scf.Init.Mixing.Weight      %7.5f\n",Mixing_weight_init);
    fprintf(fp," scf.Min.Mixing.Weight       %7.5f\n",Min_Mixing_weight);
    fprintf(fp," scf.Max.Mixing.Weight       %7.5f\n",Max_Mixing_weight);
    fprintf(fp," scf.criterion               %6.3e\n",SCF_criterion);

    fprintf(fp,"\n");
    fprintf(fp,"*******************************************************\n");
    fprintf(fp," Eigenvalues (Hartree) in the all electron calculation\n");
    fprintf(fp,"*******************************************************\n\n");

    for (n=1; n<=max_ocupied_N; n++){
      for (l=0; l<n; l++){
        num = NL2Num[n][l];
	if (0<=num){
	  fprintf(fp," n=%3d  l=%3d  %25.15Lf\n",n,l,EVAL[l][num]);
	}
      }
    }

    fprintf(fp,"\n\n");
    fprintf(fp,"*******************************************************\n");
    fprintf(fp,"  Energies (Hartree) in the all electron calculation \n");
    fprintf(fp,"*******************************************************\n\n");

    fprintf(fp," Etot   = %24.15Lf\n",*Utot);
    fprintf(fp," Etot   = Ekin + EHart + Exc + Eec\n\n");

    fprintf(fp," Ekin   = %24.15Lf\n",*Ukin);
    fprintf(fp," EHart  = %24.15Lf\n",*Uee);
    fprintf(fp," Eec    = %24.15Lf\n",*Uec);
    fprintf(fp," Exc    = %24.15Lf\n\n",*Ux+*Ucorr);

    fprintf(fp," Exc    = Ex + Ecorr = (Ex-Ekin_x) + (Ecorr-Ekin_c) + Ekin_x + Ekin_c\n");
    fprintf(fp," Ex     = %24.15Lf\n",*Ux);
    fprintf(fp," Ecorr  = %24.15Lf\n",*Ucorr);
    fprintf(fp," Ekin_x = %24.15Lf\n",*Ukin_x);
    fprintf(fp," Ekin_c = %24.15Lf\n\n",*Ukin_c);
    fprintf(fp," Eeigen = %24.15Lf\n\n",*Uele);

    fprintf(fp," Virial theorem  2*(Ekin+Ekin_x+Ekin_c)+(EHart+Eec+Exc-Ekin_x-Ekin_c) = %+18.15Lf\n",*Virial1);
    fprintf(fp," Virial theorem (EHart+Eec+Exc-Ekin_x-Ekin_c)/(Ekin+Ekin_x+Ekin_c)    = %+18.15Lf\n\n",*Virial2); 
 
    /* write wave functions */

    fprintf(fp,"\n\n");
    fprintf(fp,"***************************************************\n");
    fprintf(fp,"                Radial wave functions              \n");
    fprintf(fp,"                x, r=x*x, l=0, 1,...               \n");
    fprintf(fp,"***************************************************\n\n");
    fprintf(fp,"\n");

    d = (long double)Grid_Xmax/(long double)(Grid_Num-1);

    for (n=1; n<=max_ocupied_N; n++){
      fprintf(fp,"n=%2d\n",n);
      for (i=0; i<Grid_Num; i++){

        x = (long double)i*d;
        r = x*x;

        fprintf(fp,"%24.19LE %24.19LE ",x,r);

        for (l=0; l<n; l++){

          num = NL2Num[n][l];

	  if (0<=num){
            fprintf(fp,"%+24.19LE ",EVEC[l][num][2*i]);
	  }
          else{
            fprintf(fp,"%+24.19LE ",0.0L);
          }

	}
        fprintf(fp,"\n");
      }
    }    

    fprintf(fp,"\n\n");
    fprintf(fp,"***************************************************\n");
    fprintf(fp,"                  Charge density                   \n");
    fprintf(fp,"                x, r=x*x, charge density           \n");
    fprintf(fp,"***************************************************\n\n");
    fprintf(fp,"\n");

    d = (long double)Grid_Xmax/(long double)(Grid_Num-1);

    for (i=0; i<Grid_Num; i++){

      x = (long double)i*d;
      r = x*x;

      fprintf(fp,"%24.19LE %24.19LE ",x,r);
      fprintf(fp,"%+24.19LE\n",Rho[2*i]);
    }

    /* close the file */

    fclose(fp);
    printf("\nThe following files are generated.\n");
    printf("  %s\n",file0);

  }
  else{
    printf("Failure of saving the Eigenvalues.\n");
  }

  /* freeing of array */
  free(NumEachL);
}




static void Eigen_HH(double **ac, double *ko, int n, int EVmax)
{
  /**********************************************************************
    Eigen_HH:

    Eigen_HH.c is a subroutine to solve a seqular equation without an
    overlap matrix using Householder method and lapack's dstegr_() or
    F77_NAME(dstedc,DSTEDC)().

    Log of Eigen_HH.c:

       Nov/22/2004  Released by T.Ozaki

  ***********************************************************************/

  double ABSTOL=1.0e-14;
  double **ad,*D,*E,
                *b1,*u,*uu,
                *p,*q,*s,*c,
                s1,s2,s3,ss,u1,u2,r,p1,
                xsum,bunbo,si,co,sum,
                a1,a2,a3,a4,a5,a6,b7,r1,r2,
                r3,x1,x2,xap,tmp1,tmp2,
                bb,bb1,ui,uj,uij;

  int jj,jj1,jj2,k,ii,ll,i3,i2,j2,
             i,j,i1,j1,n1,n2,ik,
             jk,po1,nn,count;

  double Stime, Etime;
  double Stime1, Etime1;
  double Stime2, Etime2;
  double time1,time2;

  /****************************************************
    allocation of arrays:
  ****************************************************/

  n2 = n + 5;

  ad = (double**)malloc(sizeof(double*)*n2);
  for (i=0; i<n2; i++){
    ad[i] = (double*)malloc(sizeof(double)*n2);
  }

  b1 = (double*)malloc(sizeof(double)*n2);
  u = (double*)malloc(sizeof(double)*n2);
  uu = (double*)malloc(sizeof(double)*n2);
  p = (double*)malloc(sizeof(double)*n2);
  q = (double*)malloc(sizeof(double)*n2);
  s = (double*)malloc(sizeof(double)*n2);
  c = (double*)malloc(sizeof(double)*n2);

  D = (double*)malloc(sizeof(double)*n2);
  E = (double*)malloc(sizeof(double)*n2);

  for (i=1; i<=(n+2); i++){
    uu[i] = 0.0;
  }

  if (measure_time==1) printf("size n=%3d EVmax=%2d\n",n,EVmax);

  /****************************************************
                   Householder method
  ****************************************************/

  for (i=1; i<=(n-2); i++){

    s1 = ac[i+1][i] * ac[i+1][i];
    s2 = 0.0;
    u[i+1] = ac[i+1][i];
    for (i1=i+2; i1<=n; i1++){
      tmp1 = ac[i1][i]; 
      s2 += tmp1*tmp1;
      u[i1] = tmp1;
    }

    s3 = fabs(s1 + s2);

    if (ABSTOL<fabs(ac[i+1][i])){
      if (ac[i+1][i]<0.0)    s3 =  sqrt(s3);
      else                   s3 = -sqrt(s3);
    }
    else{
      s3 = sqrt(s3);
    }

    if (ABSTOL<fabs(s2)){

      ss = ac[i+1][i];
      ac[i+1][i] = s3;
      ac[i][i+1] = s3;
      u[i+1] = u[i+1] - s3;
      u1 = s3 * s3 - ss * s3;
      u2 = 2.0 * u1;
      uu[i] = u2;
      b1[i] = ss - s3;

      r = 0.0;
      for (i1=i+1; i1<=n; i1++){
	p1 = 0.0;
	for (j=i+1; j<=n; j++){
	  p1 += ac[i1][j] * u[j];
	}
	p[i1] = p1 / u1;
	r += u[i1] * p[i1];
      }
      r = r / u2;

      for (i1=i+1; i1<=n; i1++){
	q[i1] = p[i1] - r * u[i1];
      }

      for (i1=i+1; i1<=n; i1++){
        tmp1 = u[i1];
        tmp2 = q[i1]; 
	for (j1=i+1; j1<=n; j1++){
	  ac[i1][j1] -= tmp1 * q[j1] + tmp2 * u[j1];
	}
      }

    }
  }

  for (i=1; i<=n; i++){
    for (j=1; j<=n; j++){
      ad[i][j] = ac[i][j];
    }
  }

  /*
  for (i=1; i<=n; i++){
    printf("i=%4d  D=%18.15f E=%18.15f\n",i,ad[i][i],ad[i][i+1]);
  }
  */

  /****************************************************
                  call a lapack routine
  ****************************************************/

  for (i=1; i<=n; i++){
    D[i-1] = ad[i][i];
    E[i-1] = ad[i][i+1];
  }

  lapack_dstevx1(n,EVmax,D,E,ko,ac);

  /****************************************************
    transformation of eigenvectors to original space
  ****************************************************/

  for (i=2; i<=n-1; i++){
    ad[i-1][i] = b1[i-1];
  }

  for (k=1; k<=EVmax; k++){
    for (nn=2; nn<=n-1; nn++){
      if ( (1.0e-3*ABSTOL)<fabs(uu[n-nn])){
	ss = 0.0;
	for (i=n-nn+1; i<=n; i++){
	  ss += ad[n-nn][i] * ac[k][i];
	}
	ss = 2.0*ss/uu[n-nn];
	for (i=n-nn+1; i<=n; i++){
	  ac[k][i] -= ss * ad[n-nn][i];
	}
      }
    }
  }

  /****************************************************
                     normalization
  ****************************************************/

  for (j=1; j<=EVmax; j++){
    sum = 0.0;
    for (i=1; i<=n; i++){
      sum = sum + ac[j][i] * ac[j][i];
    }
    sum = 1.0/sqrt(sum);
    for (i=1; i<=n; i++){
      ac[j][i] = ac[j][i] * sum;
    }
  }
  
  /****************************************************
            Eigenvectors to the "ac" array
  ****************************************************/

  /*
  for (i=1; i<=n; i++){
    for (j=(i+1); j<=n; j++){
      tmp1 = ac[i][j];
      tmp2 = ac[j][i];
      ac[i][j] = tmp2;
      ac[j][i] = tmp1;
    }
  }
  */

  /****************************************************
                  freeing of arrays:
  ****************************************************/

  for (i=0; i<n2; i++){
    free(ad[i]);
  }
  free(ad);

  free(b1);
  free(u);
  free(uu);
  free(p);
  free(q);
  free(s);
  free(c);
  free(D);
  free(E);
}



static void lapack_dstevx1(INTEGER N, INTEGER EVmax, double *D, double *E, double *W, double **ev)
{
  int i,j;

  char  *JOBZ="V";
  char  *RANGE="I";

  double VL,VU; /* dummy */
  INTEGER IL,IU; 
  double ABSTOL=1.0e-14;
  INTEGER M;
  double *Z;
  INTEGER LDZ;
  double *WORK;
  INTEGER *IWORK;
  INTEGER *IFAIL;
  INTEGER INFO;

  IL = 1;
  IU = EVmax;

  M = IU - IL + 1;
  LDZ = N;

  Z = (double*)malloc(sizeof(double)*LDZ*N);
  WORK = (double*)malloc(sizeof(double)*5*N);
  IWORK = (INTEGER*)malloc(sizeof(INTEGER)*5*N);
  IFAIL = (INTEGER*)malloc(sizeof(INTEGER)*N);

  dstevx_( JOBZ, RANGE, &N, D, E, &VL, &VU, &IL, &IU, &ABSTOL,
           &M, W, Z, &LDZ, WORK, IWORK, IFAIL, &INFO );

  /* store eigenvectors */

  for (i=0; i<EVmax; i++) {
    for (j=0; j<N; j++) {
      ev[i+1][j+1]= Z[i*N+j];
    }
  }

  /* shift ko by 1 */
  for (i=EVmax; i>=1; i--){
    W[i]= W[i-1];
  }

  if (INFO>0) {
    /*
    printf("\n error in dstevx_, info=%d\n\n",INFO);fflush(stdout);
    */
  }
  if (INFO<0) {
    printf("info=%d in dstevx_\n",INFO);fflush(stdout);
    exit(0);
  }

  free(Z);
  free(WORK);
  free(IWORK);
  free(IFAIL);
}
