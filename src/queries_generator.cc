/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <bits/utils/utils.h>
#include <fstream>
#include <string>

using ::std::ofstream;
using ::std::stoi;
using libk2tree::utils::SaveValue;
typedef unsigned int uint;

int main(int argc, char *argv[]) {
  srand(time(NULL));

  uint cnt_qry = stoi(argv[2]);
  uint nodes = stoi(argv[3]);

  ofstream out;
  out.open(argv[1], ofstream::out);
  SaveValue<uint>(&out, cnt_qry);
  uint i;
  for (i = 0; i < cnt_qry; ++i)
    SaveValue<uint>(&out, rand()%nodes);
}


