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
#include <base/base_partition.h>
#include <hybrid_k2tree.h>


namespace libk2tree {
/**
 * <em>k<sup>2</sup></em>tree implementation partitioning the first level as
 * described in section 5.2.
 * This representation use hybrid <em>k<sup>2</sup></em>trees without compressed
 * leaves as substrees.
 */
class K2TreePartition: public base_partition<HybridK2Tree> {
 public:
  /**
   * Loads tree from a file
   *
   * @param in Input stream.
   * @see K2TreePartition::Save
   * @see K2TreePartitionBuilder::K2TreePartitionBuilder
   */
  explicit K2TreePartition(std::ifstream *in);

  /**
   * Returns number of words of size kL*kL in the leaf level.
   *
   * @return Number of words.
   */
  uint WordsCnt() const;

  /**
   * Return the number of unsigned chars needed to store a word in the leaf
   * level, ie, kL*kL/(8*sizeof(uchar)).
   *
   * @return Size of the words.
   */
  uint WordSize() const {
    return subtrees_[0][0].WordSize();
  }

  /**
   * Iterates over the words in the leaf level.
   *
   * @param fun Pointer to function, functor or lambda expecting a pointer to
   * first position of each word.
   */
  template<class Function>
  void Words(Function fun) const {
    for (int row = 0; row < k0_; ++row)
      for (int col = 0; col < k0_; ++col)
        subtrees_[row][col].Words(fun);
  }

  /**
   * Constructs a new tree with the same information but compressing the leafs
   * of each subtree. A common vocabulary including all words is used to
   * compress each subtree.
   *
   * @param out Output stream to store the resulting tree.
   */
  void CompressLeaves(std::ofstream *out) const;

  /**
   * Saves tree to file
   *
   * @param out Output stream.
   */
  void Save(std::ofstream *out) const;
};

}  // namespace libk2tree
#endif  // INCLUDE_K2TREE_PARTITION_H_
