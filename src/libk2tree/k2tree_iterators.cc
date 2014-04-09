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

template<class _Impl, class _Size>
K2TreeIterator<_Impl, _Size>::K2TreeIterator(const basic_k2tree<_Size> *t,
                                               _Size object,
                                               bool end) :
    tree_(t), object_(object), curr_(0), frames_(), end_(end) {
  if (!end) {
    frames_.push(_Impl::FirstFrame(object, t->size_));
    ++(*this);
  }
}

template<class _Impl, class _Size>
void K2TreeIterator<_Impl, _Size>::operator++() {
  bool found = false;
  size_t *acum_rank = tree_->acum_rank_;
  const BitSequence *T = tree_->T_;
  const BitArray<unsigned int> *L = &tree_->L_;

  while (!found && !end_) {
    Frame_<_Size> &f= frames_.top();

    int k = tree_->GetK(f.level);
    _Size div_level = f.N/k;
    if (f.level < tree_->height_) {

      // Entering first time in frame
      if (f.j == -1) {
        if (f.level == 0 || T->access(f.z)) {
          f.z = _Impl::Rank(f, k, div_level, tree_);
          if (f.level > 0)
            f.nxt_offset=f.offset+(acum_rank[f.level]-acum_rank[f.level-1])*k*k;
          else
            f.nxt_offset = k*k;
        } else {
          frames_.pop();
          continue;
        }
      }

      f.j++;
      if (f.j < k) {
        frames_.push(_Impl::PushNextFrame(f, k, div_level));
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
}

/*
 * Implementation for direct adjacency list
 */
template<class _Size>
Frame_<_Size> DirectImpl<_Size>::FirstFrame(_Size p, _Size size) {
  return {-1, 0, 0, 0, size, p, 0, 0};
}

template<class _Size>
Frame_<_Size> DirectImpl<_Size>::PushNextFrame(const Frame_<_Size> &f,
                                               int k,
                                               _Size div_level) {
  return {-1, f.level + 1, f.nxt_offset, 0,
          div_level, f.p % div_level, f.q + div_level*f.j, f.z + f.j};
}

template<class _Size>
size_t DirectImpl<_Size>::Rank(const Frame_<_Size> &f, int k,
                               _Size div_level,
                               const basic_k2tree<_Size> *tree) {
  size_t z;
  z = f.z > 0 ? (tree->T_->rank1(f.z-1)-tree->acum_rank_[f.level-1])*k*k : 0;
  z += f.p/div_level*k + f.offset;
  return z;
}

template<class _Size>
_Size DirectImpl<_Size>::Output(const Frame_<_Size> &frame) {
  return frame.q;
}

/* 
 * Implementation for inverse adjacency list
 */
template<class _Size>
Frame_<_Size> InverseImpl<_Size>::FirstFrame(_Size q, _Size size) {
  return {-1, 0, 0, 0, size, 0, q, 0};
}

template<class _Size>
Frame_<_Size> InverseImpl<_Size>::PushNextFrame(const Frame_<_Size> &f,
                                            int k,
                                           _Size div_level) {
  return {-1, f.level + 1, f.nxt_offset, 0,
      div_level, f.p + div_level*f.j, f.q % div_level, f.z + f.j*k};
}
template<class _Size>
size_t InverseImpl<_Size>::Rank(const Frame_<_Size> &f, int k,
                                    _Size div_level,
                                    const basic_k2tree<_Size> *tree) {
  size_t z;
  z = f.z > 0 ? (tree->T_->rank1(f.z-1)-tree->acum_rank_[f.level-1])*k*k : 0;
  z += f.q/div_level + f.offset;
  return z;
}

template<class _Size>
_Size InverseImpl<_Size>::Output(const Frame_<_Size> &frame) {
  return frame.p;
}



template<class _Size>
RangeIterator_<_Size>::RangeIterator_(const basic_k2tree<_Size> *tree,
                                     _Size p1, _Size p2,
                                     _Size q1, _Size q2,
                                     bool end) :
    tree_(tree), p1_(p1), p2_(p2), q1_(q1), q2_(q2),
    frames_(), curr_(), end_(end) {
  if (!end) {
    frames_.push({true, 0, 0, 0, 0,
        tree->size_, p1, p2, q1, q2, 0, 0, 0});
    ++(*this);
  }
}


template<class _Size>
RangeIterator_<_Size> RangeIterator_<_Size>::operator++() {
  _Size pp1, pp2, qq1, qq2;
  bool found = false;
  size_t *acum_rank = tree_->acum_rank_;
  const BitSequence *T = tree_->T_;
  const BitArray<unsigned int> *L = &tree_->L_;

  while (!found && !end_) {
    RangeFrame_<_Size> &f= frames_.top();
    size_t &z = f.z;

    int k = tree_->GetK(f.level);
    _Size div_level = f.N/k;
    if (f.level < tree_->height_) {
      size_t nxt_offset;
      if (f.level > 0)
        nxt_offset = f.offset+(acum_rank[f.level] - acum_rank[f.level-1])*k*k;
      else
        nxt_offset = k*k;

      // Entering first time in frame
      if (f.first) {
        f.first = false;
        if (f.level == 0 || T->access(f.z)) {
          z = z > 0 ? (T->rank1(z-1) - acum_rank[f.level-1])*k*k : 0;
          z += f.offset;
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
        pp1 = f.i == f.p1/div_level ? f.p1 % div_level : 0;
        pp2 = f.i == f.p2/div_level ? f.p2 % div_level : div_level - 1;
        qq1 = f.j == f.q1/div_level ? f.q1 % div_level : 0;
        qq2 = f.j == f.q2/div_level ? f.q2 % div_level : div_level - 1;
        frames_.push({true, 0, 0, f.level + 1, nxt_offset,
            div_level, pp1, pp2, qq1, qq2,
            f.dp + div_level*f.i, f.dq + div_level*f.j, z + k*f.i+f.j});
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
  return *(this);
}

template class K2TreeIterator<DirectImpl<unsigned int>, unsigned int>;
template class K2TreeIterator<InverseImpl<unsigned int>, unsigned int>;
template class RangeIterator_<unsigned int>;

}  // namespace libk2tree
