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

#include <libk2tree_basic.h>
#include <bits/utils/utils.h>
#include <cstddef>


namespace libk2tree {
namespace utils {

/*
 * Dynamic bit array implementation with an array of T as underlying
 * representation. T should be typically a uchar or an uint.
 */
template<typename T, class _Size>
class BitArray {
 public:
  /* Creates a bit array to hold up to length bits. The array is initialized
   * with all bits to false.
   */
  explicit BitArray(_Size length) :
      bits_(sizeof(T)*8),
      length_(length),
      data_(new T[Ceil<_Size>(length_, bits_)]) {
    for (_Size i = 0; i < Ceil<_Size>(length_, bits_); ++i)
      this->data_[i] = 0;
  }

  /*
   * Constructs a bitarray from a stream
   */
  explicit BitArray(ifstream *in) :
      bits_(LoadValue<int>(in)),
      length_(LoadValue<_Size>(in)),
      data_(LoadValue<T>(in, Ceil<_Size>(length_, bits_))) {}

  /* 
   * Copy constructor
   */
  BitArray(const BitArray<T, _Size>& rhs) :
      bits_(sizeof(T)*8),
      length_(rhs.length()),
      data_(new T[Ceil<_Size>(length_, bits_)]) {
    for (_Size i = 0; i < Ceil<_Size>(length_, bits_); ++i)
      this->data_[i] = rhs.data_[i];
  }

  /*
   * Saves BitArray to the output file stream.
   */
  void Save(ofstream *out) const {
    SaveValue(out, bits_);
    SaveValue(out, length_);
    SaveValue(out, data_, Ceil<_Size>(length_, bits_));
  }

  inline size_t GetSize() const {
    // bits_ + length + data_ + (ptr to data_)
    size_t size = sizeof(int) + sizeof(_Size);
    size += sizeof(_Size)*(Ceil<_Size>(length_, bits_)) + sizeof(_Size*);
    return size;
  }

  /*
   * Set bit p to true.
   * @param p Position to set.
   */
  inline void SetBit(_Size p) {
    data_[p/bits_] |= (1 << (p%bits_));
  }

  /*
   * Clean bit p to false.
   * @param p Position to clean.
   */
  inline void CleanBit(_Size p) {
    data_[p/bits_] &= ~(1 << (p%bits_));
  }

  /*
   * Returns the bit on position p.
   */
  inline bool GetBit(_Size p) const {
    return (data_[p/bits_] >> (p%bits_) ) & 1;
  }

  /*
   * Returns the length of the bit array.
   */
  inline _Size length() const {
    return length_;
  }

  /*
   * Returns the underlying representation.
   */
  inline const T *GetRawData() const {
    return data_;
  }

  ~BitArray() {
    delete [] data_;
  }


 private:
  // Number of bits on T.
  int bits_;
  // Number of bits in the array.
  _Size length_;
  // Array to hold bits.
  T * data_;

  /*
   * Declaration of methods to prevent the copy assignment.
   */
  BitArray &operator=(const BitArray&);


};

}  // namespace utils
}  // namespace libk2tree
#endif  // INCLUDE_BITS_UTILS_BITARRAY_H_
