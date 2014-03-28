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

using ::k2tree_impl::K2TreeBuilder;
using ::k2tree_impl::K2Tree;
using ::std::shared_ptr;

shared_ptr<K2Tree> BuildTree(size_t nodes, int k1, int k2, int kl, int k1_levels) {
  K2TreeBuilder tb(nodes, k1, k2, kl, k1_levels);
  tb.InsertEdge(0, 1);
  tb.InsertEdge(1, 2);
  tb.InsertEdge(1, 3);
  tb.InsertEdge(1, 4);
  tb.InsertEdge(7, 6);
  tb.InsertEdge(8, 6);
  tb.InsertEdge(8, 9);
  tb.InsertEdge(9, 6);
  tb.InsertEdge(9, 8);
  tb.InsertEdge(9, 10);
  tb.InsertEdge(10, 6);
  tb.InsertEdge(10, 9);
  return tb.Build();
}
TEST(K2Tree, CheckEdge1) {
  std::shared_ptr<K2Tree> tree = BuildTree(11, 3, 2, 2, 1);
  
  ASSERT_EQ(true,tree->CheckEdge(7,6));
}
