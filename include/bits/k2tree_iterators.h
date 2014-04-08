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

template<class _Size>
struct Frame_ {
  int j;
  int level;
  size_t offset, nxt_offset;
  _Size N;
  _Size p, q;
  size_t z;
};

template<class _Size>
struct RangeFrame_ {
  bool first;
  unsigned int i, j;
  int level;
  size_t offset;
  _Size N;
  _Size p1, p2, q1, q2;
  _Size dp, dq;
  size_t z;
};

template<class _Impl, class _Size>
class K2TreeIterator_ {
 public:
  K2TreeIterator_(const basic_k2tree<_Size> *t, _Size object, bool end);

  bool operator==(const K2TreeIterator_<_Impl, _Size> & rhs) {
    if (end_ || rhs.end_)
      return end_ == rhs.end_;
    else
      return object_ == rhs.object_ && curr_ == rhs.curr_;
  }

  bool operator!=(const K2TreeIterator_<_Impl, _Size> & rhs) {
    return !((*this) == rhs);
  }

  int operator*() {
    return curr_;
  }

  K2TreeIterator_<_Impl, _Size> operator++(int) {
    K2TreeIterator_<_Impl, _Size> i = *this;
    ++(*this);
    return i;
  }
  bool HasNext() {
    return !end_;
  }
  void operator++();

 protected:
  const basic_k2tree<_Size> *tree_;
  _Size object_;
  _Size curr_;
  stack<Frame_<_Size>> frames_;
  bool end_;
/*
  virtual void PushNextFrame(const Frame_<_Size> &frame,
                             size_t nxt_offset, int k,
                             _Size div_level) = 0;
  virtual size_t Rank(const Frame_<_Size> &frame, int k, _Size div_level) = 0;
  virtual _Size Output(const Frame_<_Size> &frame) = 0;*/
};

template<class _Size>
class DirectImpl {
 public:

 //private:
  inline static Frame_<_Size> FirstFrame(_Size p, _Size size);
  inline static Frame_<_Size> PushNextFrame(const Frame_<_Size> &f,
                                             int ,
                                            _Size div_level);
  inline static size_t Rank(const Frame_<_Size> &f, int k, _Size div_level,
                     const basic_k2tree<_Size> *tree);

  inline static _Size Output(const Frame_<_Size> &frame);
};


template<class _Size>
class InverseImpl {
 public:
  inline static Frame_<_Size> FirstFrame(_Size q, _Size size);
  inline static Frame_<_Size> PushNextFrame(const Frame_<_Size> &f, 
                                            int k,
                                            _Size div_level);
  inline static size_t Rank(const Frame_<_Size> &f, int k, _Size div_level,
                     const basic_k2tree<_Size> *tree);
  inline static _Size Output(const Frame_<_Size> &frame);
};



template<class _Size>
class RangeIterator_ {
 public:
  RangeIterator_(const basic_k2tree<_Size> *tree,
                 _Size p1, _Size p2,
                 _Size q1, _Size q2,
                 bool end);
  bool operator==(const RangeIterator_<_Size> &rhs) {
    if (end_ || rhs.end_)
      return end_ == rhs.end_;
    else
      return p1_ == rhs.p1_ && p2_ == rhs.p2_ &&
             q1_ && rhs.q1_ && q2_ == rhs.q2_ &&
             curr_ == rhs.curr_;
  }
  bool operator!=(const RangeIterator_<_Size> &rhs) {
    return !((*this) == rhs);
  }
  pair<_Size, _Size> operator*() {
    return curr_;
  }
  RangeIterator_<_Size> operator++(int) {
    RangeIterator_<_Size> i = *this;
    ++(*this);
    return i;
  }
  RangeIterator_<_Size> operator++();

 private:
  const basic_k2tree<_Size> *tree_;
  _Size p1_, p2_, q1_, q2_;
  stack<RangeFrame_<_Size>> frames_;
  pair<_Size, _Size> curr_;
  bool end_;
};
}  // namespace libk2tree
#endif  // INCLUDE_BITS_K2TREE_ITERATORS_H_
