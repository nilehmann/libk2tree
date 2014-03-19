/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <gtest/gtest.h>
#include "../src/utils/utils.h"


using ::k2tree_impl::utils::LogCeil;
using ::k2tree_impl::utils::SquaringPow;

TEST(LogCeil, 1) {
  ASSERT_EQ(4, LogCeil(1234567, 53));
}
TEST(LogCeil, 2) {
  ASSERT_EQ(11, LogCeil(1977326743, 7));
}
TEST(LogCeil, 3) {
  ASSERT_EQ(0, LogCeil(1, 54));
}
TEST(LogCeil, 4) {
  ASSERT_EQ(19, LogCeil(1162261466, 3));
}
TEST(LogCeilt, 5) {
  ASSERT_EQ(15, LogCeil(1073741824,4));
}

TEST(SquaringPow, 1) {
  ASSERT_EQ(16777216, SquaringPow(64,4));
}
TEST(SquaringPow, 2) {
  ASSERT_EQ(5153632, SquaringPow(22, 5));
}
TEST(SquaringPow, 3) {
  ASSERT_EQ(1, SquaringPow(4542121,0));
}
