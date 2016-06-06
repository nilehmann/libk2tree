/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */


#ifndef TESTS_QUERIES_CC_
#define TESTS_QUERIES_CC_

#include "./queries.h"
#include <vector>

using ::std::vector;
using ::std::pair;

vector<uint> GetSuccessors(const vector<vector<bool> > &matrix, uint p) {
  vector<uint> v;
  for (uint i = 0; i < matrix[p].size(); ++i)
    if (matrix[p][i]) v.push_back(i);
  return v;
}
vector<pair<uint, uint> > GetEdges(const vector<vector<bool> > &matrix,
                                   uint p1, uint p2, uint q1, uint q2) {
  vector<pair<uint, uint> > v;
  for (uint i = p1; i <= p2; ++i)
    for (uint j = q1; j <= q2; ++j)
      if (matrix[i][j]) v.push_back(std::make_pair(i, j));
  return v;
}
vector<uint> GetPredecessors(const vector<vector<bool> > &matrix, uint q) {
  vector<uint> v;
  for (uint i = 0; i < matrix.size(); ++i)
    if (matrix[i][q]) v.push_back(i);
  return v;
}

#endif // TESTS_QUERIES_CC_
