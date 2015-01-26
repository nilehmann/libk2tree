/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <compressed_hybrid.h>

namespace libk2tree {
using utils::LoadValue;
using utils::SaveValue;


CompressedHybrid::CompressedHybrid(std::shared_ptr<BitSequence> T,
                                   FTRep *compressL,
                                   std::shared_ptr<Vocabulary> vocabulary,
                                   uint k1, uint k2, uint kL,
                                   uint max_level_k1, uint height,
                                   cnt_size cnt, cnt_size size, size_t links)
    : base_hybrid(T, k1, k2, kL, max_level_k1, height, cnt, size, links),
      compressL_(compressL),
      vocabulary_(vocabulary) {}

CompressedHybrid::CompressedHybrid(ifstream *in)
    : base_hybrid(in),
      compressL_(LoadFT(in)),
      vocabulary_(new Vocabulary(in)) {}

CompressedHybrid::CompressedHybrid(ifstream *in,
                                   std::shared_ptr<Vocabulary> voc)
    : base_hybrid(in),
      compressL_(LoadFT(in)),
      vocabulary_(voc) {}



size_t CompressedHybrid::GetSize() const {
  size_t size = base_hybrid::GetSize();
  // TODO(nlehmann): Size of FTRep
  return size;
}



void CompressedHybrid::Save(ofstream *out, bool save_voc) const {
  base_hybrid::Save(out);
  SaveFT(out, compressL_);
  if (save_voc)
    vocabulary_->Save(out);
}

CompressedHybrid::~CompressedHybrid() {
  destroyFT(compressL_);
}

bool CompressedHybrid::operator==(const CompressedHybrid &rhs) const {
  if (T_->GetLength() != rhs.T_->GetLength()) return false;

  for (size_t i = 0; i < T_->GetLength(); ++i)
    if (T_->Access(i) != rhs.T_->Access(i)) return false;


  if (!equalsFT(compressL_, rhs.compressL_))
    return false;

  if (!( *vocabulary_ == *rhs.vocabulary_))
    return false;


  if (height_ != rhs.height_) return false;

  for (uint i = 0; i < height_ - 1; ++i)
    if (acum_rank_[i] != acum_rank_[i]) return false;

  for (uint i = 0; i <= height_; ++i)
    if (offset_[i] != offset_[i]) return false;

  return k1_ == rhs.k1_ && k2_ == rhs.k2_ && kL_ == rhs.kL_ &&
         max_level_k1_ == rhs.max_level_k1_ && size_ == rhs.size_ &&
         cnt_ == rhs.cnt_;
}

}  // namespace libk2tree

