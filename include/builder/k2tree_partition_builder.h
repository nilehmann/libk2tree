/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_BUILDER_K2TREE_PARTITION_BUILDER_H_
#define INCLUDE_BUILDER_K2TREE_PARTITION_BUILDER_H_

#include <libk2tree_basic.h>
#include <boost/filesystem.hpp>
#include <builder/k2tree_builder.h>
#include <fstream>

namespace libk2tree {
using std::shared_ptr;
using boost::filesystem::path;

/**
 * Implements a builder for the approach of section 5.2, partitioning the first
 * level and constructing a separate <em>k<sup>2</sup></em>tree for each
 * submatrix. Submatrices are stored on disk after being built. If we consider
 * the representation as a matrix of submatrices, then they have to be built in
 * row-wise order.
 */
class K2TreePartitionBuilder {
 public:
  /**
   * Creates a builder partitioning the matrix in submatrices of the given size.
   * Independently builds a tree with an hybrid approach for each submatrix.
   * The partial construction of each subtree is maintained in a temporary file.
   * After all subtrees have been built, the tree is saved in the given file and
   * can be loaded.
   *
   * @param cnt Number of objects in the relation or matrix.
   * @param submatrix_size Size of submatrices.
   * @param k1 Aritiy of the first levels in the subtrees.
   * @param k2 Arity of the second part in the subtrees.
   * @param kl Arity of the level height-1.
   * @param k1_levels Number of levels with arity k1.
   * @param file Name of the file to store the structure.
   *
   * @see K2TreePartition::K2TreePartition
   */
  K2TreePartitionBuilder(uint cnt, uint submatrix_size,
                         int k1, int k2, int kl, int k1_levels,
                         const path &file);

  /**
   * Creates a link from object p to q, assuming it correspond to the current
   * submatrix beeing built. Attempting to add a link that doesn't belong to the
   * current submatrix or after all submatrices have been built causes an
   * undefined behavior.
   *
   * @param p Identifier of the first object.
   * @param q Identifier of the second object.
   */
  void AddLink(uint p, uint q);

  /**
   * Builds a k2tree for the current submatrix and moves to the next submatrix.
   * A call to this function after all submatrices have been built causes
   * an undefined behavior.
   */
  void BuildSubtree();

  /**
   * Checks if all submatrices have been built.
   *
   * @return True if all submatrices have been built, false otherwise.
   */
  inline bool Ready() const {
    return ready_;
  }

  /**
   * Returns the number of objects in the relation or matrix.
   *
   * @return Number of objects.
   */
  inline bool cnt() const {
    return cnt_;
  }

  /**
   * Returns the row of the current submatrix beeing built in the row-wise
   * traversal.
   *
   * @return Number of row of the current submatrix.
   */
  inline int row() const {
    return row_;
  }

  /**
   * Returns the column of the current submatrix beeing build in the row-wise
   * traversal.
   *
   * @return Number of column of the current submatrix.
   */
  inline int col() const {
    return col_;
  }

  /**
   * Returns the number of division.
   *
   * @return Arity of the first level.
   */
  inline int k0() const {
    return k0_;
  }

  ~K2TreePartitionBuilder();

 private:
  /** Number of objects in the relation or matrix. */
  uint cnt_;
  /** Size of each submatrix. */
  uint submatrix_size_;
  /** Value of k for the first level, ie, there are k0*k0 submatrices. */
  int k0_;
  /** Row of the current submatrix in the matrix of submatrices. */
  int row_;
  /** Col of the current submatrix in the matrix of submatrices. */
  int col_;
  /** Whether all submatrices have been built or not */
  bool ready_;
  /** Builder for the current submatrix. */
  K2TreeBuilder builder_;
  /** Name of the temporary file. */
  path tmp_;
  /** Name of the resulting file. */
  path file_;
  /**
   * File stream to save the partial submatrices. Points to temporary file until
   * all the submatrices have been built.
   */
  ofstream out_;
};
}  // namespace libk2tree

#endif  // INCLUDE_BUILDER_K2TREE_PARTITION_BUILDER_H_

