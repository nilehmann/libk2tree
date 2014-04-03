/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 * 
 * Dynamic bit array
 */

#ifndef INCLUDE_BITS_UTILS_BITARRAY_H_
#define INCLUDE_BITS_UTILS_BITARRAY_H_

#include <bits/utils/utils.h>
#include <cstddef>


namespace k2tree_impl {
namespace utils {

/*
 * Dynamic bit array implementation with an array of T as underlying
 * representation. T should be typically a unsigned char or an unsigned int.
 */
template<typename T>
class BitArray {
 public:
  /* Create a bit array to hold up to length bits. The array is initialized
   * with all bits to false.
   */
  explicit BitArray(size_t length) :
      bits_(sizeof(T)*8),
      length_(length),
      data_(new T[length/bits_ + 1]) {
    for (size_t i = 0; i < length/bits_ + 1; ++i)
      this->data_[i] = 0;
  }

  /*
   * Construct a bitarray from a stream
   */
  explicit BitArray(ifstream *in) :
      bits_(LoadValue<int>(in)),
      length_(LoadValue<size_t>(in)),
      data_(LoadValue<T>(in, length_)) {}

  /* 
   * Copy constructor
   */
  BitArray(const BitArray<T>& rhs) :
      bits_(sizeof(T)*8),
      length_(rhs.length()),
      data_(new T[rhs.length()/bits_ + 1]) {
    for (size_t i = 0; i < length_/bits_ + 1; ++i)
      this->data_[i] = rhs.data_[i];
  }

  /*
   * Save BitArray to the output file stream.
   */
  void Save(ofstream *out) const {
    SaveValue(out, bits_);
    SaveValue(out, length_);
    SaveValue(out, data_, length_);
  }
  /*
   * Set bit p to true.
   * @param p Position to set.
   */
  inline void SetBit(size_t p) {
    data_[p/bits_] |= (1 << (p%bits_));
  }

  /*
   * Clean bit p to false.
   * @param p Position to clean.
   */
  inline void CleanBit(size_t p) {
    data_[p/bits_] &= ~(1 << (p%bits_));
  }

  /*
   * Return the bit on position p.
   */
  inline bool GetBit(size_t p) const {
    return (data_[p/bits_] >> (p%bits_) ) & 1;
  }

  /*
   * Return the length of the bit array.
   */
  inline size_t length() const {
    return length_;
  }

  /*
   * Return the underlying representation
   */
  inline T *GetRawData() const {
    return data_;
  }

  ~BitArray() {
    delete [] data_;
  }


 private:
  /*Number of bits on T */
  int bits_;
  size_t length_;
  T * data_;

  /*
   * Declaration of methods to prevent the copy assignment.
   */
  BitArray &operator=(const BitArray&);
};

}  // namespace utils
}  // namespace k2tree_impl
#endif  // INCLUDE_BITS_UTILS_BITARRAY_H_
