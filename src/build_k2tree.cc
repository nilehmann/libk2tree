/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2tree.h>
#include <utils/utils.h>
#include <string>
#include <fstream>
#include <memory>
#include <cstdio>
#include <iostream>
using namespace std;


using ::std::ifstream;
using ::std::ofstream;
using ::std::stoi;
using ::libk2tree::utils::LoadValue;
using ::std::shared_ptr;
using ::libk2tree::HybridK2Tree;
using ::libk2tree::K2TreeBuilder;
using ::libk2tree::K2TreePartitionBuilder;
typedef unsigned int uint;
typedef unsigned long ulong;

/* Time meassuring */
double ticks;
struct tms t1,t2;

void start_clock() {
	times (&t1);
}

double stop_clock() {
	times (&t2);
	return (t2.tms_utime-t1.tms_utime)/ticks;
}
/* end Time meassuring */

char *in_file, *out_file;
int k1, k2, kl;
int k1_levels;
int S;


void build_k2tree();
void build_k2tree_partition();
template<class Function>
void Neighbors(streampos pos, ifstream *in,
               uint p1, uint p2, vector<streamoff> &next, uint q2, Function fun);

int main(int argc, char *argv[]) {
  ticks = (double)sysconf(_SC_CLK_TCK);

  if (argc < 6) {
    printf("Usage: %s in out k1 k2 k1_levels [S]\n", argv[0]);
    exit(0);
  }
  in_file = argv[1];
  out_file = argv[2];
  k1 = stoi(argv[3]);
  k2 = stoi(argv[4]);
  kl = k2*k2*k2;
  k1_levels = stoi(argv[5]);

  uint t;
  printf("k1: %d, k2: %d, kl: %d, k1_levels: %d", k1, k2, kl, k1_levels);
  if (argc > 6) {
    S = stoi(argv[6]);
    printf(" S: %d\n", S);
    start_clock();
    build_k2tree_partition();
    t = stop_clock();
  } else {
    printf("\n");
    start_clock();
    build_k2tree();
    t = stop_clock();
  }

  printf("%d seconds to build.\n", t);

}

void build_k2tree() {
  ifstream in(in_file, ifstream::in);

  uint nodes = LoadValue<uint>(&in);
  ulong edges = LoadValue<ulong>(&in);

  printf("nodes: %d, edges: %ld\n", nodes, edges);

  K2TreeBuilder tb(nodes, k1, k2, kl, k1_levels);
  for (uint p = 0; p < nodes; ++p) {
    uint cnt = LoadValue<uint>(&in);
    for (uint i = 0; i < cnt; ++i) {
      uint q = LoadValue<uint>(&in);
      tb.AddLink(p, q);
    }
  }
  in.close();


  // Build and save k2tree
  ofstream out(out_file, ofstream::out);
  tb.Build(&out);
  out.close();

}

void build_k2tree_partition() {
  ifstream in(in_file, ifstream::in);

  uint nodes = LoadValue<uint>(&in);
  ulong edges = LoadValue<ulong>(&in);
  uint subm_size = 1 << S;

  printf("nodes: %d, edges: %ld, submatrix size: %d\n", nodes, edges, subm_size);


  K2TreePartitionBuilder tb(nodes, subm_size, k1, k2, kl, k1_levels, out_file);

  
  vector<streamoff> next(subm_size, 0);
  int k0 = tb.k0();
  printf("k0: %d\n", k0);

  for (int row = 0; row < k0; ++row) {
    uint p1 = row*subm_size;
    uint p2 = (row+1)*subm_size - 1 < nodes ? (row+1)*subm_size - 1: nodes - 1;

    streampos pos = in .tellg();
    std::fill(next.begin(), next.end(), 0);
    for (int col = 0; col < k0; ++col) {
      uint q1 = col*subm_size;
      uint q2 = (col+1)*subm_size - 1 < nodes ? (col+1)*subm_size - 1: nodes - 1;

      printf("row= %d, col=%d, edges between (%d,%d) and (%d, %d)\t(%2.2f%%)\n",
          row, col, p1, p2, q1, q2, (float)(row*k0+col)*100/k0/k0);
      Neighbors(pos, &in, p1, p2, next, q2, [&] (uint p, uint q) {
        tb.AddLink(p, q);
      });

      tb.BuildSubtree();
    }
  }

}

template<class Function>
void Neighbors(streampos pos, ifstream *in,
               uint p1, uint p2, vector<streamoff> &next, uint q2, Function fun) {
  in->seekg(pos);

  for (uint p = p1; p <= p2; ++p) {
    uint cnt = LoadValue<uint>(in);

    // Seek to neighbor
    in->seekg(next[p - p1]*sizeof(uint), ifstream::cur);

    uint q;
    while(next[p-p1] < cnt && (q = LoadValue<uint>(in)) <= q2) {
      fun(p, q);
      ++next[p - p1];
    }
    // Seek to next node
    if (next[p-p1] < cnt)
      in->seekg((cnt - next[p-p1] - 1)*sizeof(uint), ifstream::cur);

  }

}

