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
                           uint k1, uint k2, uint kl,
                           uint max_level_k1, uint height,
                           cnt_size cnt, cnt_size size, size_t links)
    : base_hybrid(T, k1, k2, kl, max_level_k1, height, cnt, size, links),
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


std::shared_ptr<CompressedHybrid> HybridK2Tree::CompressLeaves() const {
  std::shared_ptr<CompressedHybrid> t;

  compression::FreqVoc(*this, [&] (const HashTable &table,
                                   std::shared_ptr<Vocabulary> voc) {
    t = CompressLeaves(table, voc);
  });
  return t;
}


std::shared_ptr<CompressedHybrid> HybridK2Tree::CompressLeaves(
    const HashTable &table,
    std::shared_ptr<Vocabulary> voc) const {
  size_t cnt = WordsCnt();
  uint size = WordSize();
  uint *codewords;
  try {
    codewords = new uint[cnt];
  } catch (std::bad_alloc ba) {
    std::cerr << "[HybridK2Tree::CompressLeaves] Error: " << ba.what() << "\n";
    exit(1);
  }

  size_t i = 0;
  Words([&] (const uchar *word) {
    size_t addr;
    if (!table.search(word, size, &addr)) {
      std::cerr << "[HybridK2Tree::CompressLeaves] Error: Word not found\n";
      exit(1);
    }
    codewords[i++] = table[addr].codeword;
  });

  FTRep *compressL;
  try {
    // TODO Port to 64-bits
    compressL = createFT(codewords, cnt);
  } catch (...) {
    std::cerr << "[HybridK2Tree::CompressLeaves] Error: Could not create DAC\n";
    exit(1);
  }

  delete [] codewords;

  return std::shared_ptr<CompressedHybrid>(
      new CompressedHybrid(T_, compressL, voc,
                           k1_, k2_, kL_,
                           max_level_k1_,
                           height_,
                           cnt_, size_,
                           links_)
      );
}

bool HybridK2Tree::operator==(const HybridK2Tree &rhs) const {
  if (T_->GetLength() != rhs.T_->GetLength()) return false;
  for (size_t i = 0; i < T_->GetLength(); ++i)
    if (T_->Access(i) != rhs.T_->Access(i)) return false;

  if (L_.length() != rhs.L_.length()) return false;
  for (size_t i = 0; i < L_.length(); ++i)
    if (L_.GetBit(i) != rhs.L_.GetBit(i)) return false;

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

