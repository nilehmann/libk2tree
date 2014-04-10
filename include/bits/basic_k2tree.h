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

template<class _Size> class basic_k2tree_builder;
template<class self_type, class _Size> class K2TreeIterator;
template<class _Size> struct DirectImpl;
template<class _Size> struct InverseImpl;
template<class _Size> class RangeIterator_;

template<class _Size>
class basic_k2tree {
  friend class basic_k2tree_builder<_Size>;
  template<typename _Impl, typename A>
  friend class K2TreeIterator;
  friend struct DirectImpl<_Size>;
  friend struct InverseImpl<_Size>;
  friend class RangeIterator_<_Size>;
 public:
  typedef K2TreeIterator<DirectImpl<_Size>, _Size> DirectIterator;
  typedef K2TreeIterator<InverseImpl<_Size>, _Size> InverseIterator;
  typedef RangeIterator_<_Size> RangeIterator;

  /*
   * Load a K2Tree previously saved with Save()
   */
  explicit basic_k2tree(ifstream *in);

  ~basic_k2tree();

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

  DirectIterator DirectBegin(_Size p) const {
    return DirectIterator(this, p, false);
  }
  DirectIterator DirectEnd() const {
    return DirectIterator::end;
  }

  InverseIterator InverseBegin(_Size q) const {
    return InverseIterator(this, q, false);
  }
  InverseIterator InverseEnd() const {
    return InverseIterator::end;
  }

  RangeIterator RangeBegin(_Size p1, _Size p2, _Size q1, _Size q2) const {
    return RangeIterator(this, p1, p2, q1, q2, false);
  }

  RangeIterator RangeEnd() const {
    return RangeIterator::end;
  }


 private:
  /* 
   * Construct a k2tree with and hybrid aproach.
   *
   * @param T Bit array with the internal nodes
   * @param L Bit array with the leafs
   * @param k1 Arity of the first levels
   * @param k2 Arity of the second part
   * @param kl Arity of the level height-1
   * @param max_level_k1 Las level with arity k1
   * @param height Height of the k2tree
   * @param size Size of the expanded matrix
   */
  basic_k2tree(const BitArray<unsigned int, _Size> &T,
               const BitArray<unsigned int, _Size> &L,
               int k1, int k2, int kl, int max_level_k1, int height,
               _Size cnt, _Size size);
  // Bit array containing the nodes of internal nodes
  BitSequence *T_;
  // Bit array for the leafs.
  BitArray<unsigned int, _Size> L_;
  // Arity of the first part.
  int k1_;
  // Arity of the second part.
  int k2_;
  // Arity of the level height-1.
  int kl_;
  // Last level with arity k1
  int max_level_k1_;
  // height of the tree
  int height_;
  // Number of object
  _Size cnt_;
  // Size of the expanded matrix
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
