/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 *
 * Collection of usefull functions that have no better place to be.
 *
 */

#ifndef SRC_UTILS_UTILS_H_
#define SRC_UTILS_UTILS_H_

namespace k2tree_impl {
namespace utils {

  /*
   * Calculates the smallest integer greater or equal to N that is a power of
   * base.
   */
  int LogCeil(int N, int base);

  /* 
   * Calculates base raised to the power of exp, using approximately log_2(exp)
   * operations. exp bust be non negative.
   */
  int SquaringPow(int base, int exp);


}  // namespace utils
}  // namespace k2tree_impl

#endif  // SRC_UTILS_UTILS_H_
