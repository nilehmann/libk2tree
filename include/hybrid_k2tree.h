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
#include <base/base_hybrid.h>
#include <compressed_hybrid.h>
#include <compression/compressor.h>
#include <dacs.h>


namespace libk2tree {
using compression::HashTable;
class K2TreeBuilder;


/** 
 * K2Tree implementation using an hybrid aproach as described in section 5.1.
 */
class HybridK2Tree : public base_hybrid<HybridK2Tree> {
  friend class K2TreeBuilder;
  friend class base_hybrid<HybridK2Tree>;
 public:
  /**
   * Loads a K2Tree from a file.
   *
   * @param in Input stream pointing to the file storing the tree.
   */
  explicit HybridK2Tree(ifstream *in);

  /** 
   * Saves the k2tree to a file.
   *
   * @param out Output stream
   */
  void Save(ofstream *out) const;

  /**
   * Returns memory usage of the structure.
   *
   * @return Size in bytes.
   */
  size_t GetSize() const;

  /**
   * Method implemented for testing reasons.
   */
  bool operator==(const HybridK2Tree &rhs) const;

  /**
   * Returns number of words of kl*kl bits in the leaf level.
   *
   * @return Number of words.
   */
  uint WordsCnt() const {
    return L_.length()/kl_/kl_;
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

  /**
   * Iterates over the words in the leaf level.
   *
   * @param fun Pointer to function, functor or lambda expecting a pointer to
   * the first position of each word.
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

  /**
   * Construct a K2Tree with the same information but with compressed leaves.
   *
   * @return Pointer to the new tree.
   */
  shared_ptr<CompressedHybrid> CompressLeaves() const;

  /**
   * Construct a K2Tree with the same information but with compressed leaves
   * using the specified vocabulary.
   *
   * @param table Hash table asociating each words to it corresponding frequency
   * @param voc Word vocabulary sorted by frequency.
   * @return Pointer to the new tree.
   */
  shared_ptr<CompressedHybrid> CompressLeaves(
      const HashTable &table,
      shared_ptr<Vocabulary> voc) const;

 private:
  /** BitArray containing leaf nodes. */
  BitArray<uint, uint> L_;

  /* 
   * Builds a tree with and hybrid aproach using the specified data that
   * correctly represent a k2tree.
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
    uint z = FirstChild(f.z, height_-1, kl_) + Impl::Offset(f, kl_, div_level);
    for (int j  = 0; j < kl_; ++j) {
      if (L_.GetBit(z - T_->getLength()))
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

  /**
   * Check if the child of the specified nodes is 1 or 0.
   *
   * @param z Position representing the internal node.
   * @param child Number of the child.
   * @return True if the child is 1, false otherwise.
   */
  bool GetChildInLeaf(uint z, int child) const {
    z = FirstChild(z, height_ - 1, kl_);
    return L_.GetBit(z + child - T_->getLength());
  }
};
}  // namespace libk2tree
#endif  // INCLUDE_HYBRID_K2TREE_H_
