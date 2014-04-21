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
#include <utility>

namespace libk2tree {
namespace iterators {
using ::std::pair;
using ::std::queue;


template<class _Size>
struct Frame {
  int j;
  int level;
  _Size p, q;
  _Size z;
};

template<class _Size>
struct RangeFrame {
  _Size p1, p2;
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

  inline int operator*() {
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
  queue<Frame<_Size>> queue_;

  inline void traverse();
};

template<class _Size>
struct DirectImpl {
  inline static Frame<_Size> FirstFrame(_Size p) {
    return {-1, 0, p, 0, 0};
  }
  inline static Frame<_Size> NextFrame(const Frame<_Size> &f,
                                        int,
                                        _Size div_level) {
    return {-1, f.level + 1,
            f.p % div_level, f.q + div_level*f.j, f.z + f.j};
  }
  inline static _Size Rank(const Frame<_Size> &f, int k, _Size div_level,
                           const basic_k2tree<_Size> *t) {
    _Size z;
    z = f.z > 0 ? (t->T_->rank1(f.z-1)-t->acum_rank_[f.level-1])*k*k : 0;
    z += f.p/div_level*k + t->offset_[f.level];
    return z;
  }

  inline static _Size Output(const Frame<_Size> &f) {
    return f.q;
  }
};


template<class _Size>
struct InverseImpl {
  inline static Frame<_Size> FirstFrame(_Size q) {
    return {-1, 0, 0, q, 0};
  }
  inline static Frame<_Size> NextFrame(const Frame<_Size> &f,
                                        int k,
                                        _Size div_level) {
    return {-1, f.level + 1,
            f.p + div_level*f.j, f.q % div_level, f.z + f.j*k};
  }
  inline static _Size Rank(const Frame<_Size> &f, int k, _Size div_level,
                           const basic_k2tree<_Size> *tree) {
    _Size z;
    z = f.z > 0 ? (tree->T_->rank1(f.z-1)-tree->acum_rank_[f.level-1])*k*k : 0;
    z += f.q/div_level + tree->offset_[f.level];
    return z;
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
  inline void operator++() {
    const BitSequence *T = tree_->T_;
    const BitArray<unsigned int, _Size> &L = tree_->L_;
    while (!queue_.empty()) {
      const RangeFrame<_Size> &f = queue_.front();
      if (L.GetBit(f.z - T->getLength())) {
        curr_ = Output(f);
        queue_.pop();
        return;
      }
      queue_.pop();
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

  inline RangeFrame<_Size> FirstFrame(_Size p1, _Size p2) {
    return {p1, p2, 0, 0, 0};
  }
  inline _Size Rank(const RangeFrame<_Size> f, int level, int k) {
    const BitSequence *T = tree_->T_;
    _Size *acum_rank = tree_->acum_rank_;
    _Size z;
    z = f.z > 0 ? (T->rank1(f.z-1) - acum_rank[level-1])*k*k : 0;
    z += tree_->offset_[level];
    return z;
  }

/*  inline RangeFrame<_Size> NextFrame(const RangeFrame<_Size> f, int k,
                                      _Size div_level) {
      _Size pp1 = f.i == f.p1/div_level ? f.p1 % div_level : 0;
      _Size pp2 = f.i == f.p2/div_level ? f.p2 % div_level : div_level - 1;
      _Size qq1 = f.j == f.q1/div_level ? f.q1 % div_level : 0;
      _Size qq2 = f.j == f.q2/div_level ? f.q2 % div_level : div_level - 1;
      return {0, 0, f.level + 1,
          div_level, pp1, pp2, qq1, qq2,
          f.dp + div_level*f.i, f.dq + div_level*f.j, f.z + k*f.i+f.j};
  }*/
  inline pair<_Size, _Size> Output(const RangeFrame<_Size> f) {
    return make_pair(f.dp, f.dq);
  }
  inline void traverse();
};

}  // namespace iterators
}  // namespace libk2tree
#endif  // INCLUDE_BITS_K2TREE_ITERATORS_H_
