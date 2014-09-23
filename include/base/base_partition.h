/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_BASE_BASE_PARTITION_H_
#define INCLUDE_BASE_BASE_PARTITION_H_

#include <libk2tree_basic.h>
#include <utils/utils.h>
#include <fstream>
#include <vector>

namespace libk2tree {
using utils::LoadValue;
using utils::SaveValue;

template<class K2Tree>
class base_partition {
 public:


  /** 
   * Checks if exist a link from object p to q.
   *
   * This member function effectively calls member CheckLink of the
   * corresponding subtree.
   *
   * @param p Identifier of first object.
   * @param q Identifier of second object.
   */
  bool CheckLink(cnt_size p, cnt_size q) const {
    const K2Tree &t = subtrees_[p/submatrix_size_][q/submatrix_size_];
    return t.CheckLink(p % submatrix_size_, q % submatrix_size_);
  }

  /**
   * Returns number of links in the relation (ones in the matrix)
   * @return Number of links
   */
  size_t links() const {
    size_t l = 0;
    for (uint i = 0; i < k0_; ++i)
      for (uint j = 0; j < k0_; ++j)
        l += subtrees_[i][j].links();
    return l;
  }

  /**
   * Iterates over all links in the given row.
   * 
   * This member function effectively calls member DirectLinks of the
   * corresponding subtrees.
   *
   * @param p Row in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each
   * object related to p.
   * The function expect a parameter of type cnt_size.
   */
  template<class Function>
  void DirectLinks(cnt_size p, Function fun) const {
    uint row = (uint) (p/submatrix_size_);
    for (uint col = 0; col < k0_; ++col) {
      const K2Tree &tree = subtrees_[row][col];
      tree.DirectLinks(p % submatrix_size_, [=] (cnt_size q) {
        fun(col*submatrix_size_ + q);
      });
    }
  }

  /*
   * Iterates over all links in the given column.
   *
   * This member function effectively calls member InverseLinks of the
   * corresponding subtrees.
   *
   * @param q Column in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each
   * object related to q.
   * The function expect a parameter of type cnt_size.
   */
  template<class Function>
  void InverseLinks(cnt_size q, Function fun) const {
    uint col = (uint) (q/submatrix_size_);
    for (uint row = 0; row < k0_; ++row) {
      const K2Tree &tree = subtrees_[row][col];
      tree.InverseLinks(q % submatrix_size_, [=] (cnt_size p) {
        fun(row*submatrix_size_ + p);
      });
    }
  }

  /*
   * Iterates over all links in the specified submatrix.
   *
   * This member function effectively calls member RangeQuery of the
   * corresponding subtrees.
   *
   * @param p1 Starting row in the matrix.
   * @param p2 Ending row in the matrix.
   * @param q1 Starting column in the matrix.
   * @param q2 Ending column in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each pair
   * of objects. The function expect two parameters of type cnt_size.
   */
  template<class Function>
  void RangeQuery(cnt_size p1, cnt_size p2,
                  cnt_size q1, cnt_size q2,
                  Function fun) const {
    cnt_size div_p1 = p1/submatrix_size_, rem_p1 = p1%submatrix_size_;
    cnt_size div_p2 = p2/submatrix_size_, rem_p2 = p2%submatrix_size_;
    cnt_size div_q1 = q1/submatrix_size_, rem_q1 = q1%submatrix_size_;
    cnt_size div_q2 = q2/submatrix_size_, rem_q2 = q2%submatrix_size_;

    for (cnt_size row = div_p1; row <= div_p2; ++row) {
      p1 = row == div_p1 ? rem_p1 : 0;
      p2 = row == div_p2 ? rem_p2 : submatrix_size_ - 1;
      for (cnt_size col = div_q1; col <= div_q2; ++col) {
        q1 = col == div_q1 ? rem_q1 : 0;
        q2 = col == div_q2 ? rem_q2 : submatrix_size_ - 1;

        const K2Tree &tree = subtrees_[row][col];
        tree.RangeQuery(p1, p2, q1, q2, [=] (cnt_size p, cnt_size q) {
          fun(row*submatrix_size_ + p, col*submatrix_size_ + q);
        });
      }
    }
  }

  /*
   * Returns the number of objects in the relation or matrix.
   */
  cnt_size cnt() const {
    return cnt_;
  }


  /* 
   * Get size in bytes.
   */
  size_t GetSize() const {
    size_t size = 0;
    for (uint i = 0; i < k0_; ++i)
      for (uint j = 0; j < k0_; ++j)
        size += subtrees_[i][j].GetSize();
    return size;
  }


  bool operator==(const base_partition &rhs) {
    if (k0_ != rhs.k0_ || cnt_ != rhs.cnt_ ||
        submatrix_size_ != rhs.submatrix_size_)
      return false;
    for (uint i = 0; i < k0_; ++i)
      for (uint j = 0; j < k0_; ++j)
        if (!(subtrees_[i][j] == rhs.subtrees_[i][j]))
          return false;
    return true;
  }

 protected:
  /* Returns the number of objects in the relation or matrix.*/
  cnt_size cnt_;
  /* Size of each submatrix represented in the subtrees.*/
  cnt_size submatrix_size_;
  /* Value of k for the firt level, ie, there are k0*k0 subtree.*/
  uint k0_;
  /* Matrix of subtrees.*/
  std::vector<std::vector<K2Tree>> subtrees_;

  explicit base_partition(std::ifstream *in)
      : cnt_(LoadValue<cnt_size>(in)),
        submatrix_size_(LoadValue<cnt_size>(in)),
        k0_(LoadValue<uint>(in)),
        subtrees_(k0_) {
  }

  void Save(std::ofstream *out) const {
    SaveValue(out, cnt_);
    SaveValue(out, submatrix_size_);
    SaveValue(out, k0_);
  }


};

}  // namespace libk2tree
#endif  // INCLUDE_BASE_BASE_PARTITION_H_
