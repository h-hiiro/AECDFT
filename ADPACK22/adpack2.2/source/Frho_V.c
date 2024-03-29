/**********************************************************************
  MPAO_RadialF.c:

     MPAO_RadialF.c is a subroutine to interpolate a radial function
     of multiple pseudo atomic radial functions.

  Log of MPAO_RadialF.c:

     10/Nov/2002  Released by T.Ozaki

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "adpack_ext.h"

double Frho_V(int state_num, double R)
{
  static int mp_min,mp_max,m;
  static double h1,h2,h3,f1,f2,f3,f4;
  static double g1,g2,x1,x2,y1,y2,f;
  static double result;

  mp_min = 0;
  mp_max = Grid_Num - 1;
 
  if (R<MRV[0]){
    f = 0.0;
  }
  else if (MRV[Grid_Num-1]<R){
    f = 0.0;
  } 
  else{
    do{ 
      m = (mp_min + mp_max)/2;
      if (MRV[m]<R)
        mp_min = m;
      else 
        mp_max = m;
    }
    while((mp_max-mp_min)!=1);
    m = mp_max;

    /****************************************************
                   Spline like interpolation
    ****************************************************/

    if (m==1){
      h2 = MRV[m]   - MRV[m-1];
      h3 = MRV[m+1] - MRV[m];
      f2 = rho_V[state_num][m-1];
      f3 = rho_V[state_num][m];
      f4 = rho_V[state_num][m+1];

      h1 = -(h2+h3);
      f1 = f4;
    }
    else if (m==(Grid_Num-1)){
      h1 = MRV[m-1] - MRV[m-2];
      h2 = MRV[m]   - MRV[m-1];
      f1 = rho_V[state_num][m-2];
      f2 = rho_V[state_num][m-1];
      f3 = rho_V[state_num][m];

      h3 = -(h1+h2);
      f4 = f1;
    }
    else{
      h1 = MRV[m-1] - MRV[m-2];
      h2 = MRV[m]   - MRV[m-1];
      h3 = MRV[m+1] - MRV[m];
      f1 = rho_V[state_num][m-2];
      f2 = rho_V[state_num][m-1]; 
      f3 = rho_V[state_num][m];
      f4 = rho_V[state_num][m+1];
    }

    /****************************************************
                Calculate the value at R
    ****************************************************/

    g1 = ((f3-f2)*h1/h2 + (f2-f1)*h2/h1)/(h1+h2);
    g2 = ((f4-f3)*h2/h3 + (f3-f2)*h3/h2)/(h2+h3);

    x1 = R - MRV[m-1];
    x2 = R - MRV[m];

    y1 = x1/h2;
    y2 = x2/h2;

    f =  y2*y2*(3.0*f2 + h2*g1 + (2.0*f2 + h2*g1)*y2)
      + y1*y1*(3.0*f3 - h2*g2 - (2.0*f3 - h2*g2)*y1);
  }

  result = f;
  return result; 
}

