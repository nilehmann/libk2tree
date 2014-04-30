/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2tree.h>
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <cstdio>
#include <algorithm>

using ::libk2tree::K2TreeBuilder;
using ::libk2tree::K2Tree;
using ::std::shared_ptr;
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
      if (matrix[i][j]) v.push_back(make_pair(i, j));
  return v;
}
vector<uint> GetPredecessors(const vector<vector<bool> > &matrix, uint q) {
  vector<uint> v;
  for (uint i = 0; i < matrix.size(); ++i)
    if (matrix[i][q]) v.push_back(i);
  return v;
}


void TestDirectIterator(int k1, int k2, int kl, int k1_levels) {
  int n = rand()%100000+1;
  K2TreeBuilder tb(n, k1, k2, kl, k1_levels);
  vector<vector<bool> > matrix(n, vector<bool>(n, false));

  int p = rand()%n;
  int e = n > 100 ? rand()%(n/100) + 1 : 1;
  for (int i = 0; i < e; ++i) {
    int q = rand()%n;
    int r = rand()%n;
    matrix[p][q] = true;
    tb.AddLink(p, q);
    // to make noise
    matrix[q][r] = true;
    tb.AddLink(q, r);
  }

  shared_ptr<K2Tree > tree = tb.Build();

  vector<uint> v = GetSuccessors(matrix, p);
  uint i = 0;
  tree->DirectLinks(p, [&] (unsigned int q) {
    ASSERT_EQ(v[i++], q);
  });
  ASSERT_EQ(v.size(), i);
}

void TestInverseIterator(int k1, int k2, int kl, int k1_levels) {
  int n = rand()%100000+1;
  K2TreeBuilder tb(n, k1, k2, kl, k1_levels);
  vector<vector<bool> > matrix(n, vector<bool>(n, false));

  int q = rand()%n;
  int e = n > 100 ? rand()%(n/100) + 1 : 1;
  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int r = rand()%n;

    matrix[p][q] = true;
    tb.AddLink(p, q);
    matrix[r][p] = true;
    tb.AddLink(r, p);
  }

  shared_ptr<K2Tree > tree = tb.Build();

  vector<uint> v = GetPredecessors(matrix, q);
  uint i = 0;
  tree->InverseLinks(q, [&] (unsigned int p) {
    ASSERT_EQ(v[i++], p);
  });
  ASSERT_EQ(v.size(), i);
}

void TestRangeIterator(int k1, int k2, int kl, int k1_levels) {
  int n = rand()%9999+2;
  K2TreeBuilder tb(n, k1, k2, kl, k1_levels);
  vector<vector<bool> > matrix(n, vector<bool>(n, false));

  int e = rand()%(n*100) + 1;
  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int q = rand()%n;
    matrix[p][q] = true;
    tb.AddLink(p, q);
  }

  shared_ptr<K2Tree > tree = tb.Build();

  int p1 = rand()%(n/2);
  int p2 = rand()%(n/2) + n/2;
  int q1 = 0;
  int q2 = n-1;


  vector<pair<uint, uint> > v = GetEdges(matrix, p1, p2, q1, q2);
  uint i = 0;
  vector<pair<uint, uint> > v1;
  tree->RangeQuery(p1, p2, q1, q2, [&] (unsigned int p, unsigned int q) {
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

TEST(DirectIterator, Iterate1) {
  srand(time(NULL));
  TestDirectIterator(3, 2, 2, 1);
}
TEST(DirectIterator, Iterate2) {
  TestDirectIterator(4, 2, 8, 5);
}
TEST(DirectIterator, Iterate3) {
  TestDirectIterator(4, 2, 2, 10);
}


TEST(InverseIterator, Iterate1) {
  srand(time(NULL));
  TestInverseIterator(3, 2, 2, 1);
}
TEST(InverseIterator, Iterate2) {
  TestInverseIterator(4, 2, 8, 5);
}
TEST(InverseIterator, Iterate3) {
  TestInverseIterator(4, 2, 2, 10);
}


TEST(RangeIterator, Iterate1) {
  srand(time(NULL));
  TestRangeIterator(3, 2, 2, 1);
}
TEST(RangeIterator, Iterate2) {
  TestRangeIterator(4, 2, 8, 5);
}
TEST(RangeIterator, Iterate3) {
  TestRangeIterator(4, 2, 2, 10);
}
