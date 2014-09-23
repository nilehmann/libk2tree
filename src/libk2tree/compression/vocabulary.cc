/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */


#include <libk2tree_basic.h>
#include <compression/vocabulary.h>
#include <utils/utils.h>

namespace libk2tree {
using utils::strcmp;
using utils::LoadValue;
using utils::SaveValue;

namespace compression {

Vocabulary::Vocabulary(size_t cnt, uint size)
    : cnt_(cnt),
      size_(size),
      data_(new uchar[cnt*size_]) {}

Vocabulary::Vocabulary(std::ifstream *in)
    : cnt_(LoadValue<size_t>(in)),
      size_(LoadValue<uint>(in)),
      data_(LoadValue<uchar>(in, cnt_*size_)) {}

void Vocabulary::Save(std::ofstream *out) {
  SaveValue(out, cnt_);
  SaveValue(out, size_);
  SaveValue(out, data_, cnt_*size_);
}


void Vocabulary::print() const {
  for (size_t i = 0; i < cnt_; ++i)
    print(i);
  printf("\n");
}
void Vocabulary::print(size_t i) const {
  const uchar *word = (*this)[i];
  printf("[");
  for (uint bit = 0; bit < size_*sizeof(uchar)*8; ++bit) {
    if ((word[bit/sizeof(uchar)/8] >> (bit%(sizeof(uchar)*8))) & 1)
      printf("1");
    else
      printf("0");
  }
  printf("]");
}

void Vocabulary::assign(size_t p, const uchar* val) {
  std::copy(val, val + size_, data_ + p*size_);
}


void Vocabulary::swap(size_t a, size_t b) {
  for (uint i = 0; i < size_; ++i)
    std::swap(data_[a*size_ + i], data_[b*size_ + i]);
}


Vocabulary::~Vocabulary() {
  delete [] data_;
}

bool Vocabulary::operator==(const Vocabulary &rhs) const {
  if (cnt_ != rhs.cnt_ && size_ != rhs.size_)
    return false;
  for (size_t i = 0; i < cnt_*size_; ++i)
    if (data_[i] != rhs.data_[i])
      return false;
  return true;
}

size_t Vocabulary::quicksort(size_t left, size_t right) {
  if (left == right)
    return 0;
  if (left == right - 1)
    return 1;

  size_t i, j, k;
  i = j = k = left;
  while (k < right - 1) {
    // We use the last element as pivot.
    int cmp = strcmp((*this)[k], (*this)[right-1], size_);

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

  size_t unique;
  unique = quicksort(left, i);
  unique += quicksort(j, right);
  return unique + 1;
}

}  // namespace compression
}  // namespace libk2tree
