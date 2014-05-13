/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_K2TREE_PARTITION_H_
#define INCLUDE_K2TREE_PARTITION_H_

#include <libk2tree_basic.h>
#include <basic_partition.h>
#include <hybrid_k2tree.h>
#include <compressed_partition.h>


namespace libk2tree {
class K2TreePartition
    : public basic_partition<HybridK2Tree>,
      public compressable<K2TreePartition, void> {
 public:
  K2TreePartition(std::ifstream *in);
  
  /*
   * Return number of words of size kl*kl in the leaf level.
   */
  uint WordsCnt() const;

  /*
   * Return the size in bytes of the words in the leaf level, ie, kl*kl/8.
   */
  int WordSize() const {
   return subtrees_[0][0].WordSize();
  }

  /*
   * Iterates over the words in the leaf level.
   * @param fun Pointer to function, functor or lambda expecting a uchar *.
   */
  template<class Function>
  void Words(Function fun) const {
    for (int row = 0; row < k0_; ++row)
      for (int col = 0; col < k0_; ++col)
        subtrees_[row][col].Words(fun);
  }

  shared_ptr<CompressedPartition> BuildCompressedTree(
      const HashTable &t,
      shared_ptr<Array<uchar>> voc) const;
};

}  // namespace libk2tree
#endif  // INCLUDE_K2TREE_PARTITION_H_
