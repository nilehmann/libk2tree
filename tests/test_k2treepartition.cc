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
#include <utils/utils.h>
#include <boost/filesystem.hpp>
#include <cstdio>
#include <string>
#include <memory>
#include "./queries.h"

using ::libk2tree::K2TreePartitionBuilder;
using ::libk2tree::utils::Ceil;
using ::libk2tree::K2TreePartition;
using ::boost::filesystem::remove;
using ::std::shared_ptr;

typedef unsigned int uint;


shared_ptr<K2TreePartition> BuildPartition(vector<vector<bool>> *matrix) {
  string filename = "partition_test";
  int n = 1000;

  int e = rand()%(n*10) + 1;

  matrix->resize(n, vector<bool>(n, false));
  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int q = rand()%n;
    (*matrix)[p][q] = true;
  }

  int k0 = 10;
  int subm = n/k0;


  K2TreePartitionBuilder b(n, subm, 4, 2, 2, 3, filename);
  for (int row = 0; row < k0; ++row) {
    for (int col = 0; col < k0; ++col) {
      for (int i = 0; i < subm; ++i) {
        for (int j = 0; j < subm; ++j) {
          if ((*matrix)[i + row*subm][j + col*subm])
            b.AddLink(i + row*subm, j + col*subm);
        }
      }
      b.BuildSubtree();
    }
  }
  ifstream in(filename, ifstream::in);
  shared_ptr<K2TreePartition> t(new K2TreePartition(&in));
  in.close();
  remove(filename);
  return t;
}

TEST(k2treepartition, CheckLink) {
  srand(time(NULL));
  vector<vector<bool>> matrix;
  shared_ptr<K2TreePartition> tree = BuildPartition(&matrix);

  TestCheckLink(*tree, matrix);
}

TEST(k2treepartition, DirectLinks) {
  vector<vector<bool>> matrix;
  shared_ptr<K2TreePartition> tree = BuildPartition(&matrix);

  TestDirectLinks(*tree, matrix);
}
TEST(k2treepartition, InverseLinks) {
  vector<vector<bool>> matrix;
  shared_ptr<K2TreePartition> tree = BuildPartition(&matrix);

  TestInverseLinks(*tree, matrix);
}
TEST(k2treepartition, RangeQuery) {
  vector<vector<bool>> matrix;
  shared_ptr<K2TreePartition> tree = BuildPartition(&matrix);

  TestRangeQuery(*tree, matrix);
}

TEST(k2treepartition, Save) {
  vector<vector<bool>> matrix;
  shared_ptr<K2TreePartition> tree = BuildPartition(&matrix);

  ofstream out;
  out.open("partition_save", ofstream::out);
  tree->Save(&out);
  out.close();

  ifstream in;
  in.open("partition_save", ifstream::in);
  K2TreePartition tree2(&in);
  in.close();
  ASSERT_TRUE(*tree == tree2);
}
