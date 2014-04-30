/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_BITS_BASIC_K2TREE_H_
#define INCLUDE_BITS_BASIC_K2TREE_H_

#include <bits/utils/bitarray.h>
#include <bits/utils/utils.h>
#include <BitSequence.h>  // libcds
#include <vector>
#include <stack>
#include <fstream>
#include <cstdlib>
#include <queue>


namespace libk2tree {
using utils::BitArray;
using cds_static::BitSequence;
using cds_static::BitSequenceRG;
using std::vector;
using std::stack;
using std::ifstream;
using std::ofstream;
using utils::LoadValue;
using utils::SaveValue;
using std::queue;


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



template<class _Size> class basic_k2treebuilder;

template<class _Size> struct DirectImpl;
template<class _Size> struct InverseImpl;

template<class _Size>
class basic_k2tree {
  friend class basic_k2treebuilder<_Size>;

 public:

  /*
   * Load a K2Tree previously saved with Save(ofstream)
   */
  explicit basic_k2tree(ifstream *in);

  /* Check if exist a link from object p to q.
   * Identifiers starts with 0.
   *
   * @param p Identifier of first object.
   * @param q Identifier of second object.
   */
  bool CheckLink(_Size p, _Size q) const;

  /* 
   * Save the k2tree to a file
   */
  void Save(ofstream *out) const;

  /*
   * Method implemented for testing reasons
   */
  bool operator==(const basic_k2tree &rhs) const;

  inline _Size cnt() const {
    return cnt_;
  }


  ~basic_k2tree();

  template<class Function>
  void Direct(_Size p, Function fun) const {
    AdjacencyList<Function, DirectImpl<_Size>>(p, fun);
  }
  template<class Function>
  void Inverse(_Size q, Function fun) const {
    AdjacencyList<Function, InverseImpl<_Size>>(q, fun);
  }

  template<class Function>
  void Range(_Size p1, _Size p2, _Size q1, _Size q2, Function fun) const {
    assert(p1 <= p2 && q1 <= q2);

    _Size div_level;
    _Size pp1, pp2, qq1, qq2, dp, dq;
    queue<RangeFrame<_Size>> queue_;

    queue_.push({p1, p2, q1, q2, 0, 0, 0});
    _Size N = size_;
    int level;
    for (level = 0; level < height_-1; ++level) {
      int k = GetK(level);
      div_level = N/k;

      _Size cnt_level = queue_.size();
      for (_Size q = 0; q < cnt_level; ++q) {
        RangeFrame<_Size> &f = queue_.front();
        f.z = GetFirstChild(f.z, level, k);

        _Size div_p1 = f.p1/div_level;
        _Size rem_p1= f.p1%div_level;
        _Size div_p2 = f.p2/div_level;
        _Size rem_p2 = f.p2%div_level;
        for (_Size i = div_p1; i <= div_p2; ++i) {
          _Size z = f.z + k*i;
          dp = f.dp + div_level*i;
          pp1 = i == div_p1? rem_p1 : 0;
          pp2 = i == div_p2 ? rem_p2 : div_level - 1;

          _Size div_q1 = f.q1/div_level;
          _Size rem_q1 = f.q1%div_level;
          _Size div_q2 = f.q2/div_level;
          _Size rem_q2 = f.q2%div_level;
          for (_Size j = div_q1; j <= div_q2; ++j) {
            dq = f.dq + div_level*j;
            qq1 = j == div_q1 ? rem_q1 : 0;
            qq2 = j == div_q2 ? rem_q2 : div_level-1;
            if (T_->access(z+j))
              queue_.push({pp1, pp2, qq1, qq2, dp, dq, z + j});
          }
        }
        queue_.pop();
      }
      N = div_level;
    }


    div_level = N/kl_;
    _Size cnt_level = queue_.size();
    for (_Size q = 0; q < cnt_level; ++q) {
      RangeFrame<_Size> &f = queue_.front();
      f.z = GetFirstChild(f.z, level, kl_);

      _Size div_p1 = f.p1/div_level;
      _Size div_p2 = f.p2/div_level;
      for (_Size i = div_p1; i <= div_p2; ++i) {
        _Size z = f.z + kl_*i;
        dp = f.dp + div_level*i;

        _Size div_q1 = f.q1/div_level;
        _Size div_q2 = f.q2/div_level;
        for (_Size j = div_q1; j <= div_q2; ++j) {
          dq = f.dq + div_level*j;
          if ( L_.GetBit(z+j - T_->getLength()))
            fun(dp, dq);
        }
      }
      queue_.pop();
    }


  }

