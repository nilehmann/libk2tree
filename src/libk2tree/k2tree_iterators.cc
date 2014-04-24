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
void K2TreeIterator<_Impl, _Size>::operator++() {
  const BitSequence *T = tree_->T_;
  const BitArray<unsigned int, _Size> &L = tree_->L_;
  while (!queue_.empty()) {
    const Frame<_Size> &f = queue_.front();
    if (L.GetBit(f.z - T->getLength())) {
      curr_ = _Impl::Output(f);
      queue_.pop();
      return;
    }
    queue_.pop();
  }
  end_ = true;
}
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
        f.z = tree_->GetFirstChild(f.z, level, k);
        f.z += _Impl::Offset(f, k, div_level);
        for (int j  = 0; j < k; ++j)
          queue_.push(_Impl::NextFrame(f, j, k, div_level));
      }
      queue_.pop();
    }
    N = div_level;
  }
}

template<class _Impl, class _Size>
const K2TreeIterator<_Impl, _Size>
K2TreeIterator<_Impl, _Size>::end = {};


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
  const BitSequence *T = tree_->T_;

  queue_.push({p1_, p2_, 0, 0, 0});
  _Size N = tree_->size_;
  for (int level = 0; level < tree_->height_; ++level) {
    k = tree_->GetK(level);
    div_level = N/k;

    cnt_level = queue_.size();
    for (_Size q = 0; q < cnt_level; ++q) {
      RangeFrame<_Size> &f = queue_.front();
      if (level == 0 || T->access(f.z)) {
        f.z = tree_->GetFirstChild(f.z, level, k);

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

template class K2TreeIterator<DirectImpl<unsigned int>, unsigned int>;
template class K2TreeIterator<InverseImpl<unsigned int>, unsigned int>;
template class RangeIterator<unsigned int>;

template class K2TreeIterator<DirectImpl<size_t>, size_t>;
template class K2TreeIterator<InverseImpl<size_t>, size_t>;
template class RangeIterator<size_t>;

}  // namespace iterators
}  // namespace libk2tree
