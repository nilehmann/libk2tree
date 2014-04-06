/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_BITS_K2TREE_H_
#define INCLUDE_BITS_K2TREE_H_

#include <bits/utils/bitarray.h>
#include <bits/utils/utils.h>
#include <BitSequence.h>  // libcds
#include <vector>
#include <stack>
#include <fstream>


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

template<class _Obj> class basic_k2tree_builder;
template<class self_type, class _Obj> class K2TreeIterator_;
template<class _Obj> class DirectIterator_;
template<class _Obj> class InverseIterator_;
template<class _Obj> class RangeIterator_;

template<class _Obj>
class basic_k2tree {
  friend class basic_k2tree_builder<_Obj>;
  template<typename self_type, typename B>
  friend class K2TreeIterator_;
  friend class DirectIterator_<_Obj>;
  friend class InverseIterator_<_Obj>;
  friend class RangeIterator_<_Obj>;
 public:
  typedef DirectIterator_<_Obj> DirectIterator;
  typedef InverseIterator_<_Obj> InverseIterator;
  typedef RangeIterator_<_Obj> RangeIterator;

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
  bool CheckLink(_Obj p, _Obj q) const;

  /* 
   * Save the k2tree to a file
   */
  void Save(ofstream *out) const;

  /*
   * Method implemented for testing reasons
   */
  bool operator==(const basic_k2tree &rhs) const;

  DirectIterator DirectBegin(_Obj p) const {
    return DirectIterator(this, p, false);
  }
  DirectIterator DirectEnd() const {
    return DirectIterator(this, 0, true);
  }

  InverseIterator InverseBegin(_Obj q) const {
    return InverseIterator(this, q, false);
  }
  InverseIterator InverseEnd() const {
    return InverseIterator(this, 0, true);
  }

  RangeIterator RangeBegin(_Obj p1, _Obj p2, _Obj q1, _Obj q2) const {
    return RangeIterator(this, p1, p2, q1, q2, false);
  }

  RangeIterator RangeEnd() const {
    return RangeIterator(this, 0, 0, 0, 0, true);
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
  basic_k2tree(const BitArray<unsigned int> &T, const BitArray<unsigned int> &L,
               int k1, int k2, int kl, int max_level_k1, int height, _Obj size);
  // Bit array containing the nodes of internal nodes
  BitSequence *T_;
  // Bit array for the leafs.
  BitArray<unsigned int> L_;
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
  // Size of the expanded matrix
  _Obj size_;
  // Accumulated rank for each level.
  size_t *acum_rank_;

  int GetK(int level) const {
    if (level <= max_level_k1_)  return k1_;
    else if (level < height_ - 1)  return k2_;
    else  return kl_;
  }
};
typedef basic_k2tree<unsigned int> K2Tree;

}  // namespace libk2tree

#endif  // INCLUDE_BITS_K2TREE_H_
