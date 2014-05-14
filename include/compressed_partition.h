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

#include <basic_partition.h>
#include <compressed_hybrid.h>

namespace libk2tree {
class CompressedPartition: public basic_partition<CompressedHybrid> {
 public:
  CompressedPartition(std::ifstream *in);
  void Save(std::ofstream *out) const;

 private:
  shared_ptr<Vocabulary> vocabulary_;
};
}  // namespace libk2tree

#endif  // INCLUDE_COMPRESSED_PARTITION_H_
