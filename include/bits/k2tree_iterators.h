/*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
* ----------------------------------------------------------------------------
*/

#ifndef INCLUDE_BITS_K2TREE_ITERATORS_H_
#define INCLUDE_BITS_K2TREE_ITERATORS_H_

#include <bits/k2tree.h>
#include <stack>
#include <iostream>
using namespace std;

namespace k2tree_impl {
using ::std::pair;

template<class self_type, class A>
class K2TreeIterator_ {
 public:
  K2TreeIterator_(const K2Tree<A> *t, A object, bool end) :
      tree_(t), object_(object), curr_(0), frames_(), end_(end) {}

  bool operator==(const self_type& rhs) {
    if (end_ || rhs.end_)
      return object_ == rhs.object_ && end_ == rhs.end_;
    else
      return object_ == rhs.object_ && curr_ == rhs.curr_;
  }

  bool operator!=(const self_type& rhs) {
    return !(*static_cast<self_type*>(this) == rhs);
  }

  int operator*() {
    return curr_;
  }

  self_type operator++(int) {
    self_type i = *static_cast<self_type*>(this);
    ++(*static_cast<self_type*>(this));
    return i;
  }
  self_type operator++() {
    bool found = false;
    size_t *acum_rank = tree_->acum_rank_;
    const BitSequence *T = tree_->T_;
    const BitArray<unsigned int> *L = &tree_->L_;

    while (!found && !end_) {
      Frame &f= frames_.top();

      int k = tree_->GetK(f.level);
      A div_level = f.N/k;
      if (f.level < tree_->height_) {
        size_t nxt_offset;
        if (f.level > 0)
          nxt_offset = f.offset+(acum_rank[f.level] - acum_rank[f.level-1])*k*k;
        else
          nxt_offset = k*k;

        // Entering first time in frame
        if (f.j == -1) {
          if (f.level == 0 || T->access(f.z)) {
            f.z = Rank(f, k, div_level);
          } else {
            frames_.pop();
            continue;
          }
        }

        f.j++;
        // No more children on the root
        if (f.j < k) {
          PushNextFrame(f, nxt_offset, k, div_level);
        } else {
          if (f.level == 0)
            end_ = true;
          frames_.pop();
        }
      } else {
        // We enter and exit the leaf
        if (L->GetBit(f.z - T->getLength())) {
          curr_ = Output(f);
          found = true;
        }
        frames_.pop();
      }
    }
    return *static_cast<self_type*>(this);
  }

 protected:
  struct Frame {
    int j;
    int level;
    size_t offset;
    A N;
    A p;
    A q;
    size_t z;
  };

  const K2Tree<A> *tree_;
  A object_;
  A curr_;
  stack<Frame> frames_;
  bool end_;