 private:
  /* 
   * Construct a k2tree with and hybrid aproach. This construtor should
   * be called from a proper builder.
   *
   * @param T Bit array with the internal nodes.
   * @param L Bit array with the leafs.
   * @param k1 Arity of the first levels.
   * @param k2 Arity of the second part.
   * @param kl Arity of the level height-1.
   * @param max_level_k1 Last level with arity k1.
   * @param height Height of the tree.
   * @param cnt Number of object in the relation.
   * @param size Size of the expanded matrix.
   */
  basic_k2tree(const BitArray<unsigned int, _Size> &T,
               const BitArray<unsigned int, _Size> &L,
               int k1, int k2, int kl, int max_level_k1, int height,
               _Size cnt, _Size size);

  inline _Size  GetFirstChild(_Size z, int level, int k) const {
    // child_l(x,i) = rank(T_l, z - 1)*kl*kl + i - 1;
    z = z > 0 ? (T_->rank1(z-1) - acum_rank_[level-1])*k*k : 0;
    return z + offset_[level];
  }
  // Bit array with rank capability containing internal nodes.
  BitSequence *T_;
  // Bit array for the leafs.
  BitArray<unsigned int, _Size> L_;
  // Arity of the first part.
  int k1_;
  // Arity of the second part.
  int k2_;
  // Arity of the level height-1.
  int kl_;
  // Last level with arity k1.
  int max_level_k1_;
  // Height of the tree.
  int height_;
  // Number of object.
  _Size cnt_;
  // Size of the expanded matrix.
  _Size size_;
  // Accumulated rank for each level.
  _Size *acum_rank_;
  // Accumulated number of nodes util each level, inclusive.
  _Size *offset_;

  inline int GetK(int level) const {
    if (level <= max_level_k1_)  return k1_;
    else if (level < height_ - 1)  return k2_;
    else  return kl_;
  }

  template<class Function, class _Impl>
  void AdjacencyList(_Size object, Function fun) const {
    _Size div_level;
    _Size cnt_level;
    int k, level;
    queue<Frame<_Size> > queue_;

    queue_.push(_Impl::FirstFrame(object));
    _Size N = size_;
    for (level = 0; level < height_ - 1; ++level) {
      k = GetK(level);
      div_level = N/k;

      cnt_level = queue_.size();
      for (_Size i = 0; i < cnt_level; ++i) {
        Frame<_Size> &f = queue_.front();
        f.z = GetFirstChild(f.z, level, k);
        f.z += _Impl::Offset(f, k, div_level);
        for (int j  = 0; j < k; ++j) {
          Frame<_Size> nf = _Impl::NextFrame(f, j, k, div_level);
          if (T_->access(nf.z))
            queue_.push(nf);
        }
        queue_.pop();
      }
      N = div_level;
    }

    div_level = N/kl_;
    cnt_level = queue_.size();
    for (_Size i = 0; i < cnt_level; ++i) {
      Frame<_Size> &f = queue_.front();
      f.z = GetFirstChild(f.z, level, kl_);
      f.z += _Impl::Offset(f, kl_, div_level);
      for (int j  = 0; j < kl_; ++j) {
        Frame<_Size> nf = _Impl::NextFrame(f, j, kl_, div_level);
        if (L_.GetBit(nf.z - T_->getLength()))
          fun(_Impl::Output(nf));
      }
      queue_.pop();
    }
  }

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




}  // namespace libk2tree

#endif  // INCLUDE_BITS_BASIC_K2TREE_H_
