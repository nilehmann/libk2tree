/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 * 
 * Dynamic bitstring
 */

#ifndef SRC_UTILS_BITSTRING_H_
#define SRC_UTILS_BITSTRING_H_

#include <cstddef>

namespace k2tree_impl {
namespace utils {

/*
 * Dynamic bitstring implementation with an array of T as underlying
 * representation. T should be typically a char or an int.
 */
template<typename T>
class BitString {
 public:
  /* Create a bitstring to hold up to length bits. The string is initialized
   * with all bits to false.
   */
  explicit BitString(size_t length) :
      bits_(sizeof(T)*8),
      arr_(0),
      length_(length) {
    this->arr_ = new T[length/bits_ + 1];
    for (size_t i = 0; i < length/bits_ + 1; ++i)
      this->arr_[i] = 0;
  }
  /*
   * Set bit p to true.
   * @param p Position to set.
   */
  inline void SetBit(size_t p) {
    arr_[p/bits_] |= (1 << (p%bits_));
  }

  /*
   * Clean bit p to false.
   * @param p Position to clean.
   */
  inline void CleanBit(size_t p) {
    arr_[p/bits_] &= ~(1 << (p%bits_));
  }

  /*
   * Return the bit on position p.
   */
  inline bool GetBit(size_t p) const {
    return (arr_[p/bits_] >> (p%bits_) ) & 1;
  }

  /*
   * Return the length of the bitstring.
   */
  inline size_t length() const {
    return length_;
  }

  ~BitString() {
    delete [] arr_;
  }


 private:
  /*Number of bits on T */
  int bits_;
  T * arr_;
  size_t length_;

  /*
   * Declaration of methods to prevent the copy constructor
   * and assignment.
   */
  BitString(const BitString&);
  BitString &operator=(const BitString&);
};

}  // namespace utils
}  // namespace k2tree_impl
#endif  // SRC_UTILS_BITSTRING_H_
