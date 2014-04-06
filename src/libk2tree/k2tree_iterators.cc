/*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
* ----------------------------------------------------------------------------
*/

#include <bits/k2tree_iterators.h>
#include <bits/k2tree.h>

namespace libk2tree {

template<class self_type, class _Obj>
K2TreeIterator_<self_type, _Obj>::K2TreeIterator_(const basic_k2tree<_Obj> *t,
                                                 _Obj object,
                                                 bool end) :
    tree_(t), object_(object), curr_(0), frames_(), end_(end) {}

template<class self_type, class _Obj>
self_type K2TreeIterator_<self_type, _Obj>::operator++() {
  bool found = false;
  size_t *acum_rank = tree_->acum_rank_;
  const BitSequence *T = tree_->T_;
  const BitArray<unsigned int> *L = &tree_->L_;

  while (!found && !end_) {
    Frame_<_Obj> &f= frames_.top();


    int k = tree_->GetK(f.level);
    _Obj div_level = f.N/k;
    if (f.level < tree_->height_) {
      size_t nxt_offset;
      if (f.level > 0)
        nxt_offset = f.offset+(acum_rank[f.level] - acum_rank[f.level-1])*k*k;
      else
        nxt_offset = k*k;

      // Entering first time in frame
      if (f.j == -1) {
        if (f.level == 0 || T->access(f.z)) {
          f.z = Rank(f, k, div_level);
        } else {
          frames_.pop();
          continue;
        }
      }

      f.j++;
      if (f.j < k) {
        PushNextFrame(f, nxt_offset, k, div_level);
      } else {
        // No more children on the root
        if (f.level == 0)
          end_ = true;
        frames_.pop();
      }
    } else {
      // We enter and exit the leaf
      if (L->GetBit(f.z - T->getLength())) {
        curr_ = Output(f);
        found = true;
      }
      frames_.pop();
    }
  }
  return *static_cast<self_type*>(this);
}

template<class _Obj>
DirectIterator_<_Obj>::DirectIterator_(const basic_k2tree<_Obj> *tree,
                                       _Obj p, bool end) :
    K2TreeIterator_<DirectIterator_<_Obj>, _Obj>(tree, p, end) {
  this->frames_.push({-1, 0, 0, tree->size_, p, 0, 0});
  ++(*this);
}

template<class _Obj>
void DirectIterator_<_Obj>::PushNextFrame(const Frame_<_Obj> &f,
                                          size_t nxt_offset, int ,
                                          _Obj div_level) {
  this->frames_.push({-1, f.level + 1, nxt_offset,
      div_level, f.p % div_level, f.q + div_level*f.j, f.z + f.j});
}

template<class _Obj>
size_t DirectIterator_<_Obj>::Rank(const Frame_<_Obj> &f, int k,
                                   _Obj div_level) {
  size_t z;
  const basic_k2tree<_Obj> *tree = this->tree_;
  z = f.z > 0 ? (tree->T_->rank1(f.z-1)-tree->acum_rank_[f.level-1])*k*k : 0;
  z += f.p/div_level*k + f.offset;
  return z;
}

template<class _Obj>
_Obj DirectIterator_<_Obj>::Output(const Frame_<_Obj> &frame) {
  return frame.q;
}


template<class _Obj>
InverseIterator_<_Obj>::InverseIterator_(const basic_k2tree<_Obj> *tree,
                                         _Obj q, bool end) :
    K2TreeIterator_<InverseIterator_<_Obj>, _Obj>(tree, q, end) {
  this->frames_.push({-1, 0, 0, tree->size_, 0, q, 0});
  ++(*this);
}

template<class _Obj>
void InverseIterator_<_Obj>::PushNextFrame(const Frame_<_Obj> &f,
                                           size_t nxt_offset, int k,
                                           _Obj div_level) {
  this->frames_.push({-1, f.level + 1, nxt_offset,
      div_level, f.p + div_level*f.j, f.q % div_level, f.z + f.j*k});
}
template<class _Obj>
size_t InverseIterator_<_Obj>::Rank(const Frame_<_Obj> &f, int k,
                                    _Obj div_level) {
  size_t z;
  const basic_k2tree<_Obj> *tree = this->tree_;
  z = f.z > 0 ? (tree->T_->rank1(f.z-1)-tree->acum_rank_[f.level-1])*k*k : 0;
  z += f.q/div_level + f.offset;
  return z;
}

template<class _Obj>
_Obj InverseIterator_<_Obj>::Output(const Frame_<_Obj> &frame) {
  return frame.p;
}



template<class _Obj>
RangeIterator_<_Obj>::RangeIterator_(const basic_k2tree<_Obj> *tree,
                                     _Obj p1, _Obj p2,
                                     _Obj q1, _Obj q2,
                                     bool end) :
    tree_(tree), p1_(p1), p2_(p2), q1_(q1), q2_(q2),
    frames_(), curr_(), end_(end) {
  frames_.push({true, 0, 0, 0, 0,
      tree->size_, p1, p2, q1, q2, 0, 0, 0});
  ++(*this);
}


template<class _Obj>
RangeIterator_<_Obj> RangeIterator_<_Obj>::operator++() {
  _Obj pp1, pp2, qq1, qq2;
  bool found = false;
  size_t *acum_rank = tree_->acum_rank_;
  const BitSequence *T = tree_->T_;
  const BitArray<unsigned int> *L = &tree_->L_;

  while (!found && !end_) {
    RangeFrame_<_Obj> &f= frames_.top();
    size_t &z = f.z;

    int k = tree_->GetK(f.level);
    _Obj div_level = f.N/k;
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

template class DirectIterator_<unsigned int>;
template class InverseIterator_<unsigned int>;
template class RangeIterator_<unsigned int>;

}  // namespace libk2tree
