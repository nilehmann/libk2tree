/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <gtest/gtest.h>
#include <k2treebuilder.h>


using ::k2tree_impl::K2TreeBuilder;

/*
 * 0 1 0 0 | 0 0 0 0 | 0 0 0
 * 0 0 1 1 | 1 0 0 0 | 0 0 0
 * 0 0 0 0 | 0 0 0 0 | 0 0 0
 * 0 0 0 0 | 0 0 0 0 | 0 0 0
 * -------------------------
 * 0 0 0 0 | 0 0 0 0 | 0 0 0
 * 0 0 0 0 | 0 0 0 0 | 0 0 0
 * 0 0 0 0 | 0 0 0 0 | 0 0 0
 * 0 0 0 0 | 0 0 1 0 | 0 0 0
 * -------------------------
 * 0 0 0 0 | 0 0 1 0 | 0 1 0
 * 0 0 0 0 | 0 0 1 0 | 1 0 1
 * 0 0 0 0 | 0 0 1 0 | 0 1 0
 */


void InsertEdges(K2TreeBuilder *tb) {
  tb->InsertEdge(0, 1);
  tb->InsertEdge(1, 2);
  tb->InsertEdge(1, 3);
  tb->InsertEdge(1, 4);
  tb->InsertEdge(7, 6);
  tb->InsertEdge(8, 6);
  tb->InsertEdge(8, 9);
  tb->InsertEdge(9, 6);
  tb->InsertEdge(9, 8);
  tb->InsertEdge(9, 10);
  tb->InsertEdge(10, 6);
  tb->InsertEdge(10, 9);
}

TEST(K2TreeBuilder, 1) {
  K2TreeBuilder tb(11, 4, 2, 2, 1);
  InsertEdges(&tb);
  ASSERT_EQ(0, tb.nodes_k1());
  ASSERT_EQ(16, tb.nodes_k2());
  ASSERT_EQ(20, tb.nodes_kl());
  ASSERT_EQ(36, tb.leafs());
  ASSERT_EQ(12, tb.edges());
  ASSERT_EQ(3, tb.height());
}
TEST(K2TreeBuilder, 2) {
  K2TreeBuilder tb(11, 3, 2, 2, 1);
  InsertEdges(&tb);
  ASSERT_EQ(0, tb.nodes_k1());
  ASSERT_EQ(9, tb.nodes_k2());
  ASSERT_EQ(20, tb.nodes_kl());
  ASSERT_EQ(36, tb.leafs());
  ASSERT_EQ(12, tb.edges());
  ASSERT_EQ(3, tb.height());
}
TEST(K2TreeBuilder, 3) {
  K2TreeBuilder tb(11, 3, 2, 3, 1);
  InsertEdges(&tb);
  ASSERT_EQ(0, tb.nodes_k1());
  ASSERT_EQ(9, tb.nodes_k2());
  ASSERT_EQ(8, tb.nodes_kl());
  ASSERT_EQ(54, tb.leafs());
  ASSERT_EQ(12, tb.edges());
  ASSERT_EQ(3, tb.height());
}
TEST(K2TreeBuilder, 4) {
  K2TreeBuilder tb(11, 3, 2, 2, 2);
  InsertEdges(&tb);
  ASSERT_EQ(9, tb.nodes_k1());
  ASSERT_EQ(0, tb.nodes_k2());
  ASSERT_EQ(18, tb.nodes_kl());
  ASSERT_EQ(36, tb.leafs());
  ASSERT_EQ(12, tb.edges());
  ASSERT_EQ(3, tb.height());
}
TEST(K2TreeBuilder, 5) {
  K2TreeBuilder tb(11, 3, 2, 2, 3);
  InsertEdges(&tb);
  ASSERT_EQ(18, tb.nodes_k1());
  ASSERT_EQ(0, tb.nodes_k2());
  ASSERT_EQ(18, tb.nodes_kl());
  ASSERT_EQ(36, tb.leafs());
  ASSERT_EQ(12, tb.edges());
  ASSERT_EQ(4, tb.height());
}
