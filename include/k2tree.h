/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_K2TREE_H_
#define INCLUDE_K2TREE_H_

#include <utils/bitarray.h>
#include <BitSequenceRG.h>  // libcds
#include <vector>
#include <stack>

namespace k2tree_impl {
using utils::BitArray;
using cds_static::BitSequenceRG;
using std::vector;
using std::stack;

class K2Tree {
  friend class K2TreeBuilder;
 public:
  /* Check if exist a link from object p to q.
   * Identifiers starts with 0.
   *
   * @param p Identifier of first object.
   * @param q Identifier of second object.
   */
  bool CheckLink(size_t p, size_t q) const;

  class SuccessorsIterator {
   public:
    typedef SuccessorsIterator self_type;
    SuccessorsIterator(K2Tree *tree, size_t p);
    SuccessorsIterator operator++();
    SuccessorsIterator operator++(int);
    inline int operator*() {return successor_;}
    bool operator==(const SuccessorsIterator& rhs);
    bool operator!=(const SuccessorsIterator& rhs);
   private:
    size_t p_;
    K2Tree *tree_;
    size_t successor_;
    struct Frame {
      int j;
      int level;
      size_t offset;
      size_t N;
      size_t p;
      size_t q;
      size_t z;
    };
    stack<Frame> frames_;
  };

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
         int k1, int k2, int kl, int max_level_k1, int height, size_t size);
  // Bit array containing the nodes of internal nodes
  BitSequenceRG T_;
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
  size_t size_;
  // Accumulated rank for each level.
  int *acum_rank_;

  inline int GetK(int level) const {
    if (level <= max_level_k1_)  return k1_;
    else if (level < height_ - 1)  return k2_;
    else  return kl_;
  }
};


}  // namespace k2tree_impl

#endif  // INCLUDE_K2TREE_H_
