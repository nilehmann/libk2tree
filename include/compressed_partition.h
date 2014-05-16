/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_COMPRESSED_PARTITION_H_
#define INCLUDE_COMPRESSED_PARTITION_H_

#include <base/base_partition.h>
#include <compressed_hybrid.h>

namespace libk2tree {

/**
 * K2Tree implementation partitioning the first level as described in section
 * 5.2. This representation use hybrid k2trees with compressed leaves as
 * substrees.
 */
class CompressedPartition: public base_partition<CompressedHybrid> {
 public:
  /**
   * Loads a tree from a file
   *
   * @param in Input stream pointing to the file storing the tree.
   */
  explicit CompressedPartition(std::ifstream *in);

  /**
   * Saves tree to file
   *
   * @param out Output stream.
   */
  void Save(std::ofstream *out) const;

 private:
  /** Vocabulary of the leaves. This vocabulary is shared by all subtrees */
  shared_ptr<Vocabulary> vocabulary_;
};
}  // namespace libk2tree

#endif  // INCLUDE_COMPRESSED_PARTITION_H_
