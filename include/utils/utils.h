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

#ifndef INCLUDE_UTILS_UTILS_H_
#define INCLUDE_UTILS_UTILS_H_

#include <libk2tree_basic.h>
#include <fstream>
#include <cassert>
#include <type_traits>
#include <boost/type_traits.hpp>

namespace libk2tree {
namespace utils {
using std::ifstream;
using std::ofstream;

/**
 * Calculates the smalles integer gretear or equal to x/y
 */
template<typename T>
inline T Ceil(T x, T y) {
  static_assert(std::is_integral<T>::value, "Parameter is not integral type");
  return (x % y) ? x/y + 1 : x/y;
}

/**
 * Calculates the smallest integer greater or equal to N that is a power of
 * base.
 */
template<typename T>
uint LogCeil(double N, T base) {
  static_assert(std::is_integral<T>::value, "Parameter is not integral type");
  assert(base > 1);
  uint log;
  double pow;
  for (pow = 1, log = 0; pow < N; pow *= base, ++log) {}
  return log;
}

/**
 * Calculates base raised to the power of exp, using approximately log_2(exp)
 * operations. exp bust be non negative.
 */
int SquaringPow(int base, int exp);

/**
 * Calculates base raised to the power of exp, in exp operations.
 */
template<typename _Size>
_Size Pow(uint base, uint exp) {
  _Size pow = 1;
  for (uint i = 0; i < exp; ++i)
    pow *= base;
  return pow;
}

/**
 * Saves a value into an ofstream.
 * The type must be TriviallyCopyable
 */
template <typename T>
void SaveValue(ofstream *out, T val) {
  out->write(reinterpret_cast<char *>(&val), sizeof(T));
}

/**
 * Loads a value from an istream.
 * The type must be TriviallyCopyable
 */
template <typename T>
T LoadValue(ifstream *in) {
  T ret;
  in->read(reinterpret_cast<char *>(&ret), sizeof(T));
  return ret;
}

/**
 * Saves len values into an ofstream.
 * The type must be TriviallyCopyable
 */
template <typename T>
void SaveValue(ofstream *out, T *val, size_t length) {
  out->write(reinterpret_cast<char *>(val),
             (std::streamsize) (length * sizeof(T)));
}

/**
 * Loads len values from an istream.
 * The type must be TriviallyCopyable
 */
template <typename T>
T *LoadValue(ifstream *in, size_t length) {
  T *ret = new T[length];
  in->read(reinterpret_cast<char *>(ret),
           (std::streamsize) (length * sizeof(T)));
  return ret;
}

/**
 * Find the smallest prime greater or equal to n
 */
size_t NearestPrime(size_t n);

int strcmp(const uchar *w1, const uchar *w2, uint size);

}  // namespace utils
}  // namespace libk2tree

#endif  // INCLUDE_UTILS_UTILS_H_
