/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_BITS_BASIC_K2TREE_H_
#define INCLUDE_BITS_BASIC_K2TREE_H_

#include <bits/utils/bitarray.h>
#include <bits/utils/utils.h>
#include <BitSequence.h>  // libcds
#include <vector>
#include <stack>
#include <fstream>
#include <cstdlib>


namespace libk2tree {
using utils::BitArray;
using cds_static::BitSequence;
using cds_static::BitSequenceRG;
using std::vector;
using std::stack;
using std::ifstream;
using std::ofstream;
using utils::LoadValue;
using utils::SaveValue;

template<class _Size> class basic_k2treebuilder;
namespace iterators {
template<class _Impl, class _Size> class K2TreeIterator;
template<class _Size> struct DirectImpl;
template<class _Size> struct InverseImpl;
template<class _Size> class RangeIterator;
}  // namespace iterators

template<class _Size>
class basic_k2tree {
  template<typename _Impl, typename A>
  friend class iterators::K2TreeIterator;
  friend struct iterators::DirectImpl<_Size>;
  friend struct iterators::InverseImpl<_Size>;
  friend class iterators::RangeIterator<_Size>;
  friend class basic_k2treebuilder<_Size>;

 public:
  typedef iterators::K2TreeIterator<iterators::DirectImpl<_Size>, _Size>
  DirectIterator;
  typedef iterators::K2TreeIterator<iterators::InverseImpl<_Size>, _Size>
  InverseIterator;
  typedef iterators::RangeIterator<_Size>
  RangeIterator;

  /*
   * Load a K2Tree previously saved with Save(ofstream)
   */
  explicit basic_k2tree(ifstream *in);

  /* Check if exist a link from object p to q.
   * Identifiers starts with 0.
   *
   * @param p Identifier of first object.
   * @param q Identifier of second object.
   */
  bool CheckLink(_Size p, _Size q) const;

  /* 
   * Save the k2tree to a file
   */
  void Save(ofstream *out) const;

  /*
   * Method implemented for testing reasons
   */
  bool operator==(const basic_k2tree &rhs) const;

  inline _Size cnt() {
    return cnt_;
  }

  inline DirectIterator DirectBegin(_Size p) const {
    return DirectIterator(this, p);
  }
  inline DirectIterator DirectEnd() const {
    return DirectIterator::end;
  }

  inline InverseIterator InverseBegin(_Size q) const {
    return InverseIterator(this, q);
  }
  inline InverseIterator InverseEnd() const {
    return InverseIterator::end;
  }

  inline RangeIterator RangeBegin(_Size p1, _Size p2) const {
    return RangeIterator(this, p1, p2);
  }

  inline RangeIterator RangeEnd() const {
    return RangeIterator::end;
  }

  ~basic_k2tree();

 private:
  /* 
   * Construct a k2tree with and hybrid aproach. This construtor should
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
  basic_k2tree(const BitArray<unsigned int, _Size> &T,
               const BitArray<unsigned int, _Size> &L,
               int k1, int k2, int kl, int max_level_k1, int height,
               _Size cnt, _Size size);

  inline _Size GetFirstChild(_Size z, int level, int k) const {
    // child_l(x,i) = rank(T_l, z - 1)*kl*kl + i - 1;
    z = z > 0 ? (T_->rank1(z-1) - acum_rank_[level-1])*k*k : 0;
    return z + offset_[level];
  }
  // Bit array with rank capability containing internal nodes.
  BitSequence *T_;
  // Bit array for the leafs.
  BitArray<unsigned int, _Size> L_;
  // Arity of the first part.
  int k1_;
  // Arity of the second part.
  int k2_;
  // Arity of the level height-1.
  int kl_;
  // Last level with arity k1.
  int max_level_k1_;
  // Height of the tree.
  int height_;
  // Number of object.
  _Size cnt_;
  // Size of the expanded matrix.
  _Size size_;
  // Accumulated rank for each level.
  _Size *acum_rank_;
  // Accumulated number of nodes util each level, inclusive.
  _Size *offset_;

  inline int GetK(int level) const {
    if (level <= max_level_k1_)  return k1_;
    else if (level < height_ - 1)  return k2_;
    else  return kl_;
  }
};



}  // namespace libk2tree

#endif  // INCLUDE_BITS_BASIC_K2TREE_H_
