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


CompressedHybrid::CompressedHybrid(shared_ptr<BitSequence> T,
                                   FTRep *compressL,
                                   shared_ptr<Array<uchar>> vocabulary,
                                   int k1, int k2, int kl, int max_level_k1,
                                   int height, uint cnt, uint size)
    : basic_hybrid(T, k1, k2, kl, max_level_k1, height, cnt, size),
      compressL_(compressL),
      vocabulary_(vocabulary) {}

CompressedHybrid::CompressedHybrid(ifstream *in) 
    : basic_hybrid(in),
      compressL_(LoadFT(in)),
      vocabulary_(new Array<uchar>(in)) {}



size_t CompressedHybrid::GetSize() const {
  size_t size = T_->getSize();
  //size += L_.GetSize();
  // TODO
  size += height_*sizeof(uint);
  size += (height_+1)*sizeof(uint);
  size += 5*sizeof(int) + 2*sizeof(uint) + 2*sizeof(uint*);
  return size;
}



void CompressedHybrid::Save(ofstream *out) const {
  basic_hybrid::Save(out);
  SaveFT(out, compressL_);
  vocabulary_->Save(out);
}

CompressedHybrid::~CompressedHybrid() {
  destroyFT(compressL_);
}


}  // namespace libk2tree

