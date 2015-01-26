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

/**
 * Implements the construction of section 3.3.3, building a regular tree
 * (with pointers) inserting one link (1 in the matrix) at a time.
 */
class K2TreeBuilder {
 public:
  /**
   * Creates a builder for a tree with a hybrid approach.
   *
   * @param cnt Number of object in the relation.
   * @param k1 arity of the first levels.
   * @param k2 arity of the second part.
   * @param kL arity of the level height-1.
   * @param k1_levels Number of levels with arity k1.
   */
  K2TreeBuilder(cnt_size cnt, uint k1, uint k2, uint kL, uint k1_levels) noexcept;

  /**
   * Move constructor
   */
  K2TreeBuilder(K2TreeBuilder &&lhs) noexcept;

  /**
   * Creates a link from object p to q. Creating a link out of the range of
   * the matrix causes an undefined behavior.
   *
   * @param p Identifier of the first object.
   * @param q Identifier of the second object.
   */
  void AddLink(cnt_size p, cnt_size q);

  /**
   * Builds a k2tree with the current structure.
   */
  std::shared_ptr<HybridK2Tree> Build() const;

  /**
   * Builds a k2tree with the current structure and saves it to a file.
   * @param out Buffer to save the tree.
   */
  void Build(std::ofstream *out) const {
    std::shared_ptr<HybridK2Tree> tree = Build();
    tree->Save(out);
  }

  /**
   * Clears the builder deleting the current structure
   */
  void Clear();

  /**
   * Returns the resulting height of the tree.
   * @return Height of the tree.
   */
  inline uint height() const {
    return height_;
  }

  /**
   * Returns number nodes in the last level.
   *
   * @return Number of leaves.
   */
  inline size_t leaves() const {
    return leaves_;
  }

  /**
   * Returns the number of distinct links inserted in the tree.
   *
   * @return Number of link.
   */
  inline size_t links() const {
    return links_;
  }

  /**
   * Returns the number of non-leaf nodes.
   *
   * @return Number of internal nodes.
   */
  inline size_t internal_nodes() const {
    return internal_nodes_;
  }

  ~K2TreeBuilder();

 private:
  /** Number of objects in the original relation. */
  cnt_size cnt_;
  /** Number of rows (and cols) in the expanded matrix */
  cnt_size size_;
  /** Arity of the first part */
  uint k1_;
  /** Arity of the second part */
  uint k2_;
  /** Arity of the level height-1 */
  uint kL_;
  /** Last level with arity k1. Each node in this level has 0 or k1 children. */
  uint max_level_k1_;
  /** Height of the tree, also the number of the leaf level. */
  uint height_;
  /** Number of nodes in the last level. */
  size_t leaves_;
  /** Number of links in the relation (ones in the matrix). */
  size_t links_;
  /** 
   * Number of internal nodes, ie, nodes not in the last level. 
   * It also counts leaves not in the last level
   */
  size_t internal_nodes_;

  /** Struct to store the tree. */
  struct Node {
    union {
      /** Stores the children of a node in level height_-1 (the leaves) */
      BitArray<uchar> *data_;
      /** Array of pointers to children of internal nodes. */
      Node **children_;
    };
  };
  /**
   * Creates a node for the specified level using an appropriate k
   *
   * @param level Level
   * @return New node storing union data corresponding to the level.
   */
  Node *CreateNode(uint level);
  /**
   * Frees memory allocated for the given node assuming it was built for the
   * specified level. Recursively delete children first.
   *
   * @param n Node.
   * @param level Level containing the node.
   */
  void DeleteNode(Node *n, uint level);

  /**
   * Root of the tree. This is never NULL.
   */
  mutable Node *root_;
};


}  // namespace libk2tree

#endif  // INCLUDE_BUILDER_K2TREE_BUILDER_H_
