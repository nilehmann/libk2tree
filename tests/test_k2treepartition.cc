/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <gtest/gtest.h>
#include <k2tree.h>
#include <bits/utils/utils.h>
#include <boost/filesystem.hpp>
#include <cstdio>

using ::libk2tree::K2TreePartitionBuilder;
using ::libk2tree::utils::Ceil;
using ::libk2tree::K2TreePartition;
using ::boost::filesystem::remove;

typedef unsigned int uint;

vector<uint> GetSuccessors(const vector<vector<bool>> &matrix, uint p);
vector<uint> GetPredecessors(const vector<vector<bool>> &matrix, uint q);
vector<pair<uint, uint>> GetEdges(const vector<vector<bool>> &matrix,
                                  uint p1, uint p2, uint q1, uint q2);

TEST(k2treepartition, CheckLink) {
  string filename("partition_test_check_link");
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


  K2TreePartitionBuilder b(n, subm, 4, 2, 2, 3, filename);
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

  ifstream in(filename, ifstream::in);
  K2TreePartition tree(&in);

  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int q = rand()%n;
    ASSERT_EQ(matrix[p][q], tree.CheckLink(p, q));
  }

  remove(filename);
}

TEST(k2treepartition, DirectLinks) {
  string filename("partition_test_direct_links");
  srand(time(NULL));
  int n = 10000;

  vector<vector<bool>> matrix(n, vector<bool>(n, false));

  int p = rand()%n;
  int e = n > 100 ? rand()%(n/100) + 1 : 1;
  for (int i = 0; i < e; ++i) {
    int q = rand()%n;
    int r = rand()%n;
    matrix[p][q] = true;
    // to make noise
    matrix[q][r] = true;
  }

  int k0 = 10;
  int subm = n/k0;


  K2TreePartitionBuilder b(n, subm, 4, 2, 2, 3, filename);
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

  ifstream in(filename, ifstream::in);
  K2TreePartition tree(&in);

  vector<uint> v = GetSuccessors(matrix, p);
  uint i = 0;
  tree.DirectLinks(p, [&] (unsigned int q) {
    ASSERT_EQ(v[i++], q);
  });
  ASSERT_EQ(v.size(), i);

  remove(filename);
}
TEST(k2treepartition, InverseLinks) {
  string filename("partition_test_inverse_links");
  srand(time(NULL));
  int n = 10000;

  vector<vector<bool>> matrix(n, vector<bool>(n, false));

  int q = rand()%n;
  int e = n > 100 ? rand()%(n/100) + 1 : 1;
  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int r = rand()%n;
    matrix[p][q] = true;
    // to make noise
    matrix[r][p] = true;
  }

  int k0 = 10;
  int subm = n/k0;


  K2TreePartitionBuilder b(n, subm, 4, 2, 2, 3, filename);
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

  ifstream in(filename, ifstream::in);
  K2TreePartition tree(&in);

  vector<uint> v = GetPredecessors(matrix, q);
  uint i = 0;
  tree.InverseLinks(q, [&] (unsigned int p) {
    ASSERT_EQ(v[i++], p);
  });
  ASSERT_EQ(v.size(), i);

  remove(filename);
}
TEST(k2treepartition, RangeQuery) {
  string filename("partition_test_inverse_links");
  srand(time(NULL));
  int n = 10000;

  vector<vector<bool>> matrix(n, vector<bool>(n, false));

  int e = n > 100 ? rand()%(n/100) + 1 : 1;
  for (int i = 0; i < e; ++i) {
    int p = rand()%n;
    int q = rand()%n;
    matrix[p][q] = true;
  }

  int k0 = 10;
  int subm = n/k0;


  K2TreePartitionBuilder b(n, subm, 4, 2, 2, 3, filename);
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

  int p1 = rand()%(n/2);
  int p2 = rand()%(n/2) + n/2;
  int q1 = rand()%(n/2);
  int q2 = rand()%(n/2) + n/2;

  ifstream in(filename, ifstream::in);
  K2TreePartition tree(&in);

  vector<pair<uint, uint> > v = GetEdges(matrix, p1, p2, q1, q2);
  uint i = 0;
  vector<pair<uint, uint> > v1;
  tree.RangeQuery(p1, p2, q1, q2, [&] (unsigned int p, unsigned int q) {
    i++;
    ASSERT_TRUE(matrix[p][q]);
    v1.emplace_back(p, q);
  });
  ASSERT_EQ(v.size(), i);

  /* Test if there are repeated elements */
  int size = v1.size();
  sort(v1.begin(), v1.end());
  unique(v1.begin(), v1.end());
  ASSERT_EQ(size, v1.size());
  remove(filename);
}
