/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_COMPRESSED_HYBRID_H_
#define INCLUDE_COMPRESSED_HYBRID_H_

#include <libk2tree_basic.h>
#include <basic_hybrid.h>
#include <compression/array.h>
#include <dacs.h>
#include <memory>


namespace libk2tree {
using std::shared_ptr;
using compression::Array;

class CompressedHybrid: public basic_hybrid<CompressedHybrid> {
  friend class basic_hybrid<CompressedHybrid>;
  friend class HybridK2Tree;
 public:
  /*
   * Loads a K2Tree previously saved with Save(ofstream)
   */
  explicit CompressedHybrid(ifstream *in);

  /* 
   * Saves the k2tree to a file
   */
  void Save(ofstream *out) const;


  void Memory() const;
  size_t GetSize() const;
  bool operator==(const CompressedHybrid &rhs) const;


  ~CompressedHybrid();

 private:
  FTRep * compressL_;
  shared_ptr<Array<uchar> > vocabulary_;

  /* 
   * Builds a k2tree with and hybrid aproach. This construtor should
   * be called from a proper builder.
   *
   * @param T Bit Sequence with the internal nodes.
   * @param k1 Arity of the first levels.
   * @param k2 Arity of the second part.
   * @param kl Arity of the level height-1.
   * @param max_level_k1 Last level with arity k1.
   * @param height Height of the tree.
   * @param cnt Number of object in the relation.
   * @param size Size of the expanded matrix.
   */
  CompressedHybrid(shared_ptr<BitSequence> T,
                   FTRep *compressL,
                   shared_ptr<Array<uchar>> vocabulary,
                   int k1, int k2, int kl, int max_level_k1, int height,
                   uint cnt, uint size);

  /*
   * Returns word containing the bit at the given position
   */
  const uchar *getWord(uint pos) const {
    uint size = WordSize();
    uint iword = accessFT(compressL_, pos/(size*8));
    return vocabulary_->get(iword);
  }

  template<class Function, class Impl>
  void LeafBits(const Frame &f, uint div_level, Function fun) const {
    uint first = FirstChild(f.z, height_ - 1, kl_);

    const uchar *word = getWord(first - T_->getLength());

    uint z = first + Impl::Offset(f, kl_, div_level);
    for (int j  = 0; j < kl_; ++j) {
      uint pos = z - first;
      if ((word[pos/8] >> (pos%8)) & 1)
        fun(Impl::Output(Impl::NextFrame(f.p, f.q, z, j, div_level)));
      z = Impl::NextChild(z, kl_);
    }
  }

  template<class Function>
  void RangeLeafBits(const RangeFrame &f, uint div_level, Function fun) const {
    uint div_p1, div_p2, div_q1, div_q2;
    uint dp, dq;
    uint first = FirstChild(f.z, height_ - 1, kl_);

    const uchar *word = getWord(first - T_->getLength());

    div_p1 = f.p1/div_level, div_p2 = f.p2/div_level;
    for (uint i = div_p1; i <= div_p2; ++i) {
      uint z = first + kl_*i;
      dp = f.dp + div_level*i;

      div_q1 = f.q1/div_level, div_q2 = f.q2/div_level;
      for (uint j = div_q1; j <= div_q2; ++j) {
        dq = f.dq + div_level*j;
        uint pos = z + j - first;
        if ((word[pos/8] >> (pos%8)) &1)
          fun(dp, dq);
      }
    }
  }

  bool GetChildInLeaf(uint z, int child) const {
    z = FirstChild(z, height_ - 1, kl_);
    const uchar *word = getWord(z - T_->getLength());
    return (word[child/8] >> (child%8)) & 1;
  }

  /*
   * Return the size in bytes of the words in the leaf level, ie, kl*kl/8.
   */
  uint WordSize() const {
    return Ceil(kl_*kl_, 8);
  }
};
}  // namespace libk2tree
#endif  // INCLUDE_COMPRESSED_HYBRID_H_
