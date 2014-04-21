/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2treepartition_builder.h>
#include <memory>
#include <exception>
#include <cstdio>

namespace libk2tree {
using utils::Ceil;
using std::shared_ptr;
using boost::filesystem::unique_path;
using boost::filesystem::rename;

K2TreePartitionBuilder::K2TreePartitionBuilder(unsigned int cnt,
                                               unsigned int submatrix_size,
                                               int k1, int k2, int kl,
                                               int k1_levels,
                                               const path &file) :
    cnt_(cnt),
    submatrix_size_(submatrix_size),
    k0_(Ceil(cnt, submatrix_size)),
    row_(0),
    col_(0),
    ready_(false),
    builder_(submatrix_size_, k1, k2, kl, k1_levels),
    tmp_(unique_path(file.parent_path() / "%%%%%" )),
    file_(file),
    out_(tmp_.native()) {
}


void K2TreePartitionBuilder::AddLink(unsigned int p, unsigned int q) {
  assert(p >= row_*submatrix_size_ && p < (row_+1)*submatrix_size_);
  assert(q >= col_*submatrix_size_ && q < (col_+1)*submatrix_size_);
  if (Ready())
    throw logic_error("AddLink: Construction is ready");
  builder_.AddLink(p - row_ * submatrix_size_, q - col_ * submatrix_size_); 
}

void K2TreePartitionBuilder::BuildSubtree() {
  if (Ready())
    throw logic_error("BuildSubmatrix: Construction is ready");
  shared_ptr<K2Tree> tree = builder_.Build();
  builder_.Clear();
  tree->Save(&out_);

  ++col_;
  if (col_ >= k0_) {
    col_ = 0;
    ++row_;
  }
  if (row_ >= k0_) {
    ready_ = true;
    out_.close();
    rename(tmp_, file_);
  }
}

K2TreePartitionBuilder::~K2TreePartitionBuilder() {
  out_.close();
  //remove(tmp_);
}

}  // namespace libk2tree