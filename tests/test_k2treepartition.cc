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
#include "./queries.h"

using ::libk2tree::K2TreePartitionBuilder;
using ::libk2tree::utils::Ceil;
using ::libk2tree::K2TreePartition;
using ::boost::filesystem::remove;

typedef unsigned int uint;


void Build(string filename, vector<vector<bool>> *matrix) {
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
}

TEST(k2treepartition, CheckLink) {
  srand(time(NULL));
  string filename("partition_test_check_link");
  vector<vector<bool>> matrix;
  Build(filename, &matrix);

  ifstream in(filename, ifstream::in);
  K2TreePartition tree(&in);

  TestCheckLink(tree, matrix);
  remove(filename);
}

TEST(k2treepartition, DirectLinks) {
  string filename("partition_test_direct_links");
  vector<vector<bool>> matrix;
  Build(filename, &matrix);


  ifstream in(filename, ifstream::in);
  K2TreePartition tree(&in);

  TestDirectLinks(tree, matrix);

  remove(filename);
}
TEST(k2treepartition, InverseLinks) {
  string filename("partition_test_inverse_links");
  vector<vector<bool>> matrix;
  Build(filename, &matrix);

  ifstream in(filename, ifstream::in);
  K2TreePartition tree(&in);

  TestInverseLinks(tree, matrix);

  remove(filename);
}
TEST(k2treepartition, RangeQuery) {
  string filename("partition_test_inverse_links");
  vector<vector<bool>> matrix;
  Build(filename, &matrix);

  ifstream in(filename, ifstream::in);
  K2TreePartition tree(&in);

  TestRangeQuery(tree, matrix);

  remove(filename);
}
