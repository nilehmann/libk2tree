/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */


#include <gtest/gtest.h>
#include <bits/utils/bitarray.h>

using  ::libk2tree::utils::BitArray;

TEST(BitArrayChar, Constructor) {
  BitArray<char> bs(1000);
  for (int i = 0; i < 1000; ++i)
    ASSERT_EQ(0, bs.GetBit(i));
}
TEST(BitArray, SetBit0) {
  BitArray<char> bs(3);
  bs.SetBit(0);
  ASSERT_EQ(1, bs.GetBit(0));
}
TEST(BitArrayChar, SetBit) {
  srand(time(NULL));
  for (int i = 0; i < 100; ++i) {
    size_t N = rand()%10000 + 1000;
    BitArray<char> bs(N);

    for (int j = 0; j < 100 ; ++j) {
      size_t pos = rand()%N;
      bs.SetBit(pos);
      ASSERT_EQ(1, bs.GetBit(pos));
    }
  }
}

TEST(BitArrayChar, CleanBit) {
  srand(time(NULL));
  for (int i = 0; i < 100; ++i) {
    size_t N = rand()%10000 + 1000;
    BitArray<char> bs(N);

    for (int j = 0; j < 100 ; ++j) {
      size_t pos = rand()%N;
      bs.CleanBit(pos);
      ASSERT_EQ(0, bs.GetBit(pos));
    }
  }
}


TEST(BitArrayInt, Constructor) {
  BitArray<int> bs(1000);

  for (int i = 0; i < 1000; ++i)
    ASSERT_EQ(0, bs.GetBit(i));
}
TEST(BitArrayInt, SetBit) {
  srand(time(NULL));
  for (int i = 0; i < 100; ++i) {
    size_t N = rand()%10000 + 1000;
    BitArray<int> bs(N);

    for (int j = 0; j < 100 ; ++j) {
      size_t pos = rand()%N;
      bs.SetBit(pos);
      ASSERT_EQ(1, bs.GetBit(pos));
    }
  }
}


TEST(BitArrayInt, CleanBit) {
  srand(time(NULL));
  for (int i = 0; i < 100; ++i) {
    size_t N = rand()%10000 + 1000;
    BitArray<int> bs(N);

    for (int j = 0; j < 100 ; ++j) {
      size_t pos = rand()%N;
      bs.CleanBit(pos);
      ASSERT_EQ(0, bs.GetBit(pos));
    }
  }
}
TEST(BitArray, CopyConstructor) {
  srand(time(NULL));
  for (int i = 0; i < 100; ++i) {
    size_t N = rand()%10000 + 1000;
    BitArray<int> bs(N);

    for (int j = 0; j < 100 ; ++j) {
      size_t pos = rand()%N;
      bs.SetBit(pos);
    }
    BitArray<int> bs2(bs);
    for (int j = 0; j < 100 ; ++j) {
      size_t pos = rand()%N;
      ASSERT_EQ(bs.GetBit(pos), bs2.GetBit(pos));
    }
  }
}
