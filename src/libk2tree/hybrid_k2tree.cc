/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <hybrid_k2tree.h>
#include <compression/compressor.h>
#include <memory>

namespace libk2tree {
using utils::LoadValue;
using utils::SaveValue;
using std::make_shared;


HybridK2Tree::HybridK2Tree(const BitArray<uint> &T,
                           const BitArray<uint> &L,
                           int k1, int k2, int kl, int max_level_k1,
                           int height, uint cnt, uint size)
    : base_hybrid(T, k1, k2, kl, max_level_k1, height, cnt, size),
      L_(L) {}


HybridK2Tree::HybridK2Tree(ifstream *in)
    : base_hybrid(in),
      L_(in) {}

size_t HybridK2Tree::GetSize() const {
  size_t size = base_hybrid::GetSize();
  size += L_.GetSize();
  return size;
}

void HybridK2Tree::Save(ofstream *out) const {
  base_hybrid::Save(out);
  L_.Save(out);
}


shared_ptr<CompressedHybrid> HybridK2Tree::CompressLeaves() const {
  shared_ptr<CompressedHybrid> t;

  compression::FreqVoc(*this, [&] (const HashTable &table,
                                   shared_ptr<Vocabulary> voc) {
    t = CompressLeaves(table, voc);
  });
  return t;
}


shared_ptr<CompressedHybrid> HybridK2Tree::CompressLeaves(
    const HashTable &table,
    shared_ptr<Vocabulary> voc) const {
  uint cnt = WordsCnt();
  uint size = WordSize();
  uint *codewords = new uint[cnt];

  int i = 0;
  Words([&] (const uchar *word) {
    uint addr;
    if (!table.search(word, size, &addr)) {
      fprintf(stderr, "Word not found\n");
      exit(1);
    }
    codewords[i++] = table[addr].codeword;
  });

  FTRep *compressL = createFT(codewords, cnt);

  delete [] codewords;

  return shared_ptr<CompressedHybrid>(new CompressedHybrid(T_, compressL, voc,
                                                           k1_, k2_, kL_,
                                                           max_level_k1_,
                                                           height_,
                                                           cnt_, size_));
}



bool HybridK2Tree::operator==(const HybridK2Tree &rhs) const {
  if (T_->getLength() != rhs.T_->getLength()) return false;
  for (size_t i = 0; i < T_->getLength(); ++i)
    if (T_->access(i) != rhs.T_->access(i)) return false;

  if (L_.length() != rhs.L_.length()) return false;
  for (size_t i = 0; i < L_.length(); ++i)
    if (L_.GetBit(i) != rhs.L_.GetBit(i)) return false;

  if (height_ != rhs.height_) return false;

  for (int i = 0; i < height_ - 1; ++i)
    if (acum_rank_[i] != acum_rank_[i]) return false;

  for (int i = 0; i <= height_; ++i)
    if (offset_[i] != offset_[i]) return false;

  return k1_ == rhs.k1_ && k2_ == rhs.k2_ && kL_ == rhs.kL_ &&
         max_level_k1_ == rhs.max_level_k1_ && size_ == rhs.size_ &&
         cnt_ == rhs.cnt_;
}


}  // namespace libk2tree

