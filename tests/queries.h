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
using ::std::shared_ptr;
using ::std::vector;
using ::std::pair;


vector<uint> GetSuccessors(const vector<vector<bool> > &matrix, uint p);

vector<pair<uint, uint>> GetEdges(const vector<vector<bool> > &matrix,
                                  uint p1, uint p2, uint q1, uint q2);

vector<uint> GetPredecessors(const vector<vector<bool> > &matrix, uint q);



template<class K2Tree>
void TestDirectLinks(const K2Tree &tree, const vector<vector<bool> > &matrix) {
  int n = matrix.size();
  int e = n > 10 ? rand()%(n/4) + 1 : n;
  for (int c = 0; c < e; ++c) {
    int p = rand()%n;
    vector<uint> v = GetSuccessors(matrix, p);
    uint i = 0;
    tree.DirectLinks(p, [&] (unsigned int q) {
      ASSERT_EQ(v[i++], q);
    });
    ASSERT_EQ(v.size(), i);
  }
}

template<class K2Tree>
void TestInverseLinks(const K2Tree &tree, const vector<vector<bool>> &matrix) {
  int n = matrix.size();
  int e = n > 10 ? rand()%(n/4) + 1 : n;
  for (int c = 0; c < e; ++c) {
    int q = rand()%n;
    vector<uint> v = GetPredecessors(matrix, q);
    uint i = 0;
    tree.InverseLinks(q, [&] (unsigned int p) {
      ASSERT_EQ(v[i++], p);
    });
    ASSERT_EQ(v.size(), i);
  }
}

template<class K2Tree>
void TestRangeQuery(const K2Tree &tree, const vector<vector<bool>> &matrix) {
  int n = matrix.size();
  int p1 = rand()%(n/2);
  int p2 = rand()%(n/2) + n/2;
  int q1 = rand()%(n/2);
  int q2 = rand()%(n/2) + n/2;

  vector<pair<uint, uint> > v = GetEdges(matrix, p1, p2, q1, q2);
  uint i = 0;
  vector<pair<uint, uint> > v1;
  tree.RangeQuery(p1, p2, q1, q2, [&] (unsigned int p, unsigned int q) {
    i++;
    ASSERT_TRUE(matrix[p][q]);
    v1.emplace_back(p, q);
  });
  ASSERT_EQ(v.size(), i);

  /* Test if there are repeated elements */
  int size = v1.size();
  sort(v1.begin(), v1.end());
  unique(v1.begin(), v1.end());
  ASSERT_EQ(size, v1.size());
}
template<class K2Tree>
void TestCheckLink(const K2Tree &tree, const vector<vector<bool>> &matrix) {
  int n = matrix.size();
  int e = n > 10 ? rand()%(n*10) + 1 : n;

  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int q = rand()%n;
    ASSERT_EQ(matrix[p][q], tree.CheckLink(p, q));
  }
}


#endif  // TESTS_QUERIES_H_
