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
#include <iostream>
using namespace std;


namespace k2tree_impl {
using utils::BitArray;
using cds_static::BitSequence;
using cds_static::BitSequenceRG;
using std::vector;
using std::stack;
using std::ifstream;
using std::ofstream;
using utils::LoadValue;
using utils::SaveValue;

template<class A> class K2TreeBuilder;
template<class self_type, class A> class K2TreeIterator_;
template<class A> class DirectIterator_;
template<class A> class InverseIterator_;

template<class A>
class K2Tree {
  friend class K2TreeBuilder<A>;
  template<typename self_type, typename B>
  friend class K2TreeIterator_;
  friend class DirectIterator_<A>;
  friend class InverseIterator_<A>;
 public:
  typedef DirectIterator_<A> DirectIterator;
  typedef InverseIterator_<A> InverseIterator;

  /*
   * Load a K2Tree previously saved with Save()
   */
  explicit K2Tree(ifstream *in) :
      T_(BitSequence::load(*in)),
      L_(in),
      k1_(LoadValue<int>(in)),
      k2_(LoadValue<int>(in)),
      kl_(LoadValue<int>(in)),
      max_level_k1_(LoadValue<int>(in)),
      height_(LoadValue<int>(in)),
      size_(LoadValue<A>(in)),
      acum_rank_(LoadValue<size_t>(in, height_)) {}

  ~K2Tree() {
    delete [] acum_rank_;
    delete T_;
  }

  /* Check if exist a link from object p to q.
   * Identifiers starts with 0.
   *
   * @param p Identifier of first object.
   * @param q Identifier of second object.
   */
  bool CheckLink(A p, A q) const {
    A N, div_level;
    size_t z;
    size_t offset;  // number of nodes until current level, inclusive.
    int k;

    N = size_;
    z = offset = 0;
    for (int level = 0; level < height_; ++level) {
      k = GetK(level);

      div_level = N/k;
      if (level > 0 && T_->access(z))
        // child_l(x,i) = rank(T_l, z - 1)*kl*kl + i - 1;
        z = z > 0 ? (T_->rank1(z-1) - acum_rank_[level-1])*k*k : 0;
      else if (level > 0)
        return false;

      int child = p/div_level*k + q/div_level;
      z += child + offset;

      if (level > 0)
        offset += (acum_rank_[level] - acum_rank_[level-1])*k*k;
      else
        offset = k*k;

      N /= k, p %= div_level, q %= div_level;
    }
    return L_.GetBit(z - T_->getLength());
  }


  DirectIterator_<A> DirectBegin(A p) const {
    return DirectIterator(this, p, false);
  }
  DirectIterator_<A> DirectEnd(A p) const {
    return DirectIterator(this, p, true);
  }

  InverseIterator_<A> InverseBegin(A q) const {
    return InverseIterator(this, q, false);
  }
  InverseIterator_<A> InverseEnd(A q) const {
    return InverseIterator(this, q, true);
  }

  /* 
   * Save the k2tree to a file
   */
  void Save(ofstream *out) const {
    T_->save(*out);
    L_.Save(out);
    SaveValue(out, k1_);
    SaveValue(out, k2_);
    SaveValue(out, kl_);
    SaveValue(out, max_level_k1_);
    SaveValue(out, height_);
    SaveValue(out, size_);
    SaveValue(out, acum_rank_, height_);
  }

  /*
   * Method implemented for testing reasons
   */
  bool operator==(const K2Tree &rhs) const {
    if (T_->getLength() != rhs.T_->getLength()) return false;
    cerr << "1" << endl;
    for (size_t i = 0; i < T_->getLength(); ++i)
      if (T_->access(i) != rhs.T_->access(i)) return false;
    cerr << "2" << endl;

    if (L_.length() != rhs.L_.length()) return false;
    cerr << "3" << endl;
    for (size_t i = 0; i < L_.length(); ++i)
      if (L_.GetBit(i) != rhs.L_.GetBit(i)) return false;
    cerr << "4" << endl;

    if (height_ != rhs.height_) return false;
    cerr << "5" << endl;

    for (int i = 0; i < height_; ++i)
      if (acum_rank_[i] != acum_rank_[i]) return false;
    cerr << "6" << endl;

    return k1_ == rhs.k1_ && k2_ == rhs.k2_ && kl_ == rhs.kl_ &&
           max_level_k1_ == rhs.max_level_k1_ && size_ == rhs.size_;
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
  K2Tree(const BitArray<unsigned int> &T, const BitArray<unsigned int> &L,
         int k1, int k2, int kl, int max_level_k1, int height, A size) :
      T_(new BitSequenceRG(T.GetRawData(), T.length(), 20)),
      L_(L),
      k1_(k1),
      k2_(k2),
      kl_(kl),
      max_level_k1_(max_level_k1),
      height_(height),
      size_(size),
      acum_rank_(new size_t[height]) {
    acum_rank_[0] = 0;
    size_t acum_nodes = k1*k1;
    for (int level = 1; level < height; ++level) {
      int k = level <= max_level_k1? k1 : k2;
      acum_rank_[level] = T_->rank1(acum_nodes-1);
      acum_nodes += (acum_rank_[level]-acum_rank_[level-1])*k*k;
    }
  }
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
  A size_;
  // Accumulated rank for each level.
  size_t *acum_rank_;

  inline int GetK(int level) const {
    if (level <= max_level_k1_)  return k1_;
    else if (level < height_ - 1)  return k2_;
    else  return kl_;
  }
};


}  // namespace k2tree_impl

#endif  // INCLUDE_BITS_K2TREE_H_
