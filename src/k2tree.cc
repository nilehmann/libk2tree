/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <bits/k2tree_iterators.h>
#include <bits/utils/utils.h>
#include <BitSequence.h>

#include <cstdio>
namespace k2tree_impl {
using cds_static::BitSequenceRG;
using utils::SaveValue;
using utils::LoadValue;

K2Tree::K2Tree(const BitArray<unsigned int> &T,
               const BitArray<unsigned int> &L,
               int k1, int k2, int kl, int max_level_k1, int height,
               size_t size):
  T_(new BitSequenceRG(T.GetRawData(), T.length(), 20)),
  L_(L),
  k1_(k1),
  k2_(k2),
  kl_(kl),
  max_level_k1_(max_level_k1),
  height_(height),
  size_(size),
  acum_rank_(new size_t[height]) {
    acum_rank_[0] = 0;
    size_t acum_nodes = k1*k1;
    for (int level = 1; level < height; ++level) {
      int k = level <= max_level_k1? k1 : k2;
      acum_rank_[level] = T_->rank1(acum_nodes-1);
      acum_nodes += (acum_rank_[level]-acum_rank_[level-1])*k*k;
    }
}

K2Tree::K2Tree(ifstream *in):
  T_(BitSequence::load(*in)),
  L_(in),
  k1_(LoadValue<int>(in)),
  k2_(LoadValue<int>(in)),
  kl_(LoadValue<int>(in)),
  max_level_k1_(LoadValue<int>(in)),
  height_(LoadValue<int>(in)),
  size_(LoadValue<size_t>(in)),
  acum_rank_(LoadValue<size_t>(in, height_)) {}

void K2Tree::Save(ofstream *out) const {
  T_->save(*out);
  L_.Save(out);
  SaveValue(out, k1_);
  SaveValue(out, k2_);
  SaveValue(out, kl_);
  SaveValue(out, max_level_k1_);
  SaveValue(out, height_);
  SaveValue(out, size_);
  SaveValue(out, acum_rank_, height_);
}
K2Tree::~K2Tree() {
  delete [] acum_rank_;
  delete T_;
}

bool K2Tree::operator==(const K2Tree &rhs) const {
  if (T_->getLength() != rhs.T_->getLength()) return false;
  for (size_t i = 0; i < T_->getLength(); ++i)
    if (T_->access(i) != rhs.T_->access(i)) return false;

  if (L_.length() != rhs.L_.length()) return false;
  for (size_t i = 0; i < L_.length(); ++i)
    if (L_.GetBit(i) != rhs.L_.GetBit(i)) return false;

  if (height_ != rhs.height_) return false;

  for (int i = 0; i < height_; ++i)
    if (acum_rank_[i] != acum_rank_[i]) return false;

  return k1_ == rhs.k1_ && k2_ == rhs.k2_ && kl_ == rhs.kl_ &&
         max_level_k1_ == rhs.max_level_k1_ && size_ == rhs.size_;
}

K2Tree::DirectIterator K2Tree::DirectBegin(size_t p) const {
  return DirectIterator(this, p, false);
}

K2Tree::DirectIterator K2Tree::DirectEnd(size_t p) const {
  return DirectIterator(this, p, true);
}

K2Tree::InverseIterator K2Tree::InverseBegin(size_t q) const {
  return InverseIterator(this, q, false);
}

K2Tree::InverseIterator K2Tree::InverseEnd(size_t q) const {
  return InverseIterator(this, q, true);
}


bool K2Tree::CheckLink(size_t p, size_t q) const {
  size_t N, z, div_level;
  size_t offset;  // number of nodes until current level, inclusive.
  int k;

  N = size_;
  z = offset = 0;
  for (int level = 0; level < height_; ++level) {
    k = GetK(level);

    div_level = N/k;
    if (level > 0 && T_->access(z))
      // child_l(x,i) = rank(T_l, z - 1)*kl*kl + i - 1;
      z = z > 0 ? (T_->rank1(z-1) - acum_rank_[level-1])*k*k : 0;
    else if (level > 0)
      return false;

    int child = p/div_level*k + q/div_level;
    z += child + offset;

    if (level > 0)
      offset += (acum_rank_[level] - acum_rank_[level-1])*k*k;
    else
      offset = k*k;

    N /= k, p %= div_level, q %= div_level;
  }
  return L_.GetBit(z - T_->getLength());
}
}  // namespace k2tree_impl
