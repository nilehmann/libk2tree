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
 * <em>k<sup>2</sup></em>-tree implementation using an hybrid aproach as
 * described in section 5.1.
 */
class HybridK2Tree : public base_hybrid<HybridK2Tree> {
  friend class K2TreeBuilder;
  friend class base_hybrid<HybridK2Tree>;
 public:
  /**
   * Loads a tree from a file.
   *
   * @param in Input stream pointing to the file storing the tree.
   */
  explicit HybridK2Tree(ifstream *in);

  /** 
   * Saves the tree to a file.
   *
   * @param out Output stream
   */
  void Save(ofstream *out) const;

  /**
   * Returns memory usage.
   *
   * @return Size in bytes.
   */
  size_t GetSize() const;

  /**
   * Method implemented for testing reasons.
   */
  bool operator==(const HybridK2Tree &rhs) const;

  /**
   * Returns the number of words of \f$k_l^2\f$ bits in the leaf level.
   *
   * @return Number of words.
   */
  uint WordsCnt() const {
    return L_.length()/kl_/kl_;
  }

  /**
   * Return the number of unsigned chars necesary to store a word
   * in the leaf level.
   *
   * @return Size of a word.
   */
  uint WordSize() const {
    return Ceil(kl_*kl_, kUcharBits);
  }

  /**
   * Iterates over the words in the leaf level.
   *
   * @param fun Pointer to function, functor or lambda expecting a pointer to
   * the first position of each word.
   */
  template<typename Function>
  void Words(Function fun) const {
    uint cnt = WordsCnt();
    uint size = WordSize();

    uint bit = 0;
    for (uint i = 0; i < cnt; ++i) {
      uchar *word = new uchar[size];
      std::fill(word, word + size, 0);
      for (int j = 0; j < kl_*kl_; ++j, ++bit) {
        if (L_.GetBit(bit))
          word[j/kUcharBits] |= (1 << (j%kUcharBits));
      }
      fun(word);
      delete [] word;
    }
  }

  /**
   * Builds a <em>k<sup>2</sup></em>-tree with the same information but
   * compressing the leaves.
   *
   * @return Pointer to the new tree.
   */
  shared_ptr<CompressedHybrid> CompressLeaves() const;

  /**
   * Builds a <em>k<sup>2</sup></em>-tree with the same information but
   * compressing the leaves and using the specified vocabulary.
   *
   * @param table Hash table asociating each word with their corresponding
   * frequency.
   * @param voc Word vocabulary sorted by frequency.
   * @return Pointer to the new tree.
   */
  shared_ptr<CompressedHybrid> CompressLeaves(
      const HashTable &table,
      shared_ptr<Vocabulary> voc) const;

 private:
  /** BitArray containing leaf nodes. */
  BitArray<uint> L_;

  /**
   * Builds a tree with and hybrid aproach using the specified data that
   * correctly represents an hybrid <em>k<sup>2</sup></em>-tree.
   *
   * @param T Bit array with the internal nodes.
   * @param L Bit array with the leafs.
   * @param k1 Arity of the first levels.
   * @param k2 Arity of the second part.
   * @param kl Arity of the level height-1.
   * @param max_level_k1 Last level with arity k1.
   * @param height Height of the tree.
   * @param cnt Number of object in the original matrix.
   * @param size Size of the expanded matrix.
   */
  HybridK2Tree(const BitArray<uint> &T,
               const BitArray<uint> &L,
               int k1, int k2, int kl, int max_level_k1, int height,
               uint cnt, uint size);

  /**
   * Iterates over the leaf children corresponding to the information in 
   * the specified frame and calls fun for every child that is 1.
   *
   * @param f Frame containing the information required.
   * @param fun Pointer to function, functor or lambda to call for every bit
   * that is one. The function expect an unsigned int as argument.
   */
  template<typename Function, typename Impl>
  void LeafBits(const Frame &f, uint div_level, Function fun) const {
    uint z = Child(f.z, height_-1, kl_) + Impl::Offset(f, kl_, div_level);
    for (int j  = 0; j < kl_; ++j) {
      if (L_.GetBit(z - T_->getLength()))
        fun(Impl::Output(Impl::NextFrame(f.p, f.q, z, j, div_level)));
      z = Impl::NextChild(z, kl_);
    }
  }

  /**
   * Iterates over the leaf children corresponding to the information in
   * the specified frame and calls fun for every child that is 1.
   *
   * @param f Frame containing the information required.
   * @param fun Pointer to function, functor or lambda to call for every bit
   * that is one. The function expect two unsigned int as arguments.
   */
  template<typename Function>
  void RangeLeafBits(const RangeFrame &f, uint div_level, Function fun) const {
    uint div_p1, div_p2, div_q1, div_q2;
    uint dp, dq;
    uint first = Child(f.z, height_ - 1, kl_);

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
   * @param z Position in T representing the internal node.
   * @param child Number of the child.
   * @return True if the child is 1, false otherwise.
   */
  bool CheckLeafChild(uint z, int child) const {
    z = Child(z, height_ - 1, kl_);
    return L_.GetBit(z + child - T_->getLength());
  }
};
}  // namespace libk2tree
#endif  // INCLUDE_HYBRID_K2TREE_H_
