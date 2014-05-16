/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 *
 */

#ifndef INCLUDE_BUILDER_K2TREE_BUILDER_H_
#define INCLUDE_BUILDER_K2TREE_BUILDER_H_

#include <libk2tree_basic.h>
#include <hybrid_k2tree.h>
#include <fstream>
#include <memory>

namespace libk2tree {
using std::shared_ptr;

/**
 * Implements the construction of section 3.3.3, building a regular tree (not compressed)
 * inserting one link (1 in the matrix) at a time.
 */
class K2TreeBuilder {
 public:
  /**
   * Creates a builder for a tree with an hybrid aproach.
   *
   * @param cnt Number of object in the relation.
   * @param k1 arity of the first levels.
   * @param k2 arity of the second part.
   * @param kl arity of the level height-1.
   * @param k1_levels Number of levels with arity k1.
   */
  K2TreeBuilder(uint cnt, int k1, int k2, int kl, int k1_levels);

  /**
   * Creates a link from object p to q. Creating a link out of the range of
   * the matrix causes an undefined behavior.
   *
   * @param p Identifier of the first object.
   * @param q Identifier of the second object.
   */
  void AddLink(uint p, uint q);

  /**
   * Builds a k2tree with the current structure.
   */
  shared_ptr<HybridK2Tree> Build() const;

  /**
   * Builds a k2tree with the current structure and saves it to a file
   */
  void Build(std::ofstream *out) const {
    shared_ptr<HybridK2Tree> tree = Build();
    tree->Save(out);
  }

  /**
   * Clears the builder deleting the current structure
   */
  void Clear();

  inline int height() const {
    return height_;
  }

  inline uint leafs() const {
    return leafs_;
  }

  inline uint edges() const {
    return edges_;
  }

  inline uint internal_nodes() const {
    return internal_nodes_;
  }

  ~K2TreeBuilder();

 private:
  /** Number of objects in the original relation. */
  uint cnt_;
  /** Number of rows (and cols) in the expanded matrix */
  uint size_;
  /** Arity of the first part */
  int k1_;
  /** Arity of the second part */
  int k2_;
  /** Arity of the level height-1 */
  int kl_;
  /** Last level with arity k1. Each node in this level has 0 or k1 children. */
  int max_level_k1_;
  /** Height of the tree, also the number of the leaf level. */
  int height_;
  /** Number of nodes on the last level. */
  uint leafs_;
  /** Number of 1s on the matrix. */
  uint edges_;
  /** Number of internal nodes. */
  uint internal_nodes_;

  /** Struct to store the tree. */
  struct Node {
    union {
      /** Stores the children of the level height_-1 (the leafs) */
      BitArray<uchar, uint> *data_;
      /** Pointers to children of internal nodes. */
      Node **children_;
    };
  };
  /**
   * Creates a node for the specified level using an appropiate k
   */
  Node *CreateNode(int level);
  /**
   * Frees memory allocated for n assuming it was built for the
   * specified level. Recursively deletes children of internal nodes.
   */
  void DeleteNode(Node *n, int level);

  /**
   * Root of the tree
   */
  Node *root;
};


}  // namespace libk2tree

#endif  // INCLUDE_BUILDER_K2TREE_BUILDER_H_
