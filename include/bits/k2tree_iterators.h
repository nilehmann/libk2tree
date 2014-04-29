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
#include <queue>
#include <list>
#include <utility>
#include <array>

namespace libk2tree {
namespace iterators {
using ::std::pair;
using ::std::queue;
using ::std::array;


template<class _Size>
struct Frame {
  _Size p, q;
  _Size z;
};

template<class _Size>
struct RangeFrame {
  _Size p1, p2, q1, q2;
  _Size dp, dq;
  _Size z;
};

template<class _Impl, class _Size>
class K2TreeIterator {
 public:
  static const K2TreeIterator end;

  K2TreeIterator(const basic_k2tree<_Size> *t, _Size object);

  inline bool operator==(const K2TreeIterator<_Impl, _Size> & rhs) {
    if (end_ || rhs.end_)
      return end_ == rhs.end_;
    else
      return object_ == rhs.object_ && curr_ == rhs.curr_;
  }

  inline bool operator!=(const K2TreeIterator<_Impl, _Size> & rhs) {
    return !((*this) == rhs);
  }

  inline _Size operator*() {
    return curr_;
  }


  inline void operator++() {
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

 protected:
  K2TreeIterator();
  const basic_k2tree<_Size> *tree_;
  _Size object_;
  _Size curr_;
  bool end_;
  queue<Frame<_Size> > queue_;

  void traverse();
};

template<class _Size>
struct DirectImpl {
  inline static Frame<_Size> FirstFrame(_Size p) {
    return {p, 0, 0};
  }
  inline static Frame<_Size> NextFrame(const Frame<_Size> &f,
                                       int j, int, _Size div_level) {
    return {f.p % div_level, f.q + div_level*j, f.z + j};
  }
  inline static _Size Offset(const Frame<_Size> &f, int k, _Size div_level) {
    return f.p/div_level*k;
  }

  inline static _Size Output(const Frame<_Size> &f) {
    return f.q;
  }
};


template<class _Size>
struct InverseImpl {
  inline static Frame<_Size> FirstFrame(_Size q) {
    return {0, q, 0};
  }
  inline static Frame<_Size> NextFrame(const Frame<_Size> &f,
                                       int j, int k, _Size div_level) {
    return {f.p + div_level*j, f.q % div_level, f.z + j*k};
  }
  inline static _Size Offset(const Frame<_Size> &f, int, _Size div_level) {
    return f.q/div_level;
  }
  inline static _Size Output(const Frame<_Size> &f) {
    return f.p;
  }
};



template<class _Size>
class RangeIterator {
 public:
  static const RangeIterator end;

  RangeIterator(const basic_k2tree<_Size> *tree,
                 _Size p1, _Size p2);
  inline bool operator==(const RangeIterator<_Size> &rhs) {
    if (end_ || rhs.end_)
      return end_ == rhs.end_;
    else
      return p1_ == rhs.p1_ && p2_ == rhs.p2_ &&
             curr_ == rhs.curr_;
  }
  inline bool operator!=(const RangeIterator<_Size> &rhs) {
    return !((*this) == rhs);
  }
  inline pair<_Size, _Size> operator*() {
    return curr_;
  }
  /*void operator++() {
    const BitSequence *T = tree_->T_;
    const BitArray<unsigned int, _Size> &L = tree_->L_;
    while (!queue_.empty()) {
      const RangeFrame<_Size> &f = queue_.front();
      if (L.GetBit(f.z - T->getLength())) {
        curr_ = make_pair(f.dp, f.dq);
        queue_.pop();
        return;
      }
      queue_.pop();
    }
    end_ = true;
  }*/
  inline void operator++() {
    if (!queue_.empty()) {
      const RangeFrame<_Size> &f = queue_.front();
      curr_ = make_pair(f.dp, f.dq);
      queue_.pop();
      return;
    }
    end_ = true;
  }

 private:
  RangeIterator();

  const basic_k2tree<_Size> *tree_;
  _Size p1_, p2_;
  pair<_Size, _Size> curr_;
  bool end_;
  queue<RangeFrame<_Size>> queue_;

  static vector<pair<_Size, _Size> > vec;

  void traverse();
};

}  // namespace iterators
}  // namespace libk2tree
#endif  // INCLUDE_BITS_K2TREE_ITERATORS_H_
