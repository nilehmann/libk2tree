/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */


#include <gtest/gtest.h>
#include <utils/bitstring.h>

using  ::k2tree_impl::utils::BitString;

TEST(BitStringChar, Constructor) {
  BitString<char> bs(1000);
  for (int i = 0; i < 1000; ++i)
    ASSERT_EQ(0, bs.GetBit(i));
}
TEST(BitString, SetBit0) {
  BitString<char> bs(3);
  bs.SetBit(0);
  ASSERT_EQ(1, bs.GetBit(0));
}
TEST(BitStringChar, SetBit) {
  srand(time(NULL));
  for (int i = 0; i < 100; ++i) {
    size_t N = rand()%10000 + 1000;
    BitString<char> bs(N);

    for (int j = 0; j < 100 ; ++j) {
      size_t pos = rand()%N;
      bs.SetBit(pos);
      ASSERT_EQ(1, bs.GetBit(pos));
    }
  }
}

TEST(BitStringChar, CleanBit) {
  srand(time(NULL));
  for (int i = 0; i < 100; ++i) {
    size_t N = rand()%10000 + 1000;
    BitString<char> bs(N);

    for (int j = 0; j < 100 ; ++j) {
      size_t pos = rand()%N;
      bs.CleanBit(pos);
      ASSERT_EQ(0, bs.GetBit(pos));
    }
  }
}


TEST(BitStringInt, Constructor) {
  BitString<int> bs(1000);

  for (int i = 0; i < 1000; ++i)
    ASSERT_EQ(0, bs.GetBit(i));
}
TEST(BitStringInt, SetBit) {
  srand(time(NULL));
  for (int i = 0; i < 100; ++i) {
    size_t N = rand()%10000 + 1000;
    BitString<int> bs(N);

    for (int j = 0; j < 100 ; ++j) {
      size_t pos = rand()%N;
      bs.SetBit(pos);
      ASSERT_EQ(1, bs.GetBit(pos));
    }
  }
}

TEST(BitStringInt, CleanBit) {
  srand(time(NULL));
  for (int i = 0; i < 100; ++i) {
    size_t N = rand()%10000 + 1000;
    BitString<int> bs(N);

    for (int j = 0; j < 100 ; ++j) {
      size_t pos = rand()%N;
      bs.CleanBit(pos);
      ASSERT_EQ(0, bs.GetBit(pos));
    }
  }
}
