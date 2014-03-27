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

#include <k2treebuilder.h>
#include <utils/utils.h>
#include <queue>
#include <cstdio>


namespace k2tree_impl {

using utils::LogCeil;
using utils::Pow;
using std::queue;

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
    nodes_k1_(0),  // we dont consider the root
    nodes_k2_(0),
    nodes_kl_(0),
    root() {
  // we extend the size of the matrix to be the multiplication of the arities
  // in all levels (section 5.1). There are k1_levels levels with
  // arity k1, one with arity kl and we must find the numbers of levels with
  // arity k2, ie, the smallest integer x that satisfies:
  // k1^k1_levels * k2^x * kl >= nodes.
  int powk1 = Pow(k1, k1_levels);
  int x = LogCeil(nodes*1.0/powk1/kl, k2);
  if (x == 0)
    printf("Warning: Ignoring levels with arity k2.\n");

  height_ = k1_levels + x + 1;
  size_ = powk1 * Pow(k2, x) * kl;
}

void K2TreeBuilder::InsertEdge(size_t row, size_t col) {
  if (root == NULL)
    root = CreateNode(0);
  Node *n = root;
  int N = size_, div_level, child;
  for (int level = 0; level < height_ - 1; level++) {
    int k = level <= max_level1_ ? k1_ : k2_;
    div_level = N/k;

    child = row/div_level * k + col/div_level;
    row = row % div_level;
    col = col % div_level;
    N /= k;

    if (n->children_[child] == NULL)
        n->children_[child] = CreateNode(level + 1);

    n = n->children_[child];
  }
  // n is a node on the level height_ - 1. In this level
  // we store the children information in a BitString (the leafs)
  div_level = N/kl_;
  child = row/div_level*kl_ + col/div_level;
  n->data_->SetBit(child);
  edges_++;
}

void K2TreeBuilder::Build() const {
  BitString<int> T1(nodes_k1_), T2(nodes_k2_), Tl(nodes_kl_);
  BitString<int> L(leafs_);
  queue<Node*> q;
  q.push(root);

  int cnt_level, level;

  // Position on the bitmap T1
  int pos = -1;
  for (level = 0; level <= max_level1_; ++level) {
    cnt_level = q.size();
    for (int i = 0; i < cnt_level; ++i, ++pos) {
      Node *n = q.front(); q.pop();
      if (n != NULL) {
        if (pos >= 0)  // if not the root
          T1.SetBit(pos);

        for (int child = 0; child < k1_*k1_; ++child)
          q.push(n->children_[child]);
      }
    }
  }

  pos = 0;
  for (; level < height_ - 1; ++level) {
    cnt_level = q.size();
    for (int i = 0; i < cnt_level; ++i, ++pos) {
      Node *n = q.front(); q.pop();
      if (n != NULL) {
        T2.SetBit(pos);

        for (int child = 0; child < k2_*k2_; ++child)
          q.push(n->children_[child]);
      }
    }
  }
  int leaf_pos = 0;
  pos = 0;
  cnt_level = q.size();
  for (int i = 0; i < cnt_level; ++i, ++pos) {
    Node *n = q.front(); q.pop();
    if (n != NULL) {
      Tl.SetBit(pos);
      for (int child = 0; child < kl_*kl_; ++child, ++leaf_pos)
        if (n->data_->GetBit(child))
          L.SetBit(leaf_pos);
    }
  }
}

K2TreeBuilder::~K2TreeBuilder() {
  DeleteNode(root, 0);
}

K2TreeBuilder::Node * K2TreeBuilder::CreateNode(int level) {
  int *nodes;
  if (level + 1 <= max_level1_)
    nodes = &nodes_k1_;
  else if (level + 1 <  height_ - 1)
    nodes = &nodes_k2_;
  else
    nodes = &nodes_kl_;

  Node *n = new Node();
  if (level < height_ - 1) {
    int k = level <= max_level1_ ? k1_ : k2_;
    n->children_ = new Node*[k*k];
    for (int i = 0; i < k*k; ++i)
      n->children_[i] = NULL;
    *nodes += k*k;
  } else {
    n->data_ = new BitString<char>(kl_*kl_);
    leafs_ += kl_*kl_;
  }
  return n;
}

void K2TreeBuilder::DeleteNode(Node *n, int level) {
  if (n == NULL)
    return;

  if (level < height_ - 1) {
    int k = level <= max_level1_ ? k1_ : k2_;
    for (int i = 0; i < k*k; ++i)
      DeleteNode(n->children_[i], level+1);
    delete n->children_;
  } else {
    delete n->data_;
  }
  delete n;
}

}  // namespace k2tree_impl
