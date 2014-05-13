/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2tree_partition.h>

namespace libk2tree {
K2TreePartition::K2TreePartition(std::ifstream *in) : basic_partition(in) {}

uint K2TreePartition::WordsCnt() const {
  size_t leaves = 0;
  for (int row = 0; row < k0_; ++row)
    for (int col = 0; col < k0_; ++col)
      leaves += subtrees_[row][col].WordsCnt();
  return leaves;
}
shared_ptr<CompressedPartition> K2TreePartition::BuildCompressedTree(
    const HashTable &t,
    shared_ptr<Array<uchar>> voc) const {
  vector<vector<CompressedHybrid>> subtrees(k0_);
  for (int i = 0; i < k0_; ++i) {
    for (int j = 0; j < k0_; ++j)
  }
}

}  // namespace libk2tree 
