/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 *
 */

#ifndef INCLUDE_BITS_BASIC_K2TREEBUILDER_H_
#define INCLUDE_BITS_BASIC_K2TREEBUILDER_H_

#include <bits/basic_k2tree.h>
#include <bits/utils/bitarray.h>
#include <bits/utils/utils.h>
#include <vector>
#include <memory>
#include <queue>
#include <exception>
#include <cstddef>
#include <cstdio>

namespace libk2tree {

using utils::BitArray;
using utils::Pow;
using utils::LogCeil;
using std::shared_ptr;

/*
 * Implement the construction of section 3.3.3, building a regular tree (not compressed)
 * inserting one link (1 in the matrix) at a time.
 */
template<class _Size>
class basic_k2treebuilder {
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
  basic_k2treebuilder(_Size cnt, int k1, int k2, int kl, int k1_levels);
  /* 
   * Create a link from object p to q. Creating a link out of the range of
   * the matrix causes an undefined behavior.
   *
   * @param p Identifier of the first object.
   * @param q Identifier of the second object.
   */
  void AddLink(_Size p, _Size q);

  /*
   * Builds a k2tree with the current structure.
   */
  shared_ptr<basic_k2tree<_Size>> Build() const;

  /*
   * Clear the builder deleting the current structure
   */
  void Clear();

  inline int height() const {
    return height_;
  }

  inline _Size leafs() const {
    return leafs_;
  }

  inline _Size edges() const {
    return edges_;
  }

  inline _Size internal_nodes() const {
    return internal_nodes_;
  }

  ~basic_k2treebuilder();

 private:
  // Number of objects in the original relation.
  _Size cnt_;
  // Number of rows (and cols) in the expanded matrix
  _Size size_;
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
  _Size leafs_;
  // Number of 1s on the matrix.
  _Size edges_;
  // Number of internal nodes.
  _Size internal_nodes_;

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
  Node *CreateNode(int level);
  // Free memory allocated for n assuming it was built for the
  // specified level. Recursively delete children of internal nodes.
  void DeleteNode(Node *n, int level);

  // Root of the tree
  Node *root;
};


}  // namespace libk2tree

#endif  // INCLUDE_BITS_BASIC_K2TREEBUILDER_H_
