/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */


#include "./queries.h"
#include <k2tree.h>
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <cstdio>
#include <algorithm>
using ::libk2tree::K2TreeBuilder;
using ::libk2tree::HybridK2Tree;
using ::libk2tree::CompressedHybrid;
using ::std::shared_ptr;
using ::std::vector;
using ::std::pair;

shared_ptr<CompressedHybrid> Build(vector<vector<bool>> *matrix) {
  int n = rand()%50000+1;
  K2TreeBuilder tb(n, 4, 2, 8, 4);
  matrix->resize(n, vector<bool>(n, false));

  int e = n > 100 ? rand()%(n/100) + 1 : 1;
  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int q = rand()%n;
    (*matrix)[p][q] = true;
    tb.AddLink(p, q);
  }
  shared_ptr<HybridK2Tree > tree = tb.Build();
  return tree->CompressLeaves();
}

TEST(CompressedHybrid, DirectLinks) {
  time_t t = time(NULL);
  fprintf(stderr, "TIME: %ld", t);
  srand(t);
  vector<vector<bool>> matrix;
  shared_ptr<CompressedHybrid> tree = Build(&matrix);

  TestDirectLinks(*tree, matrix);
}

TEST(CompressedHybrid, InverseLinks) {
  vector<vector<bool>> matrix;
  shared_ptr<CompressedHybrid> tree = Build(&matrix);

  TestInverseLinks(*tree, matrix);
}
TEST(CompressedHybrid, CheckLink) {
  vector<vector<bool>> matrix;
  shared_ptr<CompressedHybrid> tree = Build(&matrix);

  TestCheckLink(*tree, matrix);
}
TEST(CompressedHybrid, RangeQuery) {
  vector<vector<bool>> matrix;
  shared_ptr<CompressedHybrid> tree = Build(&matrix);

  TestRangeQuery(*tree, matrix);
}
