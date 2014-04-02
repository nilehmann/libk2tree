/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_BITS_K2TREE_ITERATORS_H
#define INCLUDE_BITS_K2TREE_ITERATORS_H

#include <bits/k2tree.h>

namespace k2tree_impl {

template<class self_type>
class K2Tree::K2TreeIterator {
 public:
  K2TreeIterator(const K2Tree *t, bool end) :
      tree_(t), curr_(0), frames_(), end_(end) {}

  int operator*() {
    return curr_;
  }

  bool operator!=(const self_type& rhs) {
    return !(*static_cast<self_type*>(this) == rhs);
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
      Frame &frame = frames_.top();
      int &j = frame.j;
      int &level = frame.level;
      size_t &offset = frame.offset;
      size_t &N = frame.N;
      size_t &q = frame.q;
      size_t &z = frame.z;

      int k = tree_->GetK(level);
      size_t div_level = N/k;
      if (level < tree_->height_) {
        size_t nxt_offset;
        if (level > 0)
          nxt_offset = offset + (acum_rank[level] - acum_rank[level-1])*k*k;
        else
          nxt_offset = k*k;

        //  Entering first time in frame
        if (j == -1) {
          if (level == 0 || T->access(z)) {
            RangeFrame(frame, k, div_level);

            j = 0;
            PushNextFrame(frame, nxt_offset, k, div_level);
          } else {
            frames_.pop();
          }
        //  Entering after a return
        } else {
          j += 1;
          //  No more children on the root
          if (level == 0 && j >= k)
            end_ = true;
          else if (j < k)
            PushNextFrame(frame, nxt_offset, k, div_level);
          else
            frames_.pop();
        }
      } else {
        //  Entering for the first time in the leaf
        if (j == -1) {
          j++;
          if (L->GetBit(z - T->getLength())) {
            curr_ = q;
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
  size_t curr_;
  stack<Frame> frames_;
  bool end_;

  virtual void PushNextFrame(const Frame &frame, size_t nxt_offset, int k,
      size_t div_leve) = 0;
  virtual void RangeFrame(Frame &frame, int k, size_t div_level) = 0;

};
class K2Tree::DirectIterator :
    public K2Tree::K2TreeIterator<K2Tree::DirectIterator> {
 public:
  DirectIterator(const K2Tree *tree, size_t p, bool end) :
         K2TreeIterator(tree, end), p_(p) {
    frames_.push(Frame{-1, 0, 0, tree_->size_, p, 0, 0});
    ++(*this);
  }


  bool operator==(const DirectIterator& rhs) {
    if (end_ || rhs.end_)
      return p_ == rhs.p_ && end_ == rhs.end_;
    else
      return p_ == rhs.p_ && curr_ == rhs.curr_;
  }

 private:
  size_t p_;

  virtual void PushNextFrame(const Frame &frame, size_t nxt_offset, int k,
      size_t div_level) {
    const int &j = frame.j;
    const int &level = frame.level;
    const size_t &N = frame.N;
    const size_t &p = frame.p;
    const size_t &q = frame.q;
    const size_t &z = frame.z;
    frames_.push({-1, level + 1, nxt_offset,
        N/k, p % div_level, q + div_level*j, z + j});
  }
  virtual void RangeFrame(Frame &frame, int k, size_t div_level) {
    size_t &z = frame.z;
    size_t &p = frame.p;
    size_t &offset = frame.offset;
    int level = frame.level;
    z = z > 0 ? (tree_->T_.rank1(z-1) - tree_->acum_rank_[level-1])*k*k : 0;
    z += p/div_level*k + offset;
  }
};




}  // namespace k2tree_impl
#endif  // INCLUDE_BITS_K2TREE_H
