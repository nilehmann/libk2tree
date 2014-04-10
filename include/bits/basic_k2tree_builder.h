/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 *
 */

#ifndef INCLUDE_BITS_BASIC_K2TREE_BUILDER_H_
#define INCLUDE_BITS_BASIC_K2TREE_BUILDER_H_

#include <bits/basic_k2tree.h>
#include <bits/utils/bitarray.h>
#include <bits/utils/utils.h>
#include <vector>
#include <memory>
#include <queue>
#include <cstddef>

namespace libk2tree {

using utils::BitArray;
using utils::Pow;
using utils::LogCeil;
using std::shared_ptr;

/*
 * Implement the construction of section 3.3.3, building a regular tree (not compressed)
 * inserting one link (1 in the matrix) at a time.
 */
template<class _Obj>
class basic_k2tree_builder {
 public:
  /* 
   * Create a builder for a tree with an hybrid aproach.
   *
   * @param cnt Number of object in the relation.
   * @param k1 arity of the first levels.
   * @param k2 arity of the second part.
   * @param kl arity of the level height-1.
   * @param k1_levels Number of levels with arity k1.
   */
  basic_k2tree_builder(_Obj cnt, int k1, int k2, int kl, int k1_levels);
  /* 
   * Create a link from object p to q.
   * Identifiers start with 0.
   *
   * @param p Identifier of the first object.
   * @param q Identifier of the second object.
   */
  void AddLink(_Obj p, _Obj q);

  shared_ptr<basic_k2tree<_Obj>> Build() const;

  inline int height() const {
    return height_;
  }

  inline size_t leafs() const {
    return leafs_;
  }

  inline size_t edges() const {
    return edges_;
  }

  inline size_t internal_nodes() const {
    return internal_nodes_;
  }

  ~basic_k2tree_builder();

 private:
  // Number of objects in the original relation.
  _Obj cnt_;
  // Number of rows (and cols) in the expanded matrix
  _Obj size_;
  // Arity of the first part
  int k1_;
  // Arity of the second part
  int k2_;
  // Arity of the level height-1
  int kl_;
  // Last level with arity k1. Each node in this level has 0 or k1 children.
  int max_level_k1_;
  // Height of the tree, also the number of the leaf level.
  int height_;
  // Number of nodes on the last level.
  size_t leafs_;
  // Number of 1s on the matrix.
  size_t edges_;
  // Number of internal nodes.
  size_t internal_nodes_;

  // Struct to store the tree.
  struct Node {
    union {
      // Store the children of the level height_-1 (the leafs)
      BitArray<unsigned char, unsigned int> *data_;
      // Pointers to children of internal nodes.
      Node **children_;
    };
  };
  // Create a node for the specified level using an appropiate k
  inline Node *CreateNode(int level);
  // Free memory allocated for n assuming it was built for the
  // specified level. Recursively delete children of internal nodes.
  inline void DeleteNode(Node *n, int level);

  // Root of the tree
  Node *root;
};

}  // namespace libk2tree

#endif  // INCLUDE_BITS_BASIC_K2TREE_BUILDER_H_
