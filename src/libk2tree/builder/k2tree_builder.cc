/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 *
 */

#include <builder/k2tree_builder.h>
#include <utils/utils.h>
#include <utils/bitarray.h>

namespace libk2tree {
using utils::Pow;
using utils::LogCeil;
using utils::BitArray;


K2TreeBuilder::K2TreeBuilder(cnt_size cnt,
                             uint k1, uint k2, uint kL,
                             uint k1_levels) noexcept
    : cnt_(cnt),
      size_(0),
      k1_(k1),
      k2_(k2),
      kL_(kL),
      max_level_k1_(k1_levels-1),
      height_(0),
      leaves_(0),
      links_(0),
      internal_nodes_(0),  // we do not consider the root
      root_(NULL) {
  assert(k1 != 0 && k2 != 0 && kL_ != 0 && k1_levels != 0);
  // we extend the size of the matrix to be the product of the arities in all
  // levels (section 5.1). There are k1_levels levels with arity k1, one with
  // arity kL and we must find the numbers of levels with arity k2, ie, the
  // smallest integer x that satisfies:
  // k1^k1_levels * k2^x * kL >= cnt.
  uint powk1 = Pow<uint>(k1, k1_levels);
  uint x = LogCeil((double)cnt/powk1/kL, k2);
  if (x == 0)
    fprintf(stderr, "[K2TreeBuilder] Warning: Ignoring levels with arity k2.\n");

  height_ = k1_levels + x + 1;
  size_ = powk1 * Pow<uint>(k2, x) * kL;
}
K2TreeBuilder::K2TreeBuilder(K2TreeBuilder::K2TreeBuilder &&lhs) noexcept
    : cnt_(lhs.cnt_),
      size_(lhs.size_),
      k1_(lhs.k1_),
      k2_(lhs.k2_),
      kL_(lhs.kL_),
      max_level_k1_(lhs.max_level_k1_),
      height_(lhs.height_),
      leaves_(lhs.leaves_),
      links_(lhs.links_),
      internal_nodes_(lhs.internal_nodes_),
      root_(lhs.root_) {
  lhs.root_ = NULL;
  internal_nodes_ = 0;
  links_ = 0;
  leaves_ = 0;
}


void K2TreeBuilder::AddLink(cnt_size p, cnt_size q) {
  if (root_ == NULL)
    root_ = CreateNode(0);
  Node *n = root_;
  cnt_size N = size_, div_level;
  uint child;
  for (uint level = 0; level < height_ - 1; level++) {
    uint k = level <= max_level_k1_ ? k1_ : k2_;
    div_level = N/k;

    child = (uint) (p/div_level * k + q/div_level);

    if (n->children_[child] == NULL)
        n->children_[child] = CreateNode(level + 1);

    n = n->children_[child];
    N = div_level, p %= div_level, q %= div_level;
  }
  // n is a node on the level height_ - 1. In this level
  // we store the children information in a BitArray (the leaves)
  div_level = N/kL_;
  child = (uint) (p/div_level*kL_ + q/div_level);
  if (!n->data_->GetBit(child))
    links_++;
  n->data_->SetBit(child);
}


std::shared_ptr<HybridK2Tree> K2TreeBuilder::Build() const {
  try {
    if (root_ == NULL)
      return std::shared_ptr<HybridK2Tree>(new HybridK2Tree(cnt_, size_));

    BitArray<uint> T(internal_nodes_);
    BitArray<uint> L(leaves_);
    
    std::queue<Node*> q;
    q.push(root_);

    uint cnt_level;
    uint level;

    // Position on the bitmap T
    size_t pos = 0;
    for (level = 0; level < height_-1; ++level) {
      uint k = level <= max_level_k1_ ? k1_ : k2_;
      cnt_level = (uint) q.size();
      for (uint i = 0; i < cnt_level; ++i) {
        Node *n = q.front(); q.pop();
        if (n != NULL) {
          if (level > 0)  // if not the root
            T.SetBit(pos);

          for (uint child = 0; child < k*k; ++child)
            q.push(n->children_[child]);
        }
        if (level > 0)
          ++pos;
      }
    }

    // Visiting nodes in levels height - 1 and height
    size_t leaf_pos = 0;
    cnt_level = (uint) q.size();
    for (uint i = 0; i < cnt_level; ++i) {
      Node *n = q.front(); q.pop();
      if (n != NULL) {
        T.SetBit(pos);
        for (uint child = 0; child < kL_*kL_; ++child) {
          if (n->data_->GetBit(child))
            L.SetBit(leaf_pos);
          ++leaf_pos;
        }
      }
      ++pos;
    }

    HybridK2Tree *tree = new HybridK2Tree(T, L, k1_, k2_, kL_,
                                          max_level_k1_,
                                          height_, cnt_, size_, links_);
    return std::shared_ptr<HybridK2Tree>(tree);
  } catch(std::bad_alloc ba) {
    std::cerr << "[K2TreeBuilder::Build] Error: " << ba.what();
    exit(1);
  } catch(...) {
    std::cerr << "[K2TreeBuilder::Build] Error: unexpected exception\n";
    exit(1);
  }
}



void K2TreeBuilder::Clear() {
  DeleteNode(root_, 0);
  leaves_ = internal_nodes_ = links_ = 0;
  root_ = NULL;
}


K2TreeBuilder::~K2TreeBuilder() {
  DeleteNode(root_, 0);
}


K2TreeBuilder::Node *K2TreeBuilder::CreateNode(uint level) {
  try {
    K2TreeBuilder::Node *n = new K2TreeBuilder::Node;
    if (level < height_ - 1) {
      uint k = level <= max_level_k1_ ? k1_ : k2_;
      n->children_ = new Node*[k*k];
      for (uint i = 0; i < k*k; ++i)
        n->children_[i] = NULL;
      internal_nodes_ += k*k;
    } else {
      n->data_ = new BitArray<uchar>(kL_*kL_);
      leaves_ += kL_*kL_;
    }
    return n;
  } catch (std::bad_alloc ba) {
    std::cerr << "[K2TreeBuilder::CreateNode] Error: " << ba.what() << "\n";
    exit(1);
  } catch(...) {
    std::cerr << "[K2TreeBuilder::CreateNode] Error: unexpected exception\n";
    exit(1);
  }
}


void K2TreeBuilder::DeleteNode(K2TreeBuilder::Node *n, uint level) {
  if (n == NULL)
    return;

  if (level < height_ - 1) {
    uint k = level <= max_level_k1_ ? k1_ : k2_;
    for (uint i = 0; i < k*k; ++i)
      DeleteNode(n->children_[i], level+1);
    delete [] n->children_;
  } else {
    delete n->data_;
  }
  delete n;
}

}  // namespace libk2tree
