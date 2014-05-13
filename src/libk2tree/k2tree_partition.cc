/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2tree_partition.h>
#include <compressed_partition.h>
#include <compression/compressor.h>

namespace libk2tree {
K2TreePartition::K2TreePartition(std::ifstream *in) : basic_partition(in) {}

uint K2TreePartition::WordsCnt() const {
  size_t leaves = 0;
  for (int i = 0; i < k0_; ++i)
    for (int j = 0; j < k0_; ++j)
      leaves += subtrees_[i][j].WordsCnt();
  return leaves;
}

void K2TreePartition::CompressLeaves(std::ofstream *out) const {
  SaveValue(out, cnt_);
  SaveValue(out, submatrix_size_);
  SaveValue(out, k0_);

  /*compression::CompressLeaves(*this, [&] (const HashTable &table,
                                          shared_ptr<Array<uchar>> voc) {
    for (int i = 0; i < k0_; ++i) {
      for (int j = 0; j < k0_; ++j) {
        shared_ptr<CompressedHybrid> t = subtrees_[i][j].BuildCompressed(table, voc);
        t->Save(out);
      }
    }
  });*/
  for (int i = 0; i < k0_; ++i) {
    for (int j = 0; j < k0_; ++j) {
      shared_ptr<CompressedHybrid> t = subtrees_[i][j].CompressLeaves();
      t->Save(out);
      //subtrees_[i][j].Save(out);
    }
  }
}

}  // namespace libk2tree 
