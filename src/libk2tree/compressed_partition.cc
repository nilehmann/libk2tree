/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <compressed_partition.h>
#include <compression/vocabulary.h>
#include <utils/utils.h>

namespace libk2tree {
using utils::LoadValue;
using utils::SaveValue;
CompressedPartition::CompressedPartition(std::ifstream *in)
    : base_partition(in),
      vocabulary_(new Vocabulary(in)) {
  for (int i = 0; i < k0_; ++i) {
    subtrees_[i].reserve(k0_);
    for (int j = 0; j < k0_; ++j)
      subtrees_[i].emplace_back(in, vocabulary_);
  }
}

void CompressedPartition::Save(std::ofstream *out) const {
  SaveValue(out, cnt_);
  SaveValue(out, submatrix_size_);
  SaveValue(out, k0_);
  vocabulary_->Save(out);

  for (int i = 0; i < k0_; ++i)
    for (int j = 0; j < k0_; ++j)
      subtrees_[i][j].Save(out, false);
}

}  // namespace libk2tree
