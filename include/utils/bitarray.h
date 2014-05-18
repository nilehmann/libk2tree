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

#ifndef INCLUDE_UTILS_BITARRAY_H_
#define INCLUDE_UTILS_BITARRAY_H_

#include <libk2tree_basic.h>
#include <utils/utils.h>
#include <type_traits>
#include <algorithm>


namespace libk2tree {
namespace utils {

/**
 * Dynamic bit array implementation with an array of T as underlying
 * representation. T should be an integral type.
 */
template<typename T>
class BitArray {
  static_assert(std::is_integral<T>::value, "Template parameter is not an"
                                            " integral type");

 public:
  /**
   * Creates a bit array to hold up to length bits. The array is initialized
   * with all bits to false.
   *
   * @param length Number of bits.
   */
  explicit BitArray(uint length) :
      length_(length),
      data_(new T[Ceil<uint>(length_, bits_)]) {
    for (uint i = 0; i < Ceil<uint>(length_, bits_); ++i)
      this->data_[i] = 0;
  }

  /**
   * Constructs a bitarray from a file.
   *
   * @param in Input stream.
   */
  explicit BitArray(ifstream *in) :
      length_(LoadValue<uint>(in)),
      data_(LoadValue<T>(in, Ceil<uint>(length_, bits_))) {}

  /**
   * Copy constructor
   */
  BitArray(const BitArray<T>& rhs) :
      length_(rhs.length()),
      data_(new T[Ceil<uint>(length_, bits_)]) {
    uint size = Ceil<uint>(length_, bits_);
    std::copy(rhs.data_, rhs.data_ + size, data_);
  }

  /**
   * Declaration of methods to prevent the copy assignment.
   */
  BitArray &operator=(const BitArray& rhs) {
    delete [] data_;
    length_ = rhs.length_;
    uint size = Ceil<uint>(length_, bits_);
    data_ = new T[size];
    std::copy(rhs.data_, rhs.data_ + size, data_);
  }

  /**
   * Saves BitArray to the output file stream.
   *
   * @param out Output file stream.
   */
  void Save(ofstream *out) const {
    SaveValue(out, length_);
    SaveValue(out, data_, Ceil<uint>(length_, bits_));
  }

  /** 
   * Returns memory usage.
   *
   * @return Size in bytes.
   */
  inline size_t GetSize() const {
    size_t size = sizeof(int) + sizeof(uint);
    size += sizeof(uint)*(Ceil<uint>(length_, bits_)) + sizeof(uint*);
    return size;
  }

  /**
   * Set bit p to true.
   *
   * @param p Position to set.
   */
  inline void SetBit(uint p) {
    data_[p/bits_] |= (1 << (p%bits_));
  }

  /**
   * Set bit p to false.
   *
   * @param p Position to clean.
   */
  inline void CleanBit(uint p) {
    data_[p/bits_] &= ~(1 << (p%bits_));
  }

  /**
   * Returns bit on the specified position.
   *
   * @param p Position
   * @return True if the bit is 1, false otherwise.
   */
  inline bool GetBit(uint p) const {
    return (data_[p/bits_] >> (p%bits_) ) & 1;
  }

  /**
   * Returns the length of the bit array.
   *
   * @return Number of bits in the bitarray.
   */
  inline uint length() const {
    return length_;
  }

  /**
   * Returns the underlying representation.
   *
   * @return Pointer to the first position of the underlying array.
   */
  inline const T *GetRawData() const {
    return data_;
  }

  ~BitArray() {
    delete [] data_;
  }


 private:
  /** Number of bits on T. */
  static const int bits_ = sizeof(T)*kByteBits;
  /** Number of bits in the array. */
  uint length_;
  /** Array to hold bits. */
  T * data_;
};

}  // namespace utils
}  // namespace libk2tree
#endif  // INCLUDE_UTILS_BITARRAY_H_
