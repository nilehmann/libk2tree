/*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
* ----------------------------------------------------------------------------
*/

#include <bits/k2tree_iterators.h>
#include <bits/basic_k2tree.h>
#include <cstdio>

namespace libk2tree {
namespace iterators {

template<class _Impl, class _Size>
K2TreeIterator<_Impl, _Size>::K2TreeIterator(const basic_k2tree<_Size> *t,
                                             _Size object) :
    tree_(t), object_(object), curr_(0), end_(false), queue_() {
  traverse();
  ++(*this);
}
template<class _Impl, class _Size>
K2TreeIterator<_Impl, _Size>::K2TreeIterator() :
    tree_(NULL), object_(), curr_(), end_(true), queue_() {}

template<class _Impl, class _Size>
void K2TreeIterator<_Impl, _Size>::traverse() {
  _Size div_level;
  _Size cnt_level;
  int k;
  const BitSequence *T = tree_->T_;

  queue_.push(_Impl::FirstFrame(object_));
  _Size N = tree_->size_;
  for (int level = 0; level < tree_->height_; ++level) {
    k = tree_->GetK(level);
    div_level = N/k;

    cnt_level = queue_.size();
    for (_Size i = 0; i < cnt_level; ++i) {
      Frame<_Size> &f = queue_.front();
      if (level == 0 || T->access(f.z)) {
        f.z = _Impl::Rank(f, k, div_level, tree_);
        for (f.j  = 0; f.j < k; ++f.j)
          queue_.push(_Impl::NextFrame(f, k , div_level));
      }
      queue_.pop();
    }
    N = div_level;
  }
}

template<class _Impl, class _Size>
const K2TreeIterator<_Impl, _Size>
K2TreeIterator<_Impl, _Size>::end = {};
/*
template<class _Impl, class _Size>
void K2TreeIterator<_Impl, _Size>::operator++() {
  bool found = false;
  size_t *acum_rank = tree_->acum_rank_;
  _Size div_level;
  const BitSequence *T = tree_->T_;
  const BitArray<unsigned int> *L = &tree_->L_;
  while (!found && !end_) {
    Frame<_Size> &f= frames_.top();

    int k = tree_->GetK(f.level);
    div_level = f.N/k;
    if (f.level < tree_->height_) {

      // Entering first time in frame
      if (f.j == -1) {
        if (f.level == 0 || T->access(f.z)) {
          f.z = _Impl::Rank(f, k, div_level, tree_);
        } else {
          frames_.pop();
          continue;
        }
      }

      f.j++;
      if (f.j < k) {
        frames_.push(_Impl::NextFrame(f, k, div_level));
      } else {
        // No more children on the root
        if (f.level == 0)
          end_ = true;
        frames_.pop();
      }
    } else {
      // We enter and exit the leaf
      if (L->GetBit(f.z - T->getLength())) {
        curr_ = _Impl::Output(f);
        found = true;
      }
      frames_.pop();
    }
  }
}*/



template<class _Size>
RangeIterator<_Size>::RangeIterator(const basic_k2tree<_Size> *tree,
                                     _Size p1, _Size p2) :
    tree_(tree), p1_(p1), p2_(p2), curr_(), end_(false), queue_() {
  traverse();
  ++(*this);
}

template<class _Size>
RangeIterator<_Size>::RangeIterator() :
  tree_(NULL), p1_(0), p2_(0), curr_(), end_(true), queue_() {}

template<class _Size>
void RangeIterator<_Size>::traverse() {
  _Size div_level;
  _Size cnt_level;
  int k;
  const BitSequence  *T = tree_->T_;

  queue_.push(FirstFrame(p1_, p2_));
  _Size N = tree_->size_;
  for (int level = 0; level < tree_->height_; ++level) {
    k = tree_->GetK(level);
    div_level = N/k;

    cnt_level = queue_.size();
    for (_Size q = 0; q < cnt_level; ++q) {
      RangeFrame<_Size> &f = queue_.front();
      if (level == 0 || T->access(f.z)) {
        f.z = Rank(f, level, k);

        _Size start = f.p1/div_level;
        _Size end = f.p2/div_level;
        for (_Size i = start; i <= end; ++i) {
          _Size z = f.z + k*i;
          _Size dp = f.dp + div_level*i;
          _Size pp1 = i == start ? f.p1 % div_level : 0;
          _Size pp2 = i == end ? f.p2 % div_level : div_level - 1;

          for (int j  = 0; j < k; ++j) {
            queue_.push({
                pp1, pp2,
                dp, f.dq + div_level*j, z + j});
          }
        }
      }
      queue_.pop();
    }
    N = div_level;
  }
}
template<class _Size>
const RangeIterator<_Size>
RangeIterator<_Size>::end = {};
/*
template<class _Size>
void RangeIterator<_Size>::operator++() {
  _Size pp1, pp2, qq1, qq2;
  bool found = false;
  const BitSequence *T = tree_->T_;
  const BitArray<unsigned int, _Size> *L = &tree_->L_;

  while (!found && !end_) {
    RangeFrame<_Size> &f= frames_.top();

    int k = tree_->GetK(f.level);
    _Size div_level = f.N/k;
    if (f.level < tree_->height_) {

      // Entering first time in frame
      if (f.first) {
        f.first = false;
        if (f.level == 0 || T->access(f.z)) {
          f.z = Rank(f,k);
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
        frames_.push(NextFrame(f, k, div_level));
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
}*/

template class K2TreeIterator<DirectImpl<unsigned int>, unsigned int>;
template class K2TreeIterator<InverseImpl<unsigned int>, unsigned int>;
template class RangeIterator<unsigned int>;

template class K2TreeIterator<DirectImpl<size_t>, size_t>;
template class K2TreeIterator<InverseImpl<size_t>, size_t>;
template class RangeIterator<size_t>;

}  // namespace iterators
}  // namespace libk2tree
