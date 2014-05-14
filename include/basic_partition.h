/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_BASIC_PARTITION_H_
#define INCLUDE_BASIC_PARTITION_H_

#include <libk2tree_basic.h>
#include <utils/utils.h>
#include <fstream>
#include <vector>

namespace libk2tree {
using std::vector;
using utils::LoadValue;
using utils::SaveValue;

template<class K2Tree>
class basic_partition {
 public:
  /*
   * Loads a tree previously saved with a K2TreePartitionBuilder
   */
  explicit basic_partition(std::ifstream *in)
      : cnt_(LoadValue<uint>(in)),
        submatrix_size_(LoadValue<uint>(in)),
        k0_(LoadValue<int>(in)),
        subtrees_(k0_) {}


  /* Checks if exist a link from object p to q.
   *
   * This member function effectively calls member CheckLink of the
   * corresponding subtree.
   *
   * @param p Identifier of first object.
   * @param q Identifier of second object.
   */
  bool CheckLink(uint p, uint q) const {
    const K2Tree &t = subtrees_[p/submatrix_size_][q/submatrix_size_];
    return t.CheckLink(p % submatrix_size_, q % submatrix_size_);
  }

  /*
   * Iterates over all links in the given row.
   * 
   * This member function effectively calls member DirectLinks of the
   * corresponding subtrees.
   *
   * @param p Row in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each
   * object related to p.
   * The function expect a parameter of type uint.
   */
  template<class Function>
  void DirectLinks(uint p, Function fun) const {
    uint row = p/submatrix_size_;
    for (int col = 0; col < k0_; ++col) {
      const K2Tree &tree = subtrees_[row][col];
      tree.DirectLinks(p % submatrix_size_, [=] (uint q) {
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
   * The function expect a parameter of type uint.
   */
  template<class Function>
  void InverseLinks(uint q, Function fun) const {
    uint col = q/submatrix_size_;
    for (int row = 0; row < k0_; ++row) {
      const K2Tree &tree = subtrees_[row][col];
      tree.InverseLinks(q % submatrix_size_, [=] (uint p) {
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
   * of objects. The function expect two parameters of type uint.
   */
  template<class Function>
  void RangeQuery(uint p1, uint p2, uint q1, uint q2, Function fun) const {
    uint div_p1 = p1/submatrix_size_, rem_p1 = p1%submatrix_size_;
    uint div_p2 = p2/submatrix_size_, rem_p2 = p2%submatrix_size_;
    uint div_q1 = q1/submatrix_size_, rem_q1 = q1%submatrix_size_;
    uint div_q2 = q2/submatrix_size_, rem_q2 = q2%submatrix_size_;

    for (uint row = div_p1; row <= div_p2; ++row) {
      p1 = row == div_p1 ? rem_p1 : 0;
      p2 = row == div_p2 ? rem_p2 : submatrix_size_ - 1;
      for (uint col = div_q1; col <= div_q2; ++col) {
        q1 = col == div_q1 ? rem_q1 : 0;
        q2 = col == div_q2 ? rem_q2 : submatrix_size_ - 1;

        const K2Tree &tree = subtrees_[row][col];
        tree.RangeQuery(p1, p2, q1, q2, [=] (uint p, uint q) {
          fun(row*submatrix_size_ + p, col*submatrix_size_ + q);
        });
      }
    }
  }

  /*
   * Returns the number of objects in the relation or matrix.
   */
  uint cnt() const {
    return cnt_;
  }


  /* 
   * Get size in bytes.
   */
  size_t GetSize() const {
    size_t size = 0;
    for (int i = 0; i < k0_; ++i)
      for (int j = 0; j < k0_; ++j)
        size += subtrees_[i][j].GetSize();
    return size;
  }


  bool operator==(const basic_partition &rhs) {
    if (k0_ != rhs.k0_ || cnt_ != rhs.cnt_ ||
        submatrix_size_ != rhs.submatrix_size_)
      return false;
    for (int i = 0; i < k0_; ++i)
      for (int j = 0; j < k0_; ++j)
        if (!(subtrees_[i][j] == rhs.subtrees_[i][j]))
          return false;
    return true;
  }

 protected:
  // Returns the number of objects in the relation or matrix.
  uint cnt_;
  // Size of each submatrix represented in the subtrees.
  uint submatrix_size_;
  // Value of k for the firt level, ie, there are k0*k0 subtree.
  int k0_;
  // Matrix of subtrees.
  vector<vector<K2Tree>> subtrees_;
};


}  // namespace libk2tree
#endif  // INCLUDE_BASIC_PARTITION_H_
