/**********************************************************************
  adpack.h:

     adpack.h is a header file of adpack.

  Log of adpack.h:

     22/Nov/2001  Released by T.Ozaki
 
***********************************************************************/
#include <sys/types.h>
 
#define PI      3.1415926535897932384626433832795028841971693993751L
#define ASIZE1   20000   /* max number of grids                           */
#define ASIZE2      10   /* max number of L                               */
#define ASIZE3      10   /* max number of N                               */
#define ASIZE4      15   /* max number of pseudopotentials                */
#define ASIZE5      22   /* max Number of multiple bases                  */
#define ASIZE6     100   /* matrix size in Gauss_LEQ                      */
#define ASIZE7    1000   /* number of grids for Gauss-Legendre            */
#define ASIZE8     100   /* max size of character                         */
#define ASIZE9      20   /* max number of the history densities           */
#define ASIZE10    2000  /* max SCF iterations                            */
#define ASIZE11   6000   /* max number of energy mesh in searching energy */
#define ASIZE12   6000   /* max number of energy mesh in searching energy */
#define ASIZE13      2   /* for kappa of spin-orbit coupling              */
#define ASIZE14      6   /* max L of generated VPS by MR method           */
#define ASIZE15      3   /* max number of reference states in EDPP method */
#define LimitE  1.0e+303 /* limited value in your machine                 */
   
extern char filepath[ASIZE8],filename[ASIZE8];
extern char restartfile[ASIZE8];

extern int Grid_Num_Output,Local_Part_VPS,Number_VPS;
extern int Grid_Num,openmp_threads_num;;
extern int MaxL_PAO,Num_PAO,node,CTP,Num_Mixing_pDM,Mixing_switch;
extern int LL_grid,UL_grid,max_ocupied_N,Occupied_Lmax,PCC_switch,VPP_switch,TwoComp_frag;
extern int Calc_Type,Equation_Type,SOI_switch,MatP[ASIZE13][ASIZE3][ASIZE2];
extern int Check_switch,Basis_switch,Multiple_switch,SCF_MAX,XC_switch;
extern int Number_Realaxed_States;
extern int Pulay_SCF,MaxO_LF,NVPS[ASIZE4],LVPS[ASIZE4],MCP[ASIZE13][ASIZE3][ASIZE2];
extern int FC_NVPS[ASIZE4],FC_LVPS[ASIZE4];
extern int Relaxed_Switch[ASIZE3+1][ASIZE3];
extern int NumVPS_L[ASIZE2],GI_VPS[ASIZE2][ASIZE4],projector_num[ASIZE2];
extern int SCF_END,Num_Partition,Vlocal_switch,Blochl_pro_num,total_pro_num;
extern int NL_type[ASIZE2],LogD_switch,LogD_num,ghost_check;
extern int Num_MR_ES,charge_state_num,num_EDPP_grid;
extern int GVPS_MaxL,GVPS_ProNum;
/* manual mode to find PAO, (kino) */
extern int fix_MatP[ASIZE2][ASIZE5];

extern double AtomNum;  
extern double dx,Vinf,Grid_Xmin,Grid_Xmax;
extern double valence_electron,total_electron;
extern double total_electron0,total_electron1;
extern double Ekin[ASIZE15],EHart[ASIZE15],Eeigen[ASIZE15];
extern double Exc[ASIZE15],Eec[ASIZE15],Etot[ASIZE15];
extern double Ekin_VPS[ASIZE15],EHart_VPS[ASIZE15],Eeigen_VPS[ASIZE15];
extern double Exc_VPS[ASIZE15],Eec_VPS[ASIZE15],Etot_VPS[ASIZE15];
extern double Enl_VPS[ASIZE15],Etot0_VPS[ASIZE15],Eextra[ASIZE15],Eextra2[ASIZE15];
extern double LogD_MinE,LogD_MaxE,LogD_R[ASIZE2];
extern double LogD1_R[ASIZE2];
extern double SO_factor[ASIZE2],VPS_ene[ASIZE4];
extern double VPS_Rcut[ASIZE4],VPS_RcutL[ASIZE2];
extern double PAO_Rcut,PCC_Ratio,PCC_Ratio_Origin;
extern double PF[ASIZE13][ASIZE3][ASIZE2][ASIZE1];
extern double FF[ASIZE13][ASIZE3][ASIZE2][ASIZE1];
extern double MRV[ASIZE1],MXV[ASIZE1];
extern double Mixing_weight,Mixing_weight_init,SCF_criterion;
extern double Min_Mixing_weight,Max_Mixing_weight;
extern double search_UpperE,search_LowerE,MatP_ratio;
extern double Vlocal_cutoff,Vlocal_origin,Vlocal_maxcut,Vlocal_mincut;
extern double Gaussian_Alp,Av_CoreR,GVPS_Rcut;
extern double OcpN[ASIZE15][2][ASIZE3+1][ASIZE3];
extern double OcpN0[ASIZE15][2][ASIZE3+1][ASIZE3];
extern double Energy_Kin[ASIZE15][2][ASIZE3+1][ASIZE3];
extern double MPAO[ASIZE2][ASIZE5][ASIZE1];
extern double E_PAO[ASIZE2][ASIZE5],Vxc[ASIZE1],rho[ASIZE9][ASIZE1];
extern double Rrho[ASIZE9][ASIZE1],x[ASIZE7],w[ASIZE7];
extern double V[ASIZE1],V_all_ele[ASIZE1];
extern double Vcore[ASIZE1],Vh[ASIZE1],History_Uele[ASIZE9],rho_PCC[ASIZE1];
extern double V1[ASIZE2][ASIZE1],VPS[ASIZE15][ASIZE13][ASIZE2][ASIZE1];
extern double VNL[ASIZE15][ASIZE13][ASIZE2][ASIZE4][ASIZE1];
extern double VNL_W2[ASIZE13][ASIZE2][ASIZE4][ASIZE1];
extern double Vlocal[ASIZE1],E[ASIZE15][ASIZE13][ASIZE3][ASIZE2];
extern double Prho[ASIZE1],rho_V[ASIZE15][ASIZE1];
extern double NormRD[ASIZE9],Vxc_V[ASIZE1],Vh_V[ASIZE1],V2[ASIZE13][ASIZE4][ASIZE1];
extern double W2[ASIZE13][ASIZE4][ASIZE1],HisNormRD[ASIZE10],HisEeigen[ASIZE10];
extern double ProjectEnl[ASIZE4],height_wall,rising_edge,search_stepE;
extern double RNG[ASIZE3+1][ASIZE3],proj_ene[ASIZE15][ASIZE13][ASIZE2][ASIZE4];
extern double LogDE[ASIZE12],LogDF[ASIZE13][ASIZE2][3][ASIZE12];
extern double LogD_dif[ASIZE13][ASIZE2][2];
extern double charge_states[ASIZE15],Core_R[ASIZE15];
extern double Vedpp[ASIZE15][ASIZE1];
extern double Int_EDPP_Proj[ASIZE15];
extern double Evps[ASIZE15][ASIZE13][ASIZE4];
extern double Evps0[ASIZE15][ASIZE13][ASIZE4];
extern double CoesQ[30];
extern double CoesEZ[30];
extern double E_EDPP[ASIZE1];
extern double Q_EDPP[ASIZE1];
extern double ****GVPS;
extern double ****WFPS;
extern double ***TMVPS;

