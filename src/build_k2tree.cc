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

#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

namespace po = boost::program_options;
using namespace std;


using ::std::ifstream;
using ::std::ofstream;
using ::std::stoi;
using ::libk2tree::utils::LoadValue;
using ::std::shared_ptr;
using ::libk2tree::HybridK2Tree;
using ::libk2tree::K2TreeBuilder;
using ::libk2tree::K2TreePartitionBuilder;
using ::libk2tree::CompressedHybrid;
using ::libk2tree::K2TreePartition;

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

string in_file, out_file;
int k1, k2, kl;
int k1_levels;
int S;
bool compress = false;


void build_k2tree();
void build_k2tree_partition();
template<class Function>
void Neighbors(streampos pos, ifstream *in,
               uint p1, uint p2, vector<streamoff> &next, uint q2, Function fun);


void ParseOps(int argc, char *argv[]) {
  po::options_description ops("Usage: build_k2tree [options] input-graph"
                              " output-graph\n"
                              "Allowed options");
  ops.add_options()
    ("help,h", "Print this help")
    ("compress,c", "Builds a tree with compressed leaves")
    ("k1", po::value<int>(&k1)->default_value(4), "Arity of the first leveles")
    ("k2", po::value<int>(&k2)->default_value(2), "Arity of the second part")
    ("kl", po::value<int>(&kl)->default_value(-1), "Arity of the level height - 1")
    ("k1-levels", po::value<int>(&k1_levels)->default_value(5),
     "Number of level with arity k1")
    ("submatrix-size,S", po::value<int>(&S)->default_value(-1),
     "Builds a tree partitioning the first level in submatrices of size"
     " 2^S");

  po::options_description files;
  files.add_options()
    ("input-graph", po::value<string>(), "Input graph file")
    ("output-graph", po::value<string>(), "Output graph file");

  po::options_description all("All options");
  all.add(ops).add(files);

  po::positional_options_description p;
  p.add("input-graph", 1);
  p.add("output-graph", 1);

  po::variables_map map;
  po::store(po::command_line_parser(argc, argv).
            options(all).positional(p).run(), map);
  po::notify(map);

  if(map.count("help")) {
    std::cout << ops;
    exit(0);
  }
  if(map.count("compress"))
    compress = true;

  in_file = map["input-graph"].as<string>();
  out_file = map["output-graph"].as<string>();
  if (kl == -1)
    kl = k2*k2*k2;
}

int main(int argc, char *argv[]) {
  ticks = (double)sysconf(_SC_CLK_TCK);

  ParseOps(argc, argv);

  uint t;
  printf("k1: %d, k2: %d, kl: %d, k1-levels: %d\n", k1, k2, kl, k1_levels);
  printf("Input File: %s\n", in_file.c_str());
  printf("Output File: %s\n", out_file.c_str());
  start_clock();
  if (S > 0)
    build_k2tree_partition();
  else
    build_k2tree();
  
  t = stop_clock();

  printf("%d seconds to build.\n", t);

}

void build_k2tree() {
  ifstream in(in_file, ifstream::in);

  uint nodes = LoadValue<uint>(&in);
  ulong edges = LoadValue<ulong>(&in);

  printf("Nodes: %d, Edges: %ld\n", nodes, edges);

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
  shared_ptr<HybridK2Tree> tree = tb.Build();
  if (compress) {
    shared_ptr<CompressedHybrid> tree2 = tree->CompressLeaves();
    tree2->Save(&out);
  } else {
    tree->Save(&out);
  }
  out.close();
}

void build_k2tree_partition() {
  ifstream in(in_file, ifstream::in);

  uint nodes = LoadValue<uint>(&in);
  ulong edges = LoadValue<ulong>(&in);
  uint subm_size = 1 << S;

  printf("Nodes: %d, Edges: %ld, Submatrix size: %d\n", nodes, edges, subm_size);


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
  in.close();

  if (compress) {
    in.open(out_file, ifstream::in);
    K2TreePartition tree(&in);
    in.close();

    ofstream out(out_file, ofstream::out);
    tree.CompressLeaves(&out);
    out.close();
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

