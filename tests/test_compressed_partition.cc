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
using ::libk2tree::CompressedPartition;
using ::boost::filesystem::remove;
using ::std::shared_ptr;

typedef unsigned int uint;


shared_ptr<CompressedPartition>
BuildCompressed(vector<vector<bool>> *matrix) {
  uint n = 1000;

  uint e = (uint) rand()%(n*10) + 1;

  matrix->resize(n, vector<bool>(n, false));
  for (uint i = 0; i < e; ++i) {
    uint p = (uint) rand()%n;
    uint q = (uint) rand()%n;
    (*matrix)[p][q] = true;
  }

  uint k0 = 10;
  uint subm = n/k0;

  string tmp = "temp_file_construction";

  K2TreePartitionBuilder b(n, subm, 4, 2, 2, 3, tmp);
  for (uint row = 0; row < k0; ++row) {
    for (uint col = 0; col < k0; ++col) {
      for (uint i = 0; i < subm; ++i) {
        for (uint j = 0; j < subm; ++j) {
          if ((*matrix)[i + row*subm][j + col*subm])
            b.AddLink(i + row*subm, j + col*subm);
        }
      }
      b.BuildSubtree();
    }
  }
  ifstream in(tmp, ifstream::in);
  K2TreePartition tree(&in);
  in.close();

  string filename = "compressed_partition";

  ofstream out(filename, ofstream::out);
  tree.CompressLeaves(&out);
  out.close();

  in.open(filename, ifstream::in);
  shared_ptr<CompressedPartition> t(new CompressedPartition(&in));
  in.close();

  remove(tmp);
  remove(filename);
  return t;
}

TEST(CompressedPartition, CheckLink) {
  time_t t = time(NULL);
  fprintf(stderr, "TIME: %ld\n", t);
  srand((uint) t);

  vector<vector<bool>> matrix;
  shared_ptr<CompressedPartition> tree = BuildCompressed(&matrix);


  TestCheckLink(*tree, matrix);
}

TEST(CompressedPartition, DirectLinks) {
  vector<vector<bool>> matrix;
  shared_ptr<CompressedPartition> tree = BuildCompressed(&matrix);

  TestDirectLinks(*tree, matrix);
}
TEST(CompressedPartition, InverseLinks) {
  vector<vector<bool>> matrix;
  shared_ptr<CompressedPartition> tree = BuildCompressed(&matrix);

  TestInverseLinks(*tree, matrix);
}
TEST(CompressedPartition, RangeQuery) {
  vector<vector<bool>> matrix;
  shared_ptr<CompressedPartition> tree = BuildCompressed(&matrix);

  TestRangeQuery(*tree, matrix);
}
