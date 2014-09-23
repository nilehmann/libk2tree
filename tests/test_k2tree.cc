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
#include <fstream>

#include "./queries.h"

using ::libk2tree::K2TreeBuilder;
using ::libk2tree::HybridK2Tree;
using ::std::shared_ptr;
using ::std::vector;
using ::std::ifstream;
using ::std::ofstream;


shared_ptr<HybridK2Tree> Build(uint k1, uint k2, uint kl, uint k1_levels,
                               vector<vector<bool>> *matrix, uint e = 0) {
  uint n = rand()%5000+1;
  K2TreeBuilder tb(n, k1, k2, kl, k1_levels);
  matrix->resize(n, vector<bool>(n, false));
  if (e == 0)
    e = (uint) rand()%(n*10) + 1;
  for (uint i = 0; i < e; ++i) {
    uint p = (uint) rand()%n;
    uint q = (uint) rand()%n;
    (*matrix)[p][q] = true;
    tb.AddLink(p, q);
  }
  return tb.Build();
}


void CheckLink(uint k1, uint k2, uint kl, uint k1_levels) {
  vector<vector<bool>> matrix;
  shared_ptr<HybridK2Tree > tree = Build(k1, k2, kl, k1_levels, &matrix);
  TestCheckLink(*tree, matrix);
}
void DirectLinks(uint k1, uint k2, uint kl, uint k1_levels) {
  vector<vector<bool> > matrix;
  shared_ptr<HybridK2Tree > tree = Build(k1, k2, kl, k1_levels, &matrix);
  TestDirectLinks(*tree, matrix);
}
void InverseLinks(uint k1, uint k2, uint kl, uint k1_levels) {
  vector<vector<bool> > matrix;
  shared_ptr<HybridK2Tree > tree = Build(k1, k2, kl, k1_levels, &matrix);
  TestInverseLinks(*tree, matrix);
}
void RangeQuery(uint k1, uint k2, uint kl, uint k1_levels) {
  vector<vector<bool> > matrix;
  shared_ptr<HybridK2Tree > tree = Build(k1, k2, kl, k1_levels, &matrix);
  TestRangeQuery(*tree, matrix);
}
void TestSave(uint k1, uint k2, uint kl, uint k1_levels) {
  uint n = rand()%100000+1;
  K2TreeBuilder tb(n, k1, k2, kl, k1_levels);
  uint e = n > 10 ? (uint) rand()%(n/10) + 1 : 1;
  for (uint i = 0; i < e; ++i) {
    uint p = (uint) rand()%n;
    uint q = (uint) rand()%n;
    tb.AddLink(p, q);
  }
  shared_ptr<HybridK2Tree > tree = tb.Build();

  ofstream out;
  out.open("k2tree_test", ofstream::out);
  tree->Save(&out);
  out.close();

  ifstream in;
  in.open("k2tree_test", ifstream::in);
  HybridK2Tree tree2(&in);
  in.close();
  ASSERT_TRUE(tree->operator==(tree2));
  remove("k2tree_test");
}

// CHECK Link
TEST(HybridK2Tree, CheckEdge1) {
  srand((uint) time(NULL));
  CheckLink(3, 2, 2, 1);
}
TEST(HybridK2Tree, CheckEdge2) {
  CheckLink(4, 2, 8, 5);
}
TEST(HybridK2Tree, CheckEdge3) {
  CheckLink(4, 2, 2, 10);
}

// DIRECT LINKS
TEST(HybridK2Tree, DirectLinks1) {
  DirectLinks(3, 2, 2, 1);
}
TEST(HybridK2Tree, DirectLinks2) {
  DirectLinks(4, 2, 8, 5);
}
TEST(HybridK2Tree, DirectLinks3) {
  DirectLinks(4, 2, 2, 10);
}

// INVERSE LINKS
TEST(HybridK2Tree, InverseLinks1) {
  srand((uint) time(NULL));
  InverseLinks(3, 2, 2, 1);
}
TEST(HybridK2Tree, InverseLinks2) {
  InverseLinks(4, 2, 8, 5);
}
TEST(HybridK2Tree, InverseLinks3) {
  InverseLinks(4, 2, 2, 10);
}

// RANGE QUERY
TEST(HybridK2Tree, RangeQuery1) {
  srand((uint) time(NULL));
  RangeQuery(3, 2, 2, 1);
}
TEST(HybridK2Tree, RangeQuery2) {
  RangeQuery(4, 2, 8, 5);
}
TEST(HybridK2Tree, RangeQuery3) {
  RangeQuery(4, 2, 2, 10);
}

// SAVE
TEST(HybridK2Tree, Save1) {
  srand((uint) time(NULL));
  TestSave(3, 2, 2, 1);
}
TEST(HybridK2Tree, Save2) {
  TestSave(4, 2, 8, 5);
}
TEST(HybridK2Tree, Save3) {
  TestSave(4, 2, 2, 10);
}

// EMPTY
TEST(HybridK2Tree, Empty) {
  vector<vector<bool>> matrix;
  shared_ptr<HybridK2Tree > tree = Build(3, 2, 2, 1, &matrix, 0);
  TestCheckLink(*tree, matrix);
  TestDirectLinks(*tree, matrix);
  TestInverseLinks(*tree, matrix);
  TestRangeQuery(*tree, matrix);
}
