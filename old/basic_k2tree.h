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

#include <libk2tree_basic.h>
#include <leaves_compression/hash.h>
#include <leaves_compression/leaves_compressor.h>
#include <bits/utils/bitarray.h>
#include <bits/utils/utils.h>
#include <leaves_compression/leaves_compressor.h>
#include <BitSequence.h>  // libcds
#include <dacs.h>
#include <fstream>
#include <cstdlib>
#include <queue>
#include <memory>
#include <algorithm>


namespace libk2tree {
using utils::BitArray;
using cds_static::BitSequence;
using cds_static::BitSequenceRG;
using std::ifstream;
using std::ofstream;
using std::queue;
using std::unique_ptr;
using utils::Ceil;
using leaves_compression::HashTable;


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

/*
 * Teplate implementation for a compact representation of binary relations or
 * matrices using a k2tree. The template parameter specifies the integral type
 * able to hold the number of objects in the relation. The library gives
 * precompiled instances for uint (alias K2Tree) and size_t (alias LongK2Tree)
 * Object identifiers starts with 0.
 */
template<class _Size>
class basic_k2tree {
  friend class basic_k2treebuilder<_Size>;
 public:
  typedef _Size words_cnt;
  typedef _Size obj_cnt;
  /*
   * Loads a K2Tree previously saved with Save(ofstream)
   */
  explicit basic_k2tree(ifstream *in);

  /* 
   * Saves the k2tree to a file
   */
  void Save(ofstream *out) const;

  /*
   * Destructor
   */
  ~basic_k2tree();

  /*
   * Returns the number of objects in the original relation
   */
  inline obj_cnt cnt() const {
    return cnt_;
  }

  /* Checks if exist a link from object p to q.
   *
   * @param p Identifier of first object.
   * @param q Identifier of second object.
   */
  bool CheckLink(_Size p, _Size q) const;

  /*
   * Iterates over all links in the given row.
   *
   * @param p Row in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each
   * object related to p.
   * The function expect a parameter of type _Size.
   */
  template<class Function>
  void DirectLinks(_Size p, Function fun) const {
    Neighbors<Function, DirectImpl<_Size>>(p, fun);
  }

  /*
   * Iterates over all links in the given column.
   *
   * @param q Column in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each 
   * object related to q.
   * The function expect a parameter of type _Size.
   */
  template<class Function>
  void InverseLinks(_Size q, Function fun) const {
    Neighbors<Function, InverseImpl<_Size>>(q, fun);
  }


  /*
   * Return number of words of kl*kl bits in the leaf level.
   */
  words_cnt WordsCnt() const {
    return L_->length()/kl_/kl_;
  }

  /*
   * Return the size in bytes of the words in the leaf level, ie, kl*kl/8.
   */
  uint WordSize() const {
    return Ceil(kl_*kl_, 8);
  }

  /*
   * Iterates over the words in the leaf level.
   * @param fun Pointer to function, functor or lambda expecting a
   * uchar*
   */
  template<class Function>
  void Words(Function fun) const {
    words_cnt cnt = WordsCnt();
    uint size = WordSize();

    _Size bit = 0;
    for (words_cnt i = 0; i < cnt; ++i) {
      uchar *word = new uchar[size];
      std::fill(word, word + size, 0);
      for (int j = 0; j < kl_*kl_; ++j, ++bit) {
        if (L_->GetBit(bit))
          word[j/8] |= (1 << (j%8));
      }
      fun(word);
      delete [] word;
    }
    
  }


  void CompressLeaves() {
    leaves_compression::Compress(*this);    
  }

  void CompressLeaves(const HashTable &table, shared_ptr<uchar> voc);

  /*
   * Iterates over all links in the specified submatrix.
   *
   * @param p1 Starting row in the matrix.
   * @param p2 Ending row in the matrix.
   * @param q1 Starting column in the matrix.
   * @param q2 Ending column in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each 
   * pair of objects. The function expect two parameters of type _Size
   */
  template<class Function>
  void RangeQuery(_Size p1, _Size p2, _Size q1, _Size q2, Function fun) const {
    assert(p1 <= p2 && q1 <= q2);

    _Size div_level;
    _Size div_p1, rem_p1, div_p2, rem_p2;
    _Size div_q1, rem_q1, div_q2, rem_q2;
    _Size dp, dq;
    queue<RangeFrame<_Size>> queue;

    queue.push({p1, p2, q1, q2, 0, 0, 0});
    _Size N = size_;
    int level;
    for (level = 0; level < height_-1; ++level) {
      int k = GetK(level);
      div_level = N/k;

      _Size cnt_level = queue.size();
      for (_Size q = 0; q < cnt_level; ++q) {
        RangeFrame<_Size> &f = queue.front();
        f.z = FirstChild(f.z, level, k);

        div_p1 = f.p1/div_level, rem_p1= f.p1%div_level;
        div_p2 = f.p2/div_level, rem_p2 = f.p2%div_level;
        for (_Size i = div_p1; i <= div_p2; ++i) {
          _Size z = f.z + k*i;
          dp = f.dp + div_level*i;
          p1 = i == div_p1 ? rem_p1 : 0;
          p2 = i == div_p2 ? rem_p2 : div_level - 1;

          div_q1 = f.q1/div_level, rem_q1 = f.q1%div_level;
          div_q2 = f.q2/div_level, rem_q2 = f.q2%div_level;
          for (_Size j = div_q1; j <= div_q2; ++j) {
            dq = f.dq + div_level*j;
            q1 = j == div_q1 ? rem_q1 : 0;
            q2 = j == div_q2 ? rem_q2 : div_level-1;
            if (T_->access(z+j))
              queue.push({p1, p2, q1, q2, dp, dq, z + j});
          }
        }
        queue.pop();
      }
      N = div_level;
    }


    div_level = N/kl_;
    _Size cnt_level = queue.size();
    for (_Size q = 0; q < cnt_level; ++q) {
      RangeFrame<_Size> &f = queue.front();
      f.z = FirstChild(f.z, level, kl_);

      div_p1 = f.p1/div_level;
      div_p2 = f.p2/div_level;
      for (_Size i = div_p1; i <= div_p2; ++i) {
        _Size z = f.z + kl_*i;
        dp = f.dp + div_level*i;

        div_q1 = f.q1/div_level;
        div_q2 = f.q2/div_level;
        for (_Size j = div_q1; j <= div_q2; ++j) {
          dq = f.dq + div_level*j;
          if ( L_->GetBit(z+j - T_->getLength()))
            fun(dp, dq);
        }
      }
      queue.pop();
    }
  }

