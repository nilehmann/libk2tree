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

/** 
 * <em>k<sup>2</sup></em>-tree implementation using an hybrid approach as
 * described in section 5.1.
 */
class HybridK2Tree : public base_hybrid<HybridK2Tree> {
  friend class base_hybrid<HybridK2Tree>;
 public:
  /**
   * Builds a tree with and hybrid approach using the specified data that
   * correctly represents an hybrid <em>k<sup>2</sup></em>-tree.
   *
   * @param T Bit array with the internal nodes.
   * @param L Bit array with the leafs.
   * @param k1 Arity of the first levels.
   * @param k2 Arity of the second part.
   * @param kL Arity of the level height-1.
   * @param max_level_k1 Last level with arity k1.
   * @param height Height of the tree.
   * @param cnt Number of object in the original matrix.
   * @param size Size of the expanded matrix.
   */
  HybridK2Tree(const BitArray<uint> &T,
               const BitArray<uint> &L,
               uint k1, uint k2, uint kL, uint max_level_k1, uint height,
               cnt_size cnt, cnt_size size, size_t links);

  /**
   * Loads a tree from a file.
   *
   * @param in Input stream pointing to the file storing the tree.
   * @see HybridK2Tree::Save
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
  size_t WordsCnt() const {
    return L_.length()/kL_/kL_;
  }

  /**
   * Return the number of bytes necessary to store a word.
   *
   * @return Size of a word.
   */
  uint WordSize() const {
    return Ceil(kL_*kL_, kUcharBits);
  }

  /**
   * Iterates over the words in the leaf level.
   *
   * @param fun Pointer to function, functor or lambda expecting a pointer to
   * each word.
   */
  template<typename Function>
  void Words(Function fun) const {
    size_t cnt = WordsCnt();
    uint size = WordSize();

    size_t bit = 0;
    for (size_t i = 0; i < cnt; ++i) {
      uchar *word = new uchar[size];
      std::fill(word, word + size, 0);
      for (uint j = 0; j < kL_*kL_; ++j, ++bit) {
        if (L_.GetBit(bit))
          word[j/kUcharBits] |= (uchar) (1 << (j%kUcharBits));
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
  std::shared_ptr<CompressedHybrid> CompressLeaves() const;

  /**
   * Builds a <em>k<sup>2</sup></em>-tree with the same information but
   * compressing the leaves using the specified vocabulary.
   *
   * @param table Hash table associating each word with their corresponding
   * frequency.
   * @param voc Word vocabulary sorted by frequency.
   * @return Pointer to the new tree.
   */
  std::shared_ptr<CompressedHybrid> CompressLeaves(
      const HashTable &table,
      std::shared_ptr<Vocabulary> voc) const;

 private:
  /** BitArray containing leaf nodes. */
  BitArray<uint> L_;

  /**
   * Iterates over the children in the leaf corresponding to the node  
   * specified in the given frame and calls fun reporting the object for
   * every child that is 1.
   * This function simply access the position in the bit array representing the
   * last level.
   *
   * @param f Frame containing the information required.
   * @param fun Pointer to function, functor or lambda to call for every bit
   * that is one. The function expect an unsigned int as argument.
   */
  template<typename Function, typename Impl>
  void LeafBits(const Frame &f, cnt_size div_level, Function fun) const {
    size_t z = Child(f.z, height_-1, kL_) + Impl::Offset(f, kL_, div_level);
    for (uint j  = 0; j < kL_; ++j) {
      if (L_.GetBit(z - T_->getLength()))
        fun(Impl::Output(Impl::NextFrame(f.p, f.q, z, j, div_level)));
      z = Impl::NextChild(z, kL_);
    }
  }

  /**
   * Iterates over the children in the leaf lying in the range corresponding to
   * the given frame and calls fun reporting the link for every child that is 1.
   * This function simple access the position in the bit array representing the
   * last level.
   *
   * @param f Frame containing the information required.
   * @param fun Pointer to function, functor or lambda to call for every bit
   * that is one. The function expect two unsigned int as arguments.
   */
  template<typename Function>
  void RangeLeafBits(const RangeFrame &f, cnt_size div_level,
                     Function fun) const {
    cnt_size div_p1, div_p2, div_q1, div_q2;
    cnt_size dp, dq;
    size_t first = Child(f.z, height_ - 1, kL_);

    div_p1 = f.p1/div_level, div_p2 = f.p2/div_level;
    for (cnt_size i = div_p1; i <= div_p2; ++i) {
      size_t z = first + kL_*i;
      dp = f.dp + div_level*i;

      div_q1 = f.q1/div_level, div_q2 = f.q2/div_level;
      for (cnt_size j = div_q1; j <= div_q2; ++j) {
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
  bool CheckLeafChild(size_t z, uint child) const {
    z = Child(z, height_ - 1, kL_);
    return L_.GetBit(z + child - T_->getLength());
  }
};
}  // namespace libk2tree
#endif  // INCLUDE_HYBRID_K2TREE_H_
