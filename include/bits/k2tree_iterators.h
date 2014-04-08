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

#include <bits/basic_k2tree.h>
#include <stack>
#include <utility>

namespace libk2tree {
using ::std::pair;

template<class _Obj>
struct Frame_ {
  int j;
  int level;
  size_t offset;
  _Obj N;
  _Obj p, q;
  size_t z;
};
template<class _Obj>
struct RangeFrame_ {
  bool first;
  unsigned int i, j;
  int level;
  size_t offset;
  _Obj N;
  _Obj p1, p2, q1, q2;
  _Obj dp, dq;
  size_t z;
};

template<class self_type, class _Obj>
class K2TreeIterator_ {
 public:
  K2TreeIterator_(const basic_k2tree<_Obj> *t, _Obj object, bool end);

  bool operator==(const self_type& rhs) {
    if (end_ || rhs.end_)
      return end_ == rhs.end_;
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
  bool HasNext() {
    return !end_;
  }
  void operator++();

 protected:
  const basic_k2tree<_Obj> *tree_;
  _Obj object_;
  _Obj curr_;
  stack<Frame_<_Obj>> frames_;
  bool end_;

  virtual void PushNextFrame(const Frame_<_Obj> &frame,
                             size_t nxt_offset, int k,
                             _Obj div_level) = 0;
  virtual size_t Rank(const Frame_<_Obj> &frame, int k, _Obj div_level) = 0;
  virtual _Obj Output(const Frame_<_Obj> &frame) = 0;
};

template<class _Obj>
class DirectIterator_ :
    public K2TreeIterator_<DirectIterator_<_Obj>, _Obj> {
 public:
  DirectIterator_(const basic_k2tree<_Obj> *tree, _Obj p, bool end);

 private:
  virtual void PushNextFrame(const Frame_<_Obj> &f, size_t nxt_offset, int ,
      _Obj div_level);
  virtual size_t Rank(const Frame_<_Obj> &f, int k, _Obj div_level);

  virtual _Obj Output(const Frame_<_Obj> &frame);
};


template<class _Obj>
class InverseIterator_ :
    public K2TreeIterator_<InverseIterator_<_Obj>, _Obj> {
 public:
  InverseIterator_(const basic_k2tree<_Obj> *tree, _Obj q, bool end);

 private:
  virtual void PushNextFrame(const Frame_<_Obj> &f, size_t nxt_offset, int k,
      _Obj div_level);
  virtual size_t Rank(const Frame_<_Obj> &f, int k, _Obj div_level);

  virtual _Obj Output(const Frame_<_Obj> &frame);
};



template<class _Obj>
class RangeIterator_ {
 public:
  RangeIterator_(const basic_k2tree<_Obj> *tree,
                 _Obj p1, _Obj p2,
                 _Obj q1, _Obj q2,
                 bool end);
  bool operator==(const RangeIterator_<_Obj> &rhs) {
    if (end_ || rhs.end_)
      return end_ == rhs.end_;
    else
      return p1_ == rhs.p1_ && p2_ == rhs.p2_ &&
             q1_ && rhs.q1_ && q2_ == rhs.q2_ &&
             curr_ == rhs.curr_;
  }
  bool operator!=(const RangeIterator_<_Obj> &rhs) {
    return !((*this) == rhs);
  }
  pair<_Obj, _Obj> operator*() {
    return curr_;
  }
  RangeIterator_<_Obj> operator++(int) {
    RangeIterator_<_Obj> i = *this;
    ++(*this);
    return i;
  }
  RangeIterator_<_Obj> operator++();

 private:
  const basic_k2tree<_Obj> *tree_;
  _Obj p1_, p2_, q1_, q2_;
  stack<RangeFrame_<_Obj>> frames_;
  pair<_Obj, _Obj> curr_;
  bool end_;
};
}  // namespace libk2tree
#endif  // INCLUDE_BITS_K2TREE_ITERATORS_H_
