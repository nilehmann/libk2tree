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

namespace k2tree_impl {

template<class self_type>
class K2Tree::K2TreeIterator {
 public:
  K2TreeIterator(const K2Tree *t, size_t object, bool end) :
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
    int *acum_rank = tree_->acum_rank_;
    const BitSequenceRG *T = &tree_->T_;
    const BitArray<unsigned int> *L = &tree_->L_;

    while (!found && !end_) {
      Frame &f= frames_.top();

      int k = tree_->GetK(f.level);
      size_t div_level = f.N/k;
      if (f.level < tree_->height_) {
        size_t nxt_offset;
        if (f.level > 0)
          nxt_offset = f.offset+(acum_rank[f.level] - acum_rank[f.level-1])*k*k;
        else
          nxt_offset = k*k;

        //  Entering first time in frame
        if (f.j == -1) {
          if (f.level == 0 || T->access(f.z)) {
            Rank(&f, k, div_level);

            f.j = 0;
            PushNextFrame(f, nxt_offset, k, div_level);
          } else {
            frames_.pop();
          }
        //  Entering after a return
        } else {
          f.j += 1;
          //  No more children on the root
          if (f.level == 0 && f.j >= k)
            end_ = true;
          else if (f.j < k)
            PushNextFrame(f, nxt_offset, k, div_level);
          else
            frames_.pop();
        }
      } else {
        //  Entering for the first time in the leaf
        if (f.j == -1) {
          f.j++;
          if (L->GetBit(f.z - T->getLength())) {
            curr_ = Output(f);
            found = true;
          } else {
            frames_.pop();
          }
        } else {
          frames_.pop();
        }
      }
    }
    return *static_cast<self_type*>(this);
  }

 protected:
  struct Frame {
    int j;
    int level;
    size_t offset;
    size_t N;
    size_t p;
    size_t q;
    size_t z;
  };

  const K2Tree *tree_;
  size_t object_;
  size_t curr_;
  stack<Frame> frames_;
  bool end_;

  virtual void PushNextFrame(const Frame &frame, size_t nxt_offset, int k,
      size_t div_leve) = 0;
  virtual void Rank(Frame *frame, int k, size_t div_level) = 0;
  virtual size_t Output(const Frame &frame) = 0;
};
class K2Tree::DirectIterator :
    public K2Tree::K2TreeIterator<K2Tree::DirectIterator> {
 public:
  DirectIterator(const K2Tree *tree, size_t p, bool end) :
         K2TreeIterator(tree, p, end) {
    frames_.push(Frame{-1, 0, 0, tree_->size_, p, 0, 0});
    ++(*this);
  }

 private:
  virtual void PushNextFrame(const Frame &f, size_t nxt_offset, int ,
      size_t div_level) {
    frames_.push({-1, f.level + 1, nxt_offset,
        div_level, f.p % div_level, f.q + div_level*f.j, f.z + f.j});
  }
  virtual void Rank(Frame *f, int k, size_t div_level) {
    size_t &z = f->z;
    z = z > 0 ? (tree_->T_.rank1(z-1) - tree_->acum_rank_[f->level-1])*k*k : 0;
    z += f->p/div_level*k + f->offset;
  }

  virtual size_t Output(const Frame &frame) {
    return frame.q;
  }
};


class K2Tree::InverseIterator :
    public K2Tree::K2TreeIterator<K2Tree::InverseIterator> {
 public:
  InverseIterator(const K2Tree *tree, size_t q, bool end) :
         K2TreeIterator(tree, q, end) {
    frames_.push(Frame{-1, 0, 0, tree_->size_, 0, q, 0});
    ++(*this);
  }

 private:
  virtual void PushNextFrame(const Frame &f, size_t nxt_offset, int k,
      size_t div_level) {
    frames_.push({-1, f.level + 1, nxt_offset,
        div_level, f.p + div_level*f.j, f.q % div_level, f.z + f.j*k});
  }
  virtual void Rank(Frame *f, int k, size_t div_level) {
    size_t &z = f->z;
    z = z > 0 ? (tree_->T_.rank1(z-1) - tree_->acum_rank_[f->level-1])*k*k : 0;
    z += f->q/div_level + f->offset;
  }

  virtual size_t Output(const Frame &frame) {
    return frame.p;
  }
};


}  // namespace k2tree_impl
#endif  // INCLUDE_BITS_K2TREE_ITERATORS_H_
