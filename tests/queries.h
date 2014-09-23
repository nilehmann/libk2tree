/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef TESTS_QUERIES_H_
#define TESTS_QUERIES_H_

#include <k2tree.h>
#include <libk2tree_basic.h>
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <utility>

using ::libk2tree::K2TreeBuilder;
using ::libk2tree::cnt_size;
using ::std::shared_ptr;
using ::std::vector;
using ::std::pair;


vector<uint> GetSuccessors(const vector<vector<bool> > &matrix, uint p);

vector<pair<uint, uint>> GetEdges(const vector<vector<bool> > &matrix,
                                  uint p1, uint p2, uint q1, uint q2);

vector<uint> GetPredecessors(const vector<vector<bool> > &matrix, uint q);



template<class K2Tree>
void TestDirectLinks(const K2Tree &tree, const vector<vector<bool> > &matrix) {
  uint n = (uint) matrix.size();
  uint e = n > 10 ? (uint) rand()%(n/4) + 1 : n;
  for (uint c = 0; c < e; ++c) {
    uint p = (uint) rand()%n;
    vector<uint> v = GetSuccessors(matrix, p);
    uint i = 0;
    tree.DirectLinks(p, [&] (cnt_size q) {
      ASSERT_EQ(v[i++], q);
    });
    ASSERT_EQ(v.size(), i);
  }
}

template<class K2Tree>
void TestInverseLinks(const K2Tree &tree, const vector<vector<bool>> &matrix) {
  uint n = (uint) matrix.size();
  uint e = n > 10 ? (uint) rand()%(n/4) + 1 : n;
  for (uint c = 0; c < e; ++c) {
    uint q = (uint) rand()%n;
    vector<uint> v = GetPredecessors(matrix, q);
    uint i = 0;
    tree.InverseLinks(q, [&] (cnt_size p) {
      ASSERT_EQ(v[i++], p);
    });
    ASSERT_EQ(v.size(), i);
  }
}

template<class K2Tree>
void TestRangeQuery(const K2Tree &tree, const vector<vector<bool>> &matrix) {
  uint n = (uint) matrix.size();
  uint p1 = (uint) rand()%(n/2);
  uint p2 = (uint) rand()%(n/2) + n/2;
  uint q1 = (uint) rand()%(n/2);
  uint q2 = (uint) rand()%(n/2) + n/2;

  vector<pair<uint, uint> > v = GetEdges(matrix, p1, p2, q1, q2);
  uint i = 0;
  vector<pair<uint, uint> > v1;
  tree.RangeQuery(p1, p2, q1, q2, [&] (cnt_size p, cnt_size q) {
    i++;
    ASSERT_TRUE(matrix[p][q]);
    v1.emplace_back(p, q);
  });
  ASSERT_EQ(v.size(), i);

  /* Test if there are repeated elements */
  uint size = (uint) v1.size();
  sort(v1.begin(), v1.end());
  unique(v1.begin(), v1.end());
  ASSERT_EQ(size, v1.size());
}
template<class K2Tree>
void TestCheckLink(const K2Tree &tree, const vector<vector<bool>> &matrix) {
  uint n = (uint) matrix.size();
  uint e = n > 10 ? (uint) rand()%(n*10) + 1 : n;

  for (uint i = 0; i < e; ++i) {
    uint p = (uint) rand()%n;
    uint q = (uint) rand()%n;
    ASSERT_EQ(matrix[p][q], tree.CheckLink(p, q));
  }
}


#endif  // TESTS_QUERIES_H_
