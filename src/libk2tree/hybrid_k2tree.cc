/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <hybrid_k2tree.h>

namespace libk2tree {
using utils::LoadValue;
using utils::SaveValue;


HybridK2Tree::HybridK2Tree(const BitArray<uint, uint> &T,
                           const BitArray<uint, uint> &L,
                           int k1, int k2, int kl, int max_level_k1,
                           int height, uint cnt, uint size) :
    basic_hybrid(T, k1, k2, kl, max_level_k1, height, cnt, size),
    L_(L) {}

HybridK2Tree::HybridK2Tree(ifstream *in) :
    basic_hybrid(in),
    L_(in) {}




size_t HybridK2Tree::GetSize() const {
  size_t size = T_->getSize();
  size += L_.GetSize();
  size += height_*sizeof(uint);
  size += (height_+1)*sizeof(uint);
  size += 5*sizeof(int) + 2*sizeof(uint) + 2*sizeof(uint*);
  return size;
}



void HybridK2Tree::Save(ofstream *out) const {
  basic_hybrid::Save(out);
  L_.Save(out);
}


bool HybridK2Tree::operator==(const HybridK2Tree &rhs) const {
  if (T_->getLength() != rhs.T_->getLength()) return false;
  for (size_t i = 0; i < T_->getLength(); ++i)
    if (T_->access(i) != rhs.T_->access(i)) return false;

  if (L_.length() != rhs.L_.length()) return false;
  for (size_t i = 0; i < L_.length(); ++i)
    if (L_.GetBit(i) != rhs.L_.GetBit(i)) return false;

  if (height_ != rhs.height_) return false;

  for (int i = 0; i < height_; ++i)
    if (acum_rank_[i] != acum_rank_[i]) return false;

  for (int i = 0; i <= height_; ++i)
    if (offset_[i] != offset_[i]) return false;

  return k1_ == rhs.k1_ && k2_ == rhs.k2_ && kl_ == rhs.kl_ &&
         max_level_k1_ == rhs.max_level_k1_ && size_ == rhs.size_;
}


}  // namespace libk2tree

