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


void AddLinks(K2TreeBuilder *tb) {
  tb->AddLink(0, 1);
  tb->AddLink(1, 2);
  tb->AddLink(1, 3);
  tb->AddLink(1, 4);
  tb->AddLink(7, 6);
  tb->AddLink(8, 6);
  tb->AddLink(8, 9);
  tb->AddLink(9, 6);
  tb->AddLink(9, 8);
  tb->AddLink(9, 10);
  tb->AddLink(10, 6);
  tb->AddLink(10, 9);
}

TEST(K2TreeBuilder, 1) {
  K2TreeBuilder tb(11, 4, 2, 2, 1);
  AddLinks(&tb);
  ASSERT_EQ(36, tb.internal_nodes());
  ASSERT_EQ(36, tb.leafs());
  ASSERT_EQ(12, tb.edges());
  ASSERT_EQ(3, tb.height());
}
TEST(K2TreeBuilder, 2) {
  K2TreeBuilder tb(11, 3, 2, 2, 1);
  AddLinks(&tb);
  ASSERT_EQ(29, tb.internal_nodes());
  ASSERT_EQ(36, tb.leafs());
  ASSERT_EQ(12, tb.edges());
  ASSERT_EQ(3, tb.height());
}
TEST(K2TreeBuilder, 3) {
  K2TreeBuilder tb(11, 3, 2, 3, 1);
  AddLinks(&tb);
  ASSERT_EQ(17, tb.internal_nodes());
  ASSERT_EQ(54, tb.leafs());
  ASSERT_EQ(12, tb.edges());
  ASSERT_EQ(3, tb.height());
}
TEST(K2TreeBuilder, 4) {
  K2TreeBuilder tb(11, 3, 2, 2, 2);
  AddLinks(&tb);
  ASSERT_EQ(27, tb.internal_nodes());
  ASSERT_EQ(36, tb.leafs());
  ASSERT_EQ(12, tb.edges());
  ASSERT_EQ(3, tb.height());
}
TEST(K2TreeBuilder, 5) {
  K2TreeBuilder tb(11, 3, 2, 2, 3);
  AddLinks(&tb);
  ASSERT_EQ(36, tb.internal_nodes());
  ASSERT_EQ(36, tb.leafs());
  ASSERT_EQ(12, tb.edges());
  ASSERT_EQ(4, tb.height());
}
