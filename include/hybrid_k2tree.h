/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_HYBRID_K2TREE_H_
#define INCLUDE_HYBRID_K2TREE_H_
#include <basic_hybrid.h>
#include <compressed_hybrid.h>
#include <compression/compressable.h>
#include <dacs.h>



namespace libk2tree {
using compression::compressable;
using compression::HashTable;
class K2TreeBuilder;


class HybridK2Tree : public basic_hybrid<HybridK2Tree>,
                     public compressable<HybridK2Tree, CompressedHybrid> {
  friend class K2TreeBuilder;
  friend class basic_hybrid<HybridK2Tree>;
  friend class compressable<HybridK2Tree, CompressedHybrid>;
 public:
  /*
   * Loads a K2Tree previously saved with Save(ofstream)
   */
  explicit HybridK2Tree(ifstream *in);

  /* 
   * Saves the k2tree to a file
   */
  void Save(ofstream *out) const;


  void Memory() const;
  size_t GetSize() const;
  bool operator==(const HybridK2Tree &rhs) const;

  /*
   * Return number of words of kl*kl bits in the leaf level.
   */
  uint WordsCnt() const {
    return L_.length()/kl_/kl_;
  }

  /*
   * Return the size in bytes of the words in the leaf level, ie, kl*kl/8.
   */
  uint WordSize() const {
    return Ceil(kl_*kl_, 8);
  }

  /*
   * Iterates over the words in the leaf level.
   * @param fun Pointer to function, functor or lambda expecting a
   * uchar *
   */
  template<class Function>
  void Words(Function fun) const {
    uint cnt = WordsCnt();
    uint size = WordSize();

    uint bit = 0;
    for (uint i = 0; i < cnt; ++i) {
      uchar *word = new uchar[size];
      std::fill(word, word + size, 0);
      for (int j = 0; j < kl_*kl_; ++j, ++bit) {
        if (L_.GetBit(bit))
          word[j/8] |= (1 << (j%8));
      }
      fun(word);
      delete [] word;
    }
  }


 private:
  /* 
   * Builds a k2tree with and hybrid aproach. This construtor should
   * be called from a proper builder.
   *
   * @param T Bit array with the internal nodes.
   * @param L Bit array with the leafs.
   * @param k1 Arity of the first levels.
   * @param k2 Arity of the second part.
   * @param kl Arity of the level height-1.
   * @param max_level_k1 Last level with arity k1.
   * @param height Height of the tree.
   * @param cnt Number of object in the relation.
   * @param size Size of the expanded matrix.
   */
  HybridK2Tree(const BitArray<uint, uint> &T,
               const BitArray<uint, uint> &L,
               int k1, int k2, int kl, int max_level_k1, int height,
               uint cnt, uint size);

  template<class Function, class Impl>
  void LeafBits(const Frame &f, uint div_level, Function fun) const {
    uint z = FirstChild(f.z, height_-1, kl_) + Impl::Offset(f, kl_, div_level);
    for (int j  = 0; j < kl_; ++j) {
      if (L_.GetBit(z - T_->getLength()))
        fun(Impl::Output(Impl::NextFrame(f.p, f.q, z, j, div_level)));
      z = Impl::NextChild(z, kl_);
    }
  }

  template<class Function>
  void RangeLeafBits(const RangeFrame &f, uint div_level, Function fun) const {
    uint div_p1, div_p2, div_q1, div_q2;
    uint dp, dq;
    uint first = FirstChild(f.z, height_ - 1, kl_);

    div_p1 = f.p1/div_level, div_p2 = f.p2/div_level;
    for (uint i = div_p1; i <= div_p2; ++i) {
      uint z = first + kl_*i;
      dp = f.dp + div_level*i;

      div_q1 = f.q1/div_level, div_q2 = f.q2/div_level;
      for (uint j = div_q1; j <= div_q2; ++j) {
        dq = f.dq + div_level*j;
        if (L_.GetBit(z+j - T_->getLength()))
          fun(dp, dq);
      }
    }

  }

  bool GetChildInLeaf(uint z, int child) const {
    z = FirstChild(z, height_ - 1, kl_);
    return L_.GetBit(z + child - T_->getLength());
  }

  shared_ptr<CompressedHybrid>
  BuildCompressed(const HashTable &table,
                  shared_ptr<Array<uchar>> voc) const {
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
    
    shared_ptr<CompressedHybrid> t(new CompressedHybrid(T_, compressL, voc,
                                                        k1_, k2_, kl_,
                                                        max_level_k1_, height_,
                                                        cnt_, size_));
    return t;
  }

  BitArray<uint, uint> L_;
};
}  // namespace libk2tree
#endif  // INCLUDE_HYBRID_K2TREE_H_
