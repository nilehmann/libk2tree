/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_COMPRESSION_VOCABULARY_H_
#define INCLUDE_COMPRESSION_VOCABULARY_H_

#include <libk2tree_basic.h>
#include <algorithm>
#include <fstream>

namespace libk2tree {
namespace compression {

class Vocabulary {
 public:
  Vocabulary(uint cnt, uint size);


  explicit Vocabulary(std::ifstream *in);

  void Save(std::ofstream *out);

  const uchar *operator[](uint i) const {
    return data_ + i*size_;
  }
  const uchar *get(uint i) const {
    return data_ + i*size_;
  }

  void print() const;
  void print(uint i) const;

  void assign(uint p, const uchar* val);

  uint sort() {
    return quicksort(0, cnt_);
  }

  uint size() const {
    return size_;
  }

  uint cnt() const {
    return cnt_;
  }

  ~Vocabulary();

  bool operator==(const Vocabulary &rhs) const;

 private:
  void swap(uint a, uint b);
  uint quicksort(uint left, uint right);

  uint cnt_;
  size_t size_;
  uchar *data_;
};

}  // namespace compression
}  // namespace libk2tree
#endif  // INCLUDE_COMPRESSION_VOCABULARY_H_
