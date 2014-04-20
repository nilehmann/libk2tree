/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <bits/utils/utils.h>
#include <gtest/gtest.h>


using ::libk2tree::utils::LogCeil;
using ::libk2tree::utils::SquaringPow;
using ::libk2tree::utils::Pow;

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
TEST(LogCeil, 5) {
  ASSERT_EQ(15, LogCeil(1073741824, 4));
}
TEST(LogCeil, 6) {
  ASSERT_EQ(7, LogCeil(1223.245, 3));
}
TEST(LogCeil, 7) {
  ASSERT_EQ(9, LogCeil(1245623.245, 5));
}
TEST(LogCeil, 8) {
  ASSERT_EQ(0, LogCeil(0.6111111, 2));
}

TEST(SquaringPow, 1) {
  ASSERT_EQ(16777216, SquaringPow(64, 4));
}
TEST(SquaringPow, 2) {
  ASSERT_EQ(5153632, SquaringPow(22, 5));
}
TEST(SquaringPow, 3) {
  ASSERT_EQ(1, SquaringPow(4542121, 0));
}
TEST(SquaringPow, 4) {
  ASSERT_EQ(-27, SquaringPow(-3, 3));
}

TEST(Pow, 1) {
  ASSERT_EQ(16777216, Pow<unsigned int>(64, 4));
}
TEST(Pow, 2) {
  ASSERT_EQ(5153632, Pow<unsigned int>(22, 5));
}
TEST(Pow, 4) {
  ASSERT_EQ(1, Pow<unsigned int>(4542121, 0));
}
TEST(Pow, 6) {
  ASSERT_EQ(17179869184, Pow<size_t>(4, 17));
}