  /* 
   * Print a summary of the memor usage.
   */
  void Memory() const;
  /*
   * Get size in bytes.
   */
  size_t GetSize() const;

  /*
   * Method implemented for testing reasons
   */
  bool operator==(const basic_k2tree &rhs) const;

 private:
  /* 
   * Builds a k2tree with a hybrid aproach. This construtor should
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
  basic_k2tree(const BitArray<uint, _Size> &T,
               const BitArray<uint, _Size> &L,
               int k1, int k2, int kl, int max_level_k1, int height,
               _Size cnt, _Size size);

  /*
   * Gets the position of the first child of the specified node
   *
   * @param z Position of the node
   * @param level
   * @param k
   */
  inline _Size  FirstChild(_Size z, int level, int k) const {
    // child_l(x,i) = rank(T_l, z - 1)*kl*kl + i - 1;
    z = z > 0 ? (T_->rank1(z-1) - acum_rank_[level-1])*k*k : 0;
    return z + offset_[level];
  }

  /*
   * Gets the k corresponding to the given level.
   *
   * @param level
   */
  inline int GetK(int level) const {
    if (level <= max_level_k1_)  return k1_;
    else if (level < height_ - 1)  return k2_;
    else  return kl_;
  }

  template<class Function, class _Impl>
  void LeafBits(Frame<_Size> f, uint div_level, Function fun) const {
    _Size first = f.z = FirstChild(f.z, height_ - 1, kl_);
    f.z += _Impl::Offset(f, kl_, div_level);
    if (compressL_  == NULL) {
      for (int j  = 0; j < kl_; ++j) {
        Frame<_Size> newf = _Impl::NextFrame(f, j, kl_, div_level);
        if (L_->GetBit(newf.z - T_->getLength()))
          fun(_Impl::Output(newf));
      }
    }
    else {
      uint size = WordSize();
      uint pos = f.z - T_->getLength();
      uint iword = accessFT(compressL_, pos/(size*8));
      uchar *word = vocabulary_.get()+iword*size;

      for (int j  = 0; j < kl_; ++j) {
        Frame<_Size> newf = _Impl::NextFrame(f, j, kl_, div_level);
        pos = newf.z - first;
        if ((word[pos/8] >> (pos%8)) & 1)
          fun(_Impl::Output(newf));
      }
    }

  }

  /*
   * Template implementation for DirectLinks and InverseLinks
   */
  template<class Function, class _Impl>
  void Neighbors(_Size object, Function fun) const {
    _Size div_level;
    _Size cnt_level;
    int k, level;
    queue<Frame<_Size> > queue; 

    queue.push(_Impl::FirstFrame(object));
    _Size N = size_;
    for (level = 0; level < height_ - 1; ++level) {
      k = GetK(level);
      div_level = N/k;

      cnt_level = queue.size();
      for (_Size i = 0; i < cnt_level; ++i) {
        Frame<_Size> &f = queue.front();
        f.z = FirstChild(f.z, level, k);
        f.z += _Impl::Offset(f, k, div_level);
        for (int j  = 0; j < k; ++j) {
          Frame<_Size> newf = _Impl::NextFrame(f, j, k, div_level);
          if (T_->access(newf.z))
            queue.push(newf);
        }
        queue.pop();
      }
      N = div_level;
    }

    div_level = N/kl_;
    cnt_level = queue.size();
    for (_Size i = 0; i < cnt_level; ++i) {
      Frame<_Size> &f = queue.front();
      LeafBits<Function, _Impl>(f, div_level, fun);

      queue.pop();
    }
  }

  // Bit array with rank capability containing internal nodes.
  BitSequence *T_;
  // Bit array for the leafs.
  BitArray<uint, _Size> *L_;
  FTRep* compressL_;
  shared_ptr<uchar> vocabulary_;
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
