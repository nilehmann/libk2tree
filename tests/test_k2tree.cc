/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2treebuilder.h>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

using ::k2tree_impl::K2TreeBuilder;
using ::k2tree_impl::K2Tree;
using ::std::shared_ptr;
using ::std::vector;




void TestCheckLink(int k1, int k2, int kl, int k1_levels) {
  srand(time(NULL));
  int n = rand()%100000+1;
  K2TreeBuilder tb(n, k1, k2, kl, k1_levels);
  vector<vector<bool> > matrix(n, vector<bool>(n, false));
  int e = rand()%(n/10) + 1;
  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int q = rand()%n;
    matrix[p][q] = true;
    tb.AddLink(p, q);
  }
  shared_ptr<K2Tree> tree = tb.Build();

  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int q = rand()%n;
    ASSERT_EQ(matrix[p][q], tree->CheckLink(p, q));
  }
}
TEST(K2Tree, CheckEdge1) {
  TestCheckLink(3, 2, 2, 1);
}
TEST(K2Tree, CheckEdge2) {
  TestCheckLink(4, 2, 8, 5);
}
TEST(K2Tree, CheckEdge3) {
  TestCheckLink(4, 2, 2, 10);
}
