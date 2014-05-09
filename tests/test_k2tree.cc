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


shared_ptr<HybridK2Tree> Build(int k1, int k2, int kl, int k1_levels,
                               vector<vector<bool>> *matrix) {
  int n = rand()%5000+1;
  K2TreeBuilder tb(n, k1, k2, kl, k1_levels);
  matrix->resize(n, vector<bool>(n, false));
  int e = rand()%(n*10) + 1;
  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int q = rand()%n;
    (*matrix)[p][q] = true;
    tb.AddLink(p, q);
  }
  return tb.Build();
}

void CheckLink(int k1, int k2, int kl, int k1_levels) {
  vector<vector<bool>> matrix;
  shared_ptr<HybridK2Tree > tree = Build(k1, k2, kl, k1_levels, &matrix);
  TestCheckLink(*tree, matrix);
}
void DirectLinks(int k1, int k2, int kl, int k1_levels) {
  vector<vector<bool> > matrix;
  shared_ptr<HybridK2Tree > tree = Build(k1, k2, kl, k1_levels, &matrix);
  TestDirectLinks(*tree, matrix);
}
void InverseLinks(int k1, int k2, int kl, int k1_levels) {
  vector<vector<bool> > matrix;
  shared_ptr<HybridK2Tree > tree = Build(k1, k2, kl, k1_levels, &matrix);
  TestInverseLinks(*tree, matrix);
}
void RangeQuery(int k1, int k2, int kl, int k1_levels) {
  vector<vector<bool> > matrix;
  shared_ptr<HybridK2Tree > tree = Build(k1, k2, kl, k1_levels, &matrix);
  TestRangeQuery(*tree, matrix);
}
void TestSave(int k1, int k2, int kl, int k1_levels) {
  int n = rand()%100000+1;
  K2TreeBuilder tb(n, k1, k2, kl, k1_levels);
  int e = n > 10 ? rand()%(n/10) + 1 : 1;
  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int q = rand()%n;
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
  srand(time(NULL));
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
  srand(time(NULL));
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
  srand(time(NULL));
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
  srand(time(NULL));
  TestSave(3, 2, 2, 1);
}
TEST(HybridK2Tree, Save2) {
  TestSave(4, 2, 8, 5);
}
TEST(HybridK2Tree, Save3) {
  TestSave(4, 2, 2, 10);
}
