/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 *
 * Refer to utils.h for more details.
 *
 */

#include <bits/utils/utils.h>

namespace libk2tree {
namespace utils {

int LogCeil(double N, int base) {
  int log;
  double pow;
  for (pow = 1, log = 0; pow < N; pow *= base, ++log) {}
  return log;
}

int SquaringPow(int base, int exp) {
  // Iterative aproach that increase pow
  // for every bit on true in exp.
  int pow;
  for (pow = 1; exp; base *= base, exp >>= 1)
    if (exp & 1)
      pow *= base;
  return pow;
}


}  // namespace utils
}  // namespace libk2tree
