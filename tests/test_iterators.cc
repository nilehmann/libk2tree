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

using ::k2tree_impl::K2TreeBuilder;
using ::k2tree_impl::K2Tree;
using ::std::shared_ptr;
using ::std::vector;


vector<size_t> GetSuccessors(const vector<vector<bool> > &matrix, size_t p) {
  vector<size_t> v;
  for (size_t i = 0; i < matrix[p].size(); ++i)
    if (matrix[p][i]) v.push_back(i);
  return v;
}


void TestIterator(int k1, int k2, int kl, int k1_levels) {
  int n = rand()%100000+1;
  K2TreeBuilder tb(n, k1, k2, kl, k1_levels);
  vector<vector<bool> > matrix(n, vector<bool>(n, false));

  int p = rand()%n;
  int e = rand()%(n/100) + 1;
  for (int i = 0; i < e; ++i) {
    int q = rand()%n;
    int p2 = rand()%n;
    matrix[p][q] = true;
    tb.AddLink(p, q);
    tb.AddLink(q, p2);
  }

  shared_ptr<K2Tree> tree = tb.Build();

  vector<size_t> v = GetSuccessors(matrix, p);
  K2Tree::DirectIterator q = tree->DirectBegin(p);
  size_t i;
  for (i = 0; q != tree->DirectEnd(p); ++q, ++i)
    ASSERT_EQ(v[i], *q);
  ASSERT_EQ(v.size(), i);
}

TEST(DirectIterator, Iterate1) {
  srand(time(NULL));
  TestIterator(3, 2, 2, 1);
}
TEST(DirectIterator, Iterate2) {
  TestIterator(4, 2, 8, 5);
}
TEST(DirectIterator, Iterate3) {
  TestIterator(4, 2, 2, 10);
}
