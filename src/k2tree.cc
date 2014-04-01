/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2tree.h>

namespace k2tree_impl {

K2Tree::K2Tree(const BitArray<unsigned int> &T,
               const BitArray<unsigned int> &L,
               int k1, int k2, int kl, int max_level_k1, int height,
               size_t size):
  T_(T.GetRawData(), T.length(), 20),
  L_(L),
  k1_(k1),
  k2_(k2),
  kl_(kl),
  max_level_k1_(max_level_k1),
  height_(height),
  size_(size),
  acum_rank_(new int[height]) {
    acum_rank_[0] = 0;
    size_t acum_nodes = k1*k1;
    for (int level = 1; level < height; ++level) {
      int k = level <= max_level_k1? k1 : k2;
      acum_rank_[level] = T_.rank1(acum_nodes-1);
      acum_nodes += (acum_rank_[level]-acum_rank_[level-1])*k*k;
    }
}

K2Tree::DirectIterator K2Tree::DirectBegin(size_t p) const {
  return DirectIterator(this, p, false);
}

K2Tree::DirectIterator K2Tree::DirectEnd(size_t p) const {
  return DirectIterator(this, p, true);
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
    if (level > 0 && T_.access(z))
      // child_l(x,i) = rank(T_l, z - 1)*kl*kl + i - 1;
      z = z > 0 ? (T_.rank1(z-1) - acum_rank_[level-1])*k*k : 0;
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
  return L_.GetBit(z - T_.getLength());
}
}  // namespace k2tree_impl
