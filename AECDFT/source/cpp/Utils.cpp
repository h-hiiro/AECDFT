#include "LibAECDFT.hpp"
#include <cmath>
using namespace std;

// return a^b
double intpow(double a, int b) {
  if (b == 0)
    return 1;
  int b_abs = abs(b);
  double a_pow_b_abs = 1;
  for (int i = 0; i < b_abs; i++) {
    a_pow_b_abs *= a;
  }
  if (b > 0)
    return a_pow_b_abs;
  else
    return 1.0 / a_pow_b_abs;
}