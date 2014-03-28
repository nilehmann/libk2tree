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

#include <utils/bitstring.h>
#include <BitSequenceRG.h>  // libcds
#include <vector>

namespace k2tree_impl {
using utils::BitString;
using cds_static::BitSequenceRG;
using std::vector;

class K2Tree {
  friend class K2TreeBuilder;
 public:
  bool CheckEdge(size_t row, size_t col) const;
  vector<size_t> AdjacencyList(size_t row) const;
  vector<size_t> ReverseList(size_t col) const;

 private:
  /* 
   * Construct a k2tree with and hybrid aproach
   * @param T Bit array with the internal nodes
   * @param L Bit array with the leafs
   * @param k1 Arity of the first levels
   * @param k2 Arity of the second part
   * @param kl Arity of the level height-1
   * @param max_level_k1 Las level with arity k1
   * @param height Height of the k2tree
   * @param size Size of the expanded matrix
   */
  K2Tree(const BitString<unsigned int> &T,const BitString<unsigned int> &L, 
      int k1, int k2, int kl, int max_level_k1, int height, size_t size);
  // Bit array containing the nodes of internal nodes
  BitSequenceRG T_;
  // Bit array for the leafs.
  BitString<unsigned int> L_;
  // Arity of the first part.
  int k1_;
  // Arity of the second part.
  int k2_;
  // Arity of the level height-1.
  int kl_;
  // Las level with arity k1
  int max_level_k1_;
  // height of the tree
  int height_;
  // Size of the matrix
  int size_;
  // Accumulated rank for each level.
  int *acum_rank_;
};

}  // namespace k2tree_impl

#endif  // INCLUDE_K2TREE_H_
