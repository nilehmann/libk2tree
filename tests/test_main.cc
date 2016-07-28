/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <gtest/gtest.h>
#include <pthread.h>
#include "test_bitarray.cc"
#include "test_compressed_hybrid.cc"
#include "test_compressed_partition.cc"
#include "test_k2tree.cc"
#include "test_k2treebuilder.cc"
#include "test_k2treepartition.cc"
#include "test_utils.cc"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