  virtual void PushNextFrame(const Frame &frame, size_t nxt_offset, int k,
      A div_level) = 0;
  virtual size_t Rank(const Frame &frame, int k, A div_level) = 0;
  virtual A Output(const Frame &frame) = 0;
};

template<class A>
class DirectIterator_ :
    public K2TreeIterator_<DirectIterator_<A>, A> {
 public:
  DirectIterator_(const K2Tree<A> *tree, A p, bool end) :
      K2TreeIterator_<DirectIterator_<A>, A>(tree, p, end) {
    this->frames_.push(Frame{-1, 0, 0, tree->size_, p, 0, 0});
    ++(*this);
  }

 private:
  typedef typename K2TreeIterator_<DirectIterator_, A>::Frame Frame;
  virtual void PushNextFrame(const Frame &f, size_t nxt_offset, int ,
      A div_level) {
    this->frames_.push({-1, f.level + 1, nxt_offset,
        div_level, f.p % div_level, f.q + div_level*f.j, f.z + f.j});
  }
  virtual size_t Rank(const Frame &f, int k, A div_level) {
    size_t z;
    const K2Tree<A> *tree = this->tree_;
    z = f.z > 0 ? (tree->T_->rank1(f.z-1)-tree->acum_rank_[f.level-1])*k*k : 0;
    z += f.p/div_level*k + f.offset;
    return z;
  }

  virtual A Output(const Frame &frame) {
    return frame.q;
  }
};


template<class A>
class InverseIterator_ :
    public K2TreeIterator_<InverseIterator_<A>, A> {
 public:
  InverseIterator_(const K2Tree<A> *tree, A q, bool end) :
      K2TreeIterator_<InverseIterator_<A>, A>(tree, q, end) {
    this->frames_.push(Frame{-1, 0, 0, tree->size_, 0, q, 0});
    ++(*this);
  }

 private:
  typedef typename K2TreeIterator_<InverseIterator_, A>::Frame Frame;
  virtual void PushNextFrame(const Frame &f, size_t nxt_offset, int k,
      A div_level) {
    this->frames_.push({-1, f.level + 1, nxt_offset,
        div_level, f.p + div_level*f.j, f.q % div_level, f.z + f.j*k});
  }
  virtual size_t Rank(const Frame &f, int k, A div_level) {
    size_t z;
    const K2Tree<A> *tree = this->tree_;
    z = f.z > 0 ? (tree->T_->rank1(f.z-1)-tree->acum_rank_[f.level-1])*k*k : 0;
    z += f.q/div_level + f.offset;
    return z;
  }

  virtual A Output(const Frame &frame) {
    return frame.p;
  }
};



template<class A>
class RangeIterator_ {
 public:
  RangeIterator_(const K2Tree<A> *tree, A p1, A p2, A q1, A q2, bool end) :
      tree_(tree), p1_(p1), p2_(p2), q1_(q1), q2_(q2),
      frames_(), curr_(), end_(end) {
    frames_.push(Frame{-1, -1, 0, 0,
        tree->size_, p1, p2, q1, q2, 0, 0, 0});
    ++(*this);
  }
  bool operator==(const RangeIterator_<A> &rhs) {
    if (end_ || rhs.end_)
      return end_ == rhs.end_;
    else
      return p1_ == rhs.p1_ && p2_ == rhs.p2_ &&
             q1_ && rhs.q1_ && q2_ == rhs.q2_ &&
             curr_ == rhs.curr_;
  }
  bool operator!=(const RangeIterator_<A> &rhs) {
    return !((*this) == rhs);
  }
  pair<A, A> operator*() {
    return curr_;
  }
  RangeIterator_<A> operator++(int) {
    RangeIterator_<A> i = *this;
    ++(*this);
    return i;
  }
  RangeIterator_<A> operator++() {
    A pp1, pp2, qq1, qq2;
    bool found = false;
    size_t *acum_rank = tree_->acum_rank_;
    const BitSequence *T = tree_->T_;
    const BitArray<unsigned int> *L = &tree_->L_;

    while (!found && !end_) {
      Frame &f= frames_.top();
      size_t &z = f.z;

      int k = tree_->GetK(f.level);
      A div_level = f.N/k;
      if (f.level < tree_->height_) {
        size_t nxt_offset;
        if (f.level > 0)
          nxt_offset = f.offset+(acum_rank[f.level] - acum_rank[f.level-1])*k*k;
        else
          nxt_offset = k*k;

        // Entering first time in frame
        if (f.i == -1) {
          if (f.level == 0 || T->access(f.z)) {
            z = z > 0 ? (T->rank1(z-1) - acum_rank[f.level-1])*k*k : 0;
            z += f.offset;
            f.i = f.p1/div_level - 1;
            f.j = f.q2/div_level;  // so f.j++ goes out of range
          } else {
            frames_.pop();
            continue;
          }
        }
        // Entering after a return
        f.j++;
        if (f.j > f.q2/div_level) {
          f.i++;
          f.j = f.q1/div_level;
        }
        if (f.i <= f.p2/div_level) {
          pp1 = f.i == f.p1/div_level ? f.p1 % div_level : 0;
          pp2 = f.i == f.p2/div_level ? f.p2 % div_level : div_level - 1;
          qq1 = f.j == f.q1/div_level ? f.q1 % div_level : 0;
          qq2 = f.j == f.q2/div_level ? f.q2 % div_level : div_level - 1;
          frames_.push(Frame{-1, -1, f.level + 1, nxt_offset,
              div_level, pp1, pp2, qq1, qq2,
              f.dp + div_level*f.i, f.dq + div_level*f.j, z + k*f.i+f.j});
        } else {
          if (f.level == 0)
            end_ = true;
          frames_.pop();
        }
      } else {
        // We enter and exit the leaf
        if (L->GetBit(f.z - T->getLength())) {
          curr_ = make_pair(f.dp, f.dq);
          found = true;
        }
        frames_.pop();
      }
    }
    return *(this);
  }

 private:
  const K2Tree<A> *tree_;
  struct Frame {
    int i, j;
    int level;
    size_t offset;
    A N;
    A p1, p2, q1, q2;
    A dp, dq;
    size_t z;
  };
  A p1_, p2_, q1_, q2_;
  stack<Frame> frames_;
  pair<A, A> curr_;
  bool end_;

};
}  // namespace k2tree_impl
#endif  // INCLUDE_BITS_K2TREE_ITERATORS_H_
