/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_K2TREE_PARTITION_H_
#define INCLUDE_K2TREE_PARTITION_H_

#include <libk2tree_basic.h>
#include <hybrid_k2tree.h>
#include <fstream>
#include <vector>

namespace libk2tree {

class K2TreePartition {
 public:
  typedef uint uint;
  typedef size_t words_cnt;
  typedef size_t obj_cnt;

  /*
   * Loads a tree previously saved with a K2TreeParitionBuilder
   */
  explicit K2TreePartition(ifstream *in);

  /* Checks if exist a link from object p to q.
   *
   * This member function effectively calls member CheckLink of the
   * corresponding subtree.
   *
   * @param p Identifier of first object.
   * @param q Identifier of second object.
   */
  bool CheckLink(uint p, uint q) const;

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
      const HybridK2Tree &tree = subtrees_[row][col];
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
      const HybridK2Tree &tree = subtrees_[row][col];
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

        const HybridK2Tree &tree = subtrees_[row][col];
        tree.RangeQuery(p1, p2, q1, q2, [=] (uint p, uint q) {
          fun(row*submatrix_size_ + p, col*submatrix_size_ + q);
        });
      }
    }
  }

  /*
   * Returns the number of objects in the relation or matrix.
   */
  obj_cnt cnt() const {
    return cnt_;
  }


  /* 
   * Get size in bytes.
   */
  size_t GetSize() const;

  /*
   * Return number of words of size kl*kl in the leaf level.
   */
  /*words_cnt WordsCnt() const {
    size_t leaves = 0;
    for (int row = 0; row < k0_; ++row)
      for (int col = 0; col < k0_; ++col)
        leaves += subtrees_[row][col].WordsCnt();
    return leaves;
  }*/

  /*
   * Return the size in bytes of the words in the leaf level, ie, kl*kl/8.
   */
  /*int WordSize() const {
    //return subtrees_[0][0].WordSize();
  }*/

  /*
   * Iterates over the words in the leaf level.
   * @param fun Pointer to function, functor or lambda expecting a uchar *.
   */
  /*template<class Function>
  void Words(Function fun) const {
    for (int row = 0; row < k0_; ++row)
      for (int col = 0; col < k0_; ++col)
        subtrees_[row][col].Words(fun);
  }*/

 private:
  // Returns the number of objects in the relation or matrix.
  uint cnt_;
  // Size of each submatrix represented in the subtrees.
  uint submatrix_size_;
  // Value of k for the firt level, ie, there are k0*k0 subtree.
  int k0_;
  // Matrix of subtrees.
  vector<vector<HybridK2Tree>> subtrees_;
};


}  // namespace libk2tree
#endif  // INCLUDE_K2TREE_PARTITION_H_
