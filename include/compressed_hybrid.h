/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 *
 */

#ifndef INCLUDE_COMPRESSED_HYBRID_H_
#define INCLUDE_COMPRESSED_HYBRID_H_

#include <libk2tree_basic.h>
#include <base/base_hybrid.h>
#include <compression/vocabulary.h>
#include <dacs.h>
#include <memory>


namespace libk2tree {
using std::shared_ptr;
using compression::Vocabulary;

/**
 * K2Tree implementation using an hybrid aproach and compressed leaves as
 * described in section 5.1 and 5.3.
 */
class CompressedHybrid: public base_hybrid<CompressedHybrid> {
  friend class base_hybrid<CompressedHybrid>;
  friend class HybridK2Tree;
 public:
  /**
   * Loads a K2Tree from a file.
   *
   * @param in Input stream pointing to the file storing the tree.
   * @see Save(ofstream*out, bool save_voc = true) const
   */
  explicit CompressedHybrid(ifstream *in);

  /**
   * Creates a K2Tree loading it from a file but using voc as vocabulary.
   *
   * @param in Input stream pointing to the file storing the tree.
   * @param voc Vocabulary for the leaf.
   * @see Save(ofstream*out, bool save_voc = true) const
   */
  CompressedHybrid(ifstream *in, shared_ptr<Vocabulary> voc);

  /** 
   * Saves the k2tree to a file.
   *
   * @param out Stream poiting to file.
   * @param save_voc Wheter or not to save the vocabulary.
   */
  void Save(ofstream *out, bool save_voc = true) const;


  /**
   * Returns memory usage of the structure.
   *
   * @return Size in bytes
   */
  size_t GetSize() const;

  /**
   * Method implemented for testing reasons
   */
  bool operator==(const CompressedHybrid &rhs) const;


  ~CompressedHybrid();

 private:
  /** Compressed representation of the leafs using dacs */
  FTRep * compressL_;
  /** Pointer to vocabulary */
  shared_ptr<Vocabulary> vocabulary_;

  /** 
   * Builds a tree with the specified data that correctly represent a k2tree.
   *
   * @param T Bit Sequence storing the internal nodes. Multiple instances
   * can share the same sequence.
   * @param compressL Compressed representation of the last level using direct
   * addressable codes.
   * @param vocabulary Pointer to vocabulary of the leafs.
   * @param k1 Arity of the first levels.
   * @param k2 Arity of the second part.
   * @param kl Arity of the level height-1.
   * @param max_level_k1 Last level with arity k1.
   * @param height Height of the tree.
   * @param cnt Number of object in the relation represented by the tree.
   * @param size Size of the expanded matrix.
   */
  CompressedHybrid(shared_ptr<BitSequence> T,
                   FTRep *compressL,
                   shared_ptr<Vocabulary> vocabulary,
                   int k1, int k2, int kl, int max_level_k1, int height,
                   uint cnt, uint size);

  /**
   * Returns word containing the bit at the given position
   * @param pos Position of the bit in the complete sequence of bit of the last
   * level
   * @return Pointer to the the first position of the word.
   */
  const uchar *getWord(uint pos) const {
    uint iword = accessFT(compressL_, pos/(kl_*kl_));
    return vocabulary_->get(iword);
  }

  /**
   * Iterate over the childs in the leaf corresponding to the information in 
   * the specified frame and calls fun for every child that is 1.
   *
   * @param f Frame containing the information required.
   * @param fun Pointer to function, functor or lambda to call for every bit
   * that is one. The function expect a unsigned int as argument.
   */
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

  /**
   * Iterates over the childs in the leaf corresponding to the information in
   * the specified frame and calls fun for every child that is 1.
   *
   * @param f Frame containing the information required.
   * @param fun Pointer to function, functor or lambda to call for every bit
   * that is one. The function expect two unsigned int as arguments.
   */
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

  /**
   * Check if the child of the specified nodes is 1 or 0.
   *
   * @param z Position representing the internal node.
   * @param child Number of the child.
   * @return True if the child is 1, false otherwise.
   */
  bool GetChildInLeaf(uint z, int child) const {
    z = FirstChild(z, height_ - 1, kl_);
    const uchar *word = getWord(z - T_->getLength());
    return (word[child/8] >> (child%8)) & 1;
  }

  /**
   * Return the number of unsigned chars necesary to store the words 
   * in the leaf level, ie, kl*kl/(8*sizeof(uchar))
   *
   * @return Size of the words.
   */
  uint WordSize() const {
    return Ceil(kl_*kl_, 8);
  }
};
}  // namespace libk2tree
#endif  // INCLUDE_COMPRESSED_HYBRID_H_
