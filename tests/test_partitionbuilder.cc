/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <gtest/gtest.h>
#include <k2treepartition_builder.h>
#include <bits/utils/utils.h>
#include <cstdio>
#include <boost/filesystem.hpp>

using ::libk2tree::K2TreePartitionBuilder;
using ::libk2tree::utils::Ceil;
using ::boost::filesystem::remove;

TEST(k2treepartition_builder, 1) {
  srand(time(NULL));
  int n = 10000;

  int e = rand()%(n*10) + 1;

  vector<vector<bool>> matrix(n, vector<bool>(n, false));
  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int q = rand()%n;
    matrix[p][q] = true;
  }

  int k0 = 10;
  int subm = n/k0;


  K2TreePartitionBuilder b(n, subm, 4, 2, 2, 3, "partition_test123");
  for (int row = 0; row < k0; ++row) {
    for (int col = 0; col < k0; ++col) {
      for (int i = 0; i < subm; ++i) {
        for (int j = 0; j < subm; ++j) {
          if (matrix[i + row*subm][j + col*subm])
            b.AddLink(i + row*subm, j + col*subm);
        }
      }
      b.BuildSubtree();
    }
  }
  remove("partition_test123");
}