extern double PAO_RadialF(int l, double R, double RadF[ASIZE2][ASIZE1]);
extern double HokanF(double R, double RadF[ASIZE1], int waru_switch);
extern double MPAO_RadialF(int l, int mul, double R);
extern double VNLF(int so, int l, int mul, double R);
extern double Frho_V(int state_num, double R);

extern void readfile(char *argv[]);
extern void Set_Init();
extern void Initial_Density();
extern void GR_Pulay(int state_num, int SCF_iter);
extern void Simple_Mixing(int state_num,
                   double rho0[ASIZE1],
                   double rho1[ASIZE1],
                   double rho2[ASIZE1],
                   double Rrho2[ASIZE1]);
extern void All_Electron(int state_num);
extern void FEM_All_Electron();
extern void FEMLDA_All_Electron();
extern void FEMHF_All_Electron();
extern void Total_Energy(int state_num);
extern void Density(int state_num);
extern void Density_V(int state_num, double OcpN0[ASIZE15][2][ASIZE3+1][ASIZE3]);
extern void Density_PCC(double rho_PCC[ASIZE1], int state_num, double OcpN0[ASIZE15][2][ASIZE3+1][ASIZE3]);
extern void Core(int SCF, double Z, double Vcore[ASIZE1]);
extern void Hartree(double rh[ASIZE1], double h[ASIZE1]);
extern void XC_Xa(double rh[ASIZE1], double xc[ASIZE1]);
extern void XC_CA(double rh[ASIZE1], double xc[ASIZE1], int P_switch);
extern void XC4atom_PBE(double rh[ASIZE1], double xc[ASIZE1], int P_switch);
extern void XC_VWN(double rh[ASIZE1], double xc[ASIZE1], int P_switch);
extern void XC_PBE(double dens[2], double GDENS[3][2], double Exc[2],
            double DEXDD[2], double DECDD[2],
            double DEXDGD[3][2], double DECDGD[3][2]);
extern void XC_PW91C(double dens[2], double Ec[1], double Vc[2]);
extern void XC_EX(int NSP, double DS0, double DS[2], double EX[1], double VX[2]);
extern void VP();
extern void Hamming_O(int eq_type,
               int NL, double ep, double kappa,
               double Mo[ASIZE1],  double Lo[ASIZE1], double DM[ASIZE1], 
               double VPL[ASIZE1], double VNLp[ASIZE1],
               double Reduce_Num_grid);
extern void Hamming_I(int eq_type, int NL, double ep, double kappa,
               double Mi[ASIZE1],  double Li[ASIZE1], double DM[ASIZE1],
               double VPL[ASIZE1], double VNLp[ASIZE1],
               double Reduce_Num_grid);
extern void Gauss_LEQ(int n, double a[ASIZE6][ASIZE6], double x[ASIZE6]);
extern void Gauss_Legendre(int n, double x[ASIZE7], double w[ASIZE7],
                    int *ncof, int *flag);
extern void BHS(int state_num);
extern void TM(int state_num);
extern void MBK(int state_num);
extern void MBK2(int state_num);
extern void MR();
extern void Multiple_PAO();
// void E_NL(int NumVPS);
extern void Calc_Vlocal(double Vlocal[ASIZE1], double *V0, int local_switch);
extern void Init_VPS();
extern void Generate_VNL();
extern void Log_DeriF();
extern void ghost(int state_num);
extern void Output(char *filein);
extern void Restart_save(int state_num);
extern int Restart_load(int state_num);
extern void Make_EDPP();
extern void Find_LESP();
extern void Empty_VPS();

extern void qsort_int(long n, int *a, int *b);
extern void qsort_double(long n, double *a, double *b);

/* functions defined in addfunc.c */
extern double isgn(int nu);
extern double sgn(double nu);
extern double largest(double a, double b);
extern double smallest(double a, double b);
extern void fnjoint(char name1[ASIZE8],char name2[ASIZE8],char name3[ASIZE8]);
extern void chcp(char name1[ASIZE8],char name2[ASIZE8]);
