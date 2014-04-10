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
using ::std::pair;
using ::std::queue;


template<class _Size>
struct Frame_ {
  int j;
  int level;
  _Size N;
  _Size p, q;
  _Size z;
};

template<class _Size>
struct RangeFrame_ {
  //bool first;
  unsigned int i, j;
  int level;
  _Size N;
  _Size p1, p2, q1, q2;
  _Size dp, dq;
  _Size z;
};

template<class _Impl, class _Size>
class K2TreeIterator {
 public:
  K2TreeIterator(const basic_k2tree<_Size> *t, _Size object, bool end);

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
      const Frame_<_Size> &f = queue_.front();
      if (L.GetBit(f.z - T->getLength())) {
        curr_ = _Impl::Output(f);
        queue_.pop();
        return;
      }
      queue_.pop();
    }
    end_ = true;
  }
  const static K2TreeIterator end;
 protected:
  const basic_k2tree<_Size> *tree_;
  _Size object_;
  _Size curr_;
  //stack<Frame_<_Size>> frames_;
  bool end_;
  queue<Frame_<_Size>> queue_;

  inline void traverse();
};

template<class _Size>
struct DirectImpl {
  inline static Frame_<_Size> FirstFrame(_Size p, _Size size) {
    return {-1, 0, size, p, 0, 0};
  }
  inline static Frame_<_Size> NextFrame(const Frame_<_Size> &f,
                                        int k,
                                         _Size div_level) {
    return {-1, f.level + 1,
            div_level, f.p % div_level, f.q + div_level*f.j, f.z + f.j};
  }
  inline static _Size Rank(const Frame_<_Size> &f, int k, _Size div_level,
                           const basic_k2tree<_Size> *t) {
    _Size z;
    z = f.z > 0 ? (t->T_->rank1(f.z-1)-t->acum_rank_[f.level-1])*k*k : 0;
    z += f.p/div_level*k + t->offset_[f.level];
    return z;
  }

  inline static _Size Output(const Frame_<_Size> &frame) {
    return frame.q;
  }
};


template<class _Size>
struct InverseImpl {
  inline static Frame_<_Size> FirstFrame(_Size q, _Size size) {
    return {-1, 0, size, 0, q, 0};
  }
  inline static Frame_<_Size> NextFrame(const Frame_<_Size> &f, 
                                        int k,
                                        _Size div_level) {
    return {-1, f.level + 1,
        div_level, f.p + div_level*f.j, f.q % div_level, f.z + f.j*k};
  }
  inline static _Size Rank(const Frame_<_Size> &f, int k, _Size div_level,
                           const basic_k2tree<_Size> *tree) {
    _Size z;
    z = f.z > 0 ? (tree->T_->rank1(f.z-1)-tree->acum_rank_[f.level-1])*k*k : 0;
    z += f.q/div_level + tree->offset_[f.level];
    return z;
  }
  inline static _Size Output(const Frame_<_Size> &frame) {
    return frame.p;
  }
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
  //void operator++();
  inline void operator++() {
    const BitSequence *T = tree_->T_;
    const BitArray<unsigned int, _Size> &L = tree_->L_;
    while (!queue_.empty()) {
      const RangeFrame_<_Size> &f = queue_.front();
      if (L.GetBit(f.z - T->getLength())) {
        curr_ = Output(f);
        queue_.pop();
        return;
      }
      queue_.pop();
    }
    end_ = true;
  }
  const static RangeIterator_ end;

 private:
  const basic_k2tree<_Size> *tree_;
  _Size p1_, p2_, q1_, q2_;
  stack<RangeFrame_<_Size>> frames_;
  pair<_Size, _Size> curr_;
  bool end_;
  queue<RangeFrame_<_Size>> queue_;

  inline RangeFrame_<_Size> FirstFrame(_Size p1, _Size p2, _Size q1, _Size q2) {
    return {0, 0, 0, tree_->size_, p1, p2, q1, q2, 0, 0, 0};
  }
  inline _Size Rank(const RangeFrame_<_Size> f, int k) {
    const BitSequence *T = tree_->T_;
    _Size *acum_rank = tree_->acum_rank_;
    _Size z;
    z = f.z > 0 ? (T->rank1(f.z-1) - acum_rank[f.level-1])*k*k : 0;
    z += tree_->offset_[f.level];
    return z;
  }

  inline RangeFrame_<_Size> NextFrame(const RangeFrame_<_Size> f, int k,
                                      _Size div_level) {
      _Size pp1 = f.i == f.p1/div_level ? f.p1 % div_level : 0;
      _Size pp2 = f.i == f.p2/div_level ? f.p2 % div_level : div_level - 1;
      _Size qq1 = f.j == f.q1/div_level ? f.q1 % div_level : 0;
      _Size qq2 = f.j == f.q2/div_level ? f.q2 % div_level : div_level - 1;
      return {0, 0, f.level + 1,
          div_level, pp1, pp2, qq1, qq2,
          f.dp + div_level*f.i, f.dq + div_level*f.j, f.z + k*f.i+f.j};
  }
  inline pair<_Size, _Size> Output(const RangeFrame_<_Size> f) {
    return make_pair(f.dp, f.dq);
  }
  inline void traverse();
    

};
}  // namespace libk2tree
#endif  // INCLUDE_BITS_K2TREE_ITERATORS_H_
