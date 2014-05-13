/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_COMPRESSION_ARRAY_H_
#define INCLUDE_COMPRESSION_ARRAY_H_

#include <libk2tree_basic.h>
#include <utils/utils.h>
#include <algorithm>
#include <fstream>

namespace libk2tree {
using utils::strcmp;
using utils::LoadValue;
using utils::SaveValue;
namespace compression {

template<class T>
class Array {
 public:
  Array(uint cnt, uint size)
      : cnt_(cnt),
        size_(size),
        data_(new T[cnt*size_]) {}

  explicit Array(std::ifstream *in)
      : cnt_(LoadValue<uint>(in)),
        size_(LoadValue<size_t>(in)),
        data_(LoadValue<T>(in, cnt_*size_)) {}

  void Save(std::ofstream *out) {
    SaveValue(out, cnt_);
    SaveValue(out, size_);
    SaveValue(out, data_, cnt_*size_);
  }

  const T *operator[](uint i) const {
    return data_ + i*size_;
  }
  const T *get(uint i) const {
    return data_ + i*size_;
  }

  void print() const {
    for (uint i = 0; i < cnt_; ++i)
      print(i);
  }
  void print(uint i) const {
    const T *word = (*this)[i];
    for (uint bit = 0; bit < size_*sizeof(T)*8; ++bit) {
      if ((word[bit/sizeof(T)/8] >> (bit%(sizeof(T)*8))) & 1)
        fprintf(stderr, "1");
      else
        fprintf(stderr, "0");
    }
    fprintf(stderr, "\n");
  }

  void assign(uint p, const T* val) {
    std::copy(val, val + size_, data_ + p*size_);
  }

  uint sort() {
    return quicksort(0, cnt_);
  }

  void swap(uint a, uint b) {
    for (uint i = 0; i < size_; ++i)
      std::swap(data_[a*size_ + i], data_[b*size_ + i]);
  }

  uint size() const {
    return size_;
  }

  uint cnt() const {
    return cnt_;
  }

  ~Array() {
    delete [] data_;
  }

  bool operator==(const Array &rhs) {
    if (cnt_ != rhs.cnt_ && size_ != rhs.size_)
      return false;
    for (size_t i = 0; i < cnt_*size_; ++i)
      if (data_[i] != rhs.data_[i])
        return false;
    return true;
  }

 private:
  uint quicksort(uint left, uint right) {
    if (left == right)
      return 0;
    if (left == right - 1)
      return 1;

    uint i, j, k;
    i = j = k = left;
    while (k < right - 1) {
      // We use the last element as pivot.
      char cmp = strcmp((*this)[k], (*this)[right-1], size_);

      if (cmp == 0) {
        swap(j, k);
        ++j;
      }
      if (cmp < 0) {
        swap(j, k);
        swap(j, i);
        ++j;
        ++i;
      }
      ++k;
    }
    // Move the pivot to final position
    swap(j, right - 1);
    ++j;

    uint unique;
    unique = quicksort(left, i);
    unique += quicksort(j, right);
    return unique + 1;
  }

  uint cnt_;
  size_t size_;
  T *data_;
};

}  // namespace compression
}  // namespace libk2tree
#endif  // INCLUDE_COMPRESSION_ARRAY_H_
