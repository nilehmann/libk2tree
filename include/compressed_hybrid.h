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
using compression::Vocabulary;

/**
 * <em>k<sup>2</sup></em>tree implementation with an hybrid approach and
 * compressed leaves as described in section 5.1 and 5.3.
 */
class CompressedHybrid: public base_hybrid<CompressedHybrid> {
  friend class base_hybrid<CompressedHybrid>;
 public:
  /** 
   * Builds a tree with the specified data that correctly represent a
   * <em>k<sup>2</sup></em>tree.
   *
   * @param T Bit Sequence storing the internal nodes. Multiple instances
   * can share the same sequence.
   * @param compressL Compressed representation of the last level using direct
   * addressable codes.
   * @param vocabulary Pointer to vocabulary of the leafs. Multiple instances
   * can share the same vocabulary.
   * @param k1 Arity of the first levels.
   * @param k2 Arity of the second part.
   * @param kL Arity of the level height-1.
   * @param max_level_k1 Last level with arity k1.
   * @param height Height of the tree.
   * @param cnt Number of object in the relation represented by the tree.
   * @param size Size of the expanded matrix.
   */
  CompressedHybrid(shared_ptr<BitSequence> T,
                   FTRep *compressL,
                   std::shared_ptr<Vocabulary> vocabulary,
                   uint k1, uint k2, uint kL, uint max_level_k1, uint height,
                   cnt_size cnt, cnt_size size, size_t links);

  /**
   * Loads a tree from a file. If the file doesn't contain
   * a valid representation this function yields an undefined behaviour.
   *
   * @param in Input stream pointing to the file storing the tree.
   * @see CompressedHybrid::Save
   */
  explicit CompressedHybrid(ifstream *in);

  /**
   * Creates a tree loading it from a file but using the specified vocabulary.
   * This method is used when a series of trees share the same vocabulary.
   *
   * @param in Input stream pointing to the file storing the tree.
   * @param voc Vocabulary of the leaf level.
   * @see CompressedHybrid::Save
   */
  CompressedHybrid(ifstream *in, shared_ptr<Vocabulary> voc);

  /** 
   * Saves the tree to a file.
   *
   * @param out Stream pointing to file.
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
  std::shared_ptr<Vocabulary> vocabulary_;

  /**
   * Returns word containing the bit at the given position
   * It access the corresponding word in the DAC.
   *
   * @param pos Position in the complete sequence of bit of the last level.
   * @return Pointer to the first position of the word.
   */
  const uchar *GetWord(size_t pos) const {
    // TODO Port to 64-bits
    uint iword = accessFT(compressL_, pos/(kL_*kL_));
    return vocabulary_->get(iword);
  }

  /**
   * Iterates over the children in the leaf corresponding to the node  
   * specified in the given frame and calls fun reporting the object for
   * every child that is 1.
   * This function makes one access to the DAC to obtain the word containing
   * the \a kL_<sup>2</sup> children.
   *
   * @param f Frame containing the information required.
   * @param fun Pointer to function, functor or lambda to call for every bit
   * that is one. The function expect a unsigned int as argument.
   */
  template<class Function, class Impl>
  void LeafBits(const Frame &f, Divider<cnt_size> div_level,
                Function fun) const {
    size_t first = Child(f.z, height_ - 1, kL_);

    const uchar *word = GetWord(first - T_->getLength());

    size_t z = first + Impl::Offset(f, kL_, div_level);
    for (uint j = 0; j < kL_; ++j) {
      size_t pos = z - first;
      if ((word[pos/kUcharBits] >> (pos%kUcharBits)) & 1)
        fun(Impl::Output(Impl::NextFrame(f.p, f.q, z, j, div_level)));
      z = Impl::NextChild(z, kL_);
    }
  }

  /**
   * Iterates over the children in the leaf lying in the range corresponding to
   * the given frame and calls fun reporting the link for every child that is 1.
   * This function makes one access to the DAC to obtain the word containing
   * the \a kL_<sup>2</sup> children.
   *
   * @param f Frame containing the information required.
   * @param fun Pointer to function, functor or lambda to call for every bit
   * that is one. The function expect two unsigned int as arguments.
   */
  template<class Function>
  void RangeLeafBits(const RangeFrame &f, Divider<cnt_size> div_level,
                     Function fun) const {
    cnt_size div_p1, div_p2, div_q1, div_q2;
    cnt_size dp, dq;
    size_t first = Child(f.z, height_ - 1, kL_);

    const uchar *word = GetWord(first - T_->getLength());

    div_p1 = f.p1/div_level, div_p2 = f.p2/div_level;
    for (cnt_size i = div_p1; i <= div_p2; ++i) {
      size_t z = first + kL_*i;
      dp = f.dp + (cnt_size) div_level*i;

      div_q1 = f.q1/div_level, div_q2 = f.q2/div_level;
      for (cnt_size j = div_q1; j <= div_q2; ++j) {
        dq = f.dq + (cnt_size) div_level*j;
        size_t pos = z + j - first;
        if ((word[pos/kUcharBits] >> (pos%kUcharBits)) &1)
          fun(dp, dq);
      }
    }
  }

  /**
   * Check if a child of the specified nodes is 1 or 0.
   *
   * @param z Position representing the internal node.
   * @param child Number of the child.
   *
   * @return True if the child is 1, false otherwise.
   */
  bool CheckLeafChild(size_t z, uint child) const {
    z = Child(z, height_ - 1, kL_);
    const uchar *word = GetWord(z - T_->getLength());
    return (word[child/kUcharBits] >> (child%kUcharBits)) & 1;
  }

  /**
   * Return the number of unsigned chars necessary to store a word 
   * in the leaf level, ie, \f$\frac{k_L^2}{kUcharBits}\f$
   *
   * @return Size of the words.
   */
  uint WordSize() const {
    return Ceil(kL_*kL_, kUcharBits);
  }
};
}  // namespace libk2tree
#endif  // INCLUDE_COMPRESSED_HYBRID_H_
