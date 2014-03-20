/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 *
 * Refer to k2treebuilder.h for interface information.
 */

#include <cstdio>
#include "./k2treebuilder.h"
#include "utils/utils.h"

namespace k2tree_impl {

using utils::LogCeil;
using utils::Pow;

K2TreeBuilder::K2TreeBuilder(size_t nodes, int k1, int k2, int kl,
            int k1_levels) :
    nodes_(nodes),
    size_(0),
    k1_(k1),
    k2_(k2),
    kl_(kl),
    max_level1_(k1_levels-1),
    height_(-1),
    leafs_(0),
    edges_(0),
    internal_nodes_(),  // we dont consider the root
    root() {
  // we extend the size of the matrix to be the multiplication of the arities
  // in all levels (section 5.1). There are k1_levels levels with
  // arity k1, one with arity kl and we must find the numbers of levels with
  // arity k2, ie, the smallest x that satisfies:
  // k1^k1_levels * k2^x * kl >= nodes.
  int powk1 = Pow(k1, k1_levels);
  int x = LogCeil(nodes*1.0/powk1/kl, k2);
  if (x == 0)
    printf("Warning: Ignoring levels with arity k2.\n");

  height_ = k1_levels + x + 1;
  size_ = powk1 * Pow(k2, x) * kl;
}

void K2TreeBuilder::InsertEdge(size_t row, size_t col) {
  if (root == NULL) {
    root = new Node(k1_*k1_);
    internal_nodes_ += k1_*k1_;
  }
  Node *n = root;
  int N = size_, div_level, child;
  int level;
  for (level = 0; level < height_ - 1; level++) {
    int k = level <= max_level1_ ? k1_ : k2_;
    div_level = N/k;

    child = row/div_level * k + col/div_level;
    row = row % div_level;
    col = col % div_level;
    N /= k;

    if (n->childs_[child] == NULL) {
      k = level + 1 <= max_level1_ ? k1_ : k2_;
      n->childs_[child] = new Node(k*k);
      if (level < height_ - 2)
        internal_nodes_ += k*k;
    }
    n = n->childs_[child];
  }
  // n is a node on the level height - 1. In this level
  // we store the children information in a BitString (the leafs)
  if (n->data_ == NULL) {
    ++leafs_;
    n->data_ = new BitString<char>(kl_*kl_);
  }
  div_level = N/kl_;
  child = row/div_level*kl_ + col/div_level;
  n->data_->SetBit(child);
  edges_++;
}

K2TreeBuilder::~K2TreeBuilder() {
  if (root != NULL)
    delete root;
}

K2TreeBuilder::Node::Node(int N) :
    data_(),
    childs_(new Node*[N]),
    k1_(N) {
  for (int i = 0; i < N; ++i)
    childs_[i] = NULL;
}


K2TreeBuilder::Node::~Node() {
  for (int i = 0; i < k1_; ++i)
    if (childs_[i] != NULL)
      delete childs_[i];
  delete [] childs_;
  if (data_ != NULL)
    delete data_;
}

}  // namespace k2tree_impl
