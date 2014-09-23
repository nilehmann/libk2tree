/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <utils/bitarray.h>
#include <utils/utils.h>
#include <fstream>
#include <cstdio>
#include <string>

using ::std::ofstream;
using libk2tree::utils::SaveValue;
using ::libk2tree::utils::BitArray;
typedef unsigned int uint;

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("Usage: %s file nodes cnt_qry\n", argv[0]);
    exit(0);
  }
  srand((uint) time(NULL));

  uint nodes = (uint) std::stoi(argv[2]);
  uint cnt_qry = (uint) std::stoi(argv[3]);

  ofstream out;
  out.open(argv[1], ofstream::out);
  SaveValue<uint>(&out, cnt_qry);
  for (uint i = 0; i < cnt_qry; ++i)
    SaveValue<uint>(&out, (uint) rand()%nodes);
}


