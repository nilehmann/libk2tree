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

#include <fstream>

class K2TreePartition {
 public:
   using unsigned int = uint;
  /*
   * Loads a tree previously saved with a K2TreeParitionBuilder
   */
  PartitionK2Tree(ifstream *in);

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
   * @parm p Row in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each
   * object related to p.
   * The function expect a parameter of type _Size.
   */
  template<class Function>
  void DirectLinks(uint p, Function fun) const;

  /*
   * Iterates over all links in the given column.
   *
   * This member function effectively calls member InverseLinks of the
   * corresponding subtrees.
   *
   * @parm q Column in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each
   * object related to q.
   * The function expect a parameter of type _Size.
   */
  template<class Function>
  void InverseLinks(uint q, Function fun) const;

  /*
   * Iterates over all links in the specified submatrix.
   *
   * This member function effectively calls member RangeQuery of the
   * corresponding subtrees.
   *
   * @parm p1 Starting row in the matrix.
   * @parm p2 Ending row in the matrix.
   * @parm q1 Starting column in the matrix.
   * @parm q2 Ending column in the matrix.
   * @param fun Pointer to function, functor or lambda to be called for each pair o
   * objects. The function expect two parameters of type _Size
   */
  template<class Function>
  void RangeQuery(uint p1, uint p2, uint q1, uint q2, Function fun) const;
};


#endif  // INCLUDE_K2TREE_PARTITION_H_
