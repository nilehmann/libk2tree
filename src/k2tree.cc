/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2tree.h>

#include <iostream>
using namespace std;
namespace k2tree_impl {

K2Tree::K2Tree(const BitString<unsigned int> &T , 
               const BitString<unsigned int> &L,
               int k1, int k2, int kl, int max_level_k1, int height, size_t size):
  T_(T.GetRawData(), T.length(), 20),
  L_(L),
  k1_(k1),
  k2_(k2),
  kl_(kl),
  max_level_k1_(max_level_k1),
  height_(height),
  size_(size),
  acum_rank_(new int[height]) {
  for (size_t i = 0; i < T.length(); ++i)
    cerr << T.GetBit(i) << " ";
  cerr << endl;

    acum_rank_[0] = 0;
    size_t acum_nodes = k1*k1;
    for (int level = 1; level < height; ++level) {
      int k = level <= max_level_k1? k1 : k2;
      acum_rank_[level] = T_.rank1(acum_nodes-1);
      acum_nodes += (acum_rank_[level]-acum_rank_[level-1])*k*k;
    }

}

bool K2Tree::CheckEdge(size_t row, size_t col) const {
  size_t N, z, div_level;
  size_t offset;  // number of nodes until current level, inclusive.
  int k;

  N = size_;
  offset = 0;
  for (int level = 0; level < height_; ++level) {
    if (level <= max_level_k1_) k = k1_;
    else if (level < height_ - 1) k = k2_;
    else k = kl_;

    int k = level <= max_level_k1_ ? k1_ : k2_;
    div_level = N/k;
    if (level > 0 && T_.access(z))
      //child_l(x,i) = rank(T_l, z - 1)*kl*kl + i - 1;
      z = z > 0 ? (T_.rank1(z-1) - acum_rank_[level-1])*k*k : 0;
    else
      return false;

    int child = row/div_level*k + col/div_level;
    z += child + offset;

    if (level > 0)
      offset += (acum_rank[level] - acum_rank_[level-1])*k*k;
    else
      offset = k*k;

    N /= k, row %= div_level, col %= div_level;
  }
  return L.GetBit(z - T.getLength());
}
}  // namespace k2tree_impl
