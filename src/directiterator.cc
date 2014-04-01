/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2tree.h>

namespace k2tree_impl {
using cds_static::BitSequenceRG;
typedef K2Tree::DirectIterator self_type;

K2Tree::DirectIterator::DirectIterator(const K2Tree *tree,
                                               size_t p,
                                               bool end) :
    tree_(tree), p_(p), curr_(0), frames_(), end_(end) {
  frames_.push(Frame{-1, 0, 0, tree_->size_, p, 0, 0});
  ++(*this);
}


self_type K2Tree::DirectIterator::operator++(int junk) {
  self_type i = *this;
  ++(*this);
  return i;
}

bool K2Tree::DirectIterator::operator==(const self_type& rhs) {
  if (end_ || rhs.end_)
    return p_ == rhs.p_ && end_ == rhs.end_;
  else
    return p_ == rhs.p_ && curr_ == rhs.curr_;
}

bool K2Tree::DirectIterator::operator!=(const self_type& rhs) {
  return !(*this == rhs);
}

self_type K2Tree::DirectIterator::operator++() {
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
    size_t &p = frame.p;
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
          z = z > 0 ? (T->rank1(z-1) - acum_rank[level-1])*k*k : 0;
          z += p/div_level*k + offset;

          j = 0;
          frames_.push({-1, level + 1, nxt_offset,
              N/k, p % div_level, q + div_level*j, z + j});
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
          frames_.push({-1, level + 1, nxt_offset,
              N/k, p % div_level, q + div_level*j, z + j});
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
  return *this;
}


}  // namespace k2tree_impl
