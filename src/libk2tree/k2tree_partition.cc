/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2tree_partition.h>
#include <bits/utils/utils.h>


namespace libk2tree {
using utils::LoadValue;

K2TreePartition::K2TreePartition(ifstream *in) :
    cnt_(LoadValue<uint>(in)),
    submatrix_size_(LoadValue<uint>(in)),
    k0_(LoadValue<int>(in)),
    subtrees(k0_) {
  for (int i = 0; i < k0_; ++i) {
    subtrees[i].reserve(k0_);
    for (int j = 0; j < k0_; ++j)
      subtrees[i].emplace_back(in);
  }

}

bool K2TreePartition::CheckLink(uint p, uint q) const {
  const basic_k2tree<uint> &t = subtrees[p/submatrix_size_][q/submatrix_size_];
  return t.CheckLink(p % submatrix_size_, q % submatrix_size_);
}

void K2TreePartition::Memory() const {
  for (int i = 0; i < k0_; ++i)
    for (int j = 0; j < k0_; ++j) {
      printf("row: %d, col: %d\n", i, j);
      subtrees[i][j].Memory();
      printf("\n");
    }
  printf("Total: %ld\n", GetSize());
}

size_t K2TreePartition::GetSize() const {
  size_t size = 0;
  for (int i = 0; i < k0_; ++i)
    for (int j = 0; j < k0_; ++j)
      size += subtrees[i][j].GetSize();
  return size;
}

}  // namespace libk2tree
