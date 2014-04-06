/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 *
 */

#ifndef INCLUDE_BITS_K2TREEBUILDER_H_
#define INCLUDE_BITS_K2TREEBUILDER_H_

#include <bits/k2tree.h>
#include <bits/utils/bitarray.h>
#include <bits/utils/utils.h>
#include <vector>
#include <memory>
#include <queue>
#include <cstddef>

namespace k2tree_impl {

using utils::BitArray;
using utils::Pow;
using utils::LogCeil;
using std::shared_ptr;

/*
 * Implement the construction of section 3.3.3, building a regular tree (not compressed)
 * inserting one link (1 in the matrix) at a time.
 */
template<class A>
class K2TreeBuilder {
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
  K2TreeBuilder(A cnt, int k1, int k2, int kl, int k1_levels):
      cnt_(cnt),
      size_(0),
      k1_(k1),
      k2_(k2),
      kl_(kl),
      max_level_k1_(k1_levels-1),
      height_(-1),
      leafs_(0),
      edges_(0),
      internal_nodes_(0),  // we dont consider the root
      root() {
    // we extend the size of the matrix to be the multiplication of the arities
    // in all levels (section 5.1). There are k1_levels levels with
    // arity k1, one with arity kl and we must find the numbers of levels with
    // arity k2, ie, the smallest integer x that satisfies:
    // k1^k1_levels * k2^x * kl >= cnt.
    int powk1 = Pow(k1, k1_levels);
    int x = LogCeil(cnt*1.0/powk1/kl, k2);
    if (x == 0)
      fprintf(stderr, "Warning: Ignoring levels with arity k2.\n");

    height_ = k1_levels + x + 1;
    size_ = powk1 * Pow(k2, x) * kl;
  }
  /* 
   * Create a link from object p to q.
   * Identifiers start with 0.
   *
   * @param p Identifier of the first object.
   * @param q Identifier of the second object.
   */
  void AddLink(A p, A q) {
    if (root == NULL)
      root = CreateNode(0);
    Node *n = root;
    A N = size_, div_level;
    int child;
    for (int level = 0; level < height_ - 1; level++) {
      int k = level <= max_level_k1_ ? k1_ : k2_;
      div_level = N/k;

      child = p/div_level * k + q/div_level;

      if (n->children_[child] == NULL)
          n->children_[child] = CreateNode(level + 1);

      n = n->children_[child];
      N = div_level, p %= div_level, q %= div_level;
    }
    // n is a node on the level height_ - 1. In this level
    // we store the children information in a BitArray (the leafs)
    div_level = N/kl_;
    child = p/div_level*kl_ + q/div_level;
    if (!n->data_->GetBit(child))
      edges_++;
    n->data_->SetBit(child);
  }

  shared_ptr<K2Tree<A>> Build() const {
    BitArray<unsigned int> T(internal_nodes_);
    BitArray<unsigned int> L(leafs_);
    queue<Node*> q;
    q.push(root);

    size_t cnt_level;
    int level;

    // Position on the bitmap T
    size_t pos = -1;
    for (level = 0; level < height_-1; ++level) {
      int k = level <= max_level_k1_ ? k1_ : k2_;
      cnt_level = q.size();
      for (int i = 0; i < cnt_level; ++i, ++pos) {
        Node *n = q.front(); q.pop();
        if (n != NULL) {
          if (level > 0)  // if not the root
            T.SetBit(pos);

          for (int child = 0; child < k*k; ++child)
            q.push(n->children_[child]);
        }
      }
    }

    // Visiting nodes on level height - 1
    size_t leaf_pos = 0;
    cnt_level = q.size();
    for (int i = 0; i < cnt_level; ++i, ++pos) {
      Node *n = q.front(); q.pop();
      if (n != NULL) {
        T.SetBit(pos);
        for (int child = 0; child < kl_*kl_; ++child, ++leaf_pos)
          if (n->data_->GetBit(child))
            L.SetBit(leaf_pos);
      }
    }

    K2Tree<A> *tree = new K2Tree<A>(T, L, k1_, k2_, kl_, max_level_k1_,
                                    height_, size_);
    return shared_ptr<K2Tree<A> >(tree);
  }

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

  ~K2TreeBuilder() {
    DeleteNode(root, 0);
  }

 private:
  // Number of objects in the original relation.
  A cnt_;
  // Number of rows (and cols) in the expanded matrix
  A size_;
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
      BitArray<unsigned char> *data_;
      // Pointers to children of internal nodes.
      Node **children_;
    };
  };
  // Create a node for the specified level using an appropiate k
  Node *CreateNode(int level) {
    Node *n = new Node();
    if (level < height_ - 1) {
      int k = level <= max_level_k1_ ? k1_ : k2_;
      n->children_ = new Node*[k*k];
      for (int i = 0; i < k*k; ++i)
        n->children_[i] = NULL;
      internal_nodes_ += k*k;
    } else {
      n->data_ = new BitArray<unsigned char>(kl_*kl_);
      leafs_ += kl_*kl_;
    }
    return n;
  }
  // Free memory allocated for n assuming it was built for the
  // specified level. Recursively delete children of internal nodes.
  void DeleteNode(Node *n, int level) {
    if (n == NULL)
      return;

    if (level < height_ - 1) {
      int k = level <= max_level_k1_ ? k1_ : k2_;
      for (int i = 0; i < k*k; ++i)
        DeleteNode(n->children_[i], level+1);
      --internal_nodes_;
      delete [] n->children_;
    } else {
      leafs_ -= kl_*kl_;
      delete n->data_;
    }
    delete n;
  }

  // Root of the tree
  Node *root;
};

}  // namespace k2tree_impl

#endif  // INCLUDE_BITS_K2TREEBUILDER_H_
