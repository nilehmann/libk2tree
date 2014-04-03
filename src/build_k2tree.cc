/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2tree.h>
#include <bits/utils/utils.h>
#include <string>
#include <fstream>
#include <memory>
#include <cstdio>


using ::std::ifstream;
using ::std::ofstream;
using ::std::stoi;
using ::k2tree_impl::utils::LoadValue;
using ::std::shared_ptr;
using ::k2tree_impl::K2Tree;
using ::k2tree_impl::K2TreeBuilder;

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

int main(int arc, char *argv[]) {
  char *in_file = argv[1];
  char *out_file = argv[2];
  int k1 = stoi(argv[3]);
  int k2 = stoi(argv[4]);
  int kl = k2*k2*k2;
  int k1_levels = stoi(argv[5]);
  printf("k1: %d, k2: %d, kl: %d, k1_levels: %d\n", k1, k2, kl, k1_levels);

  //ifstream in;
  //in.open(in_file, ifstream::in);
  FILE *in;
  in = fopen (in_file, "r");

  //unsigned int nodes = LoadValue<unsigned int>(&in);
  //unsigned long edges = LoadValue<unsigned long>(&in);
  unsigned int nodes;
  unsigned long edges;
  fread(&nodes, sizeof(unsigned int),1 , in);
  fread(&edges, sizeof(unsigned long), 1, in); 


  printf("nodes: %d, edges: %ld\n", nodes, edges);
  ticks = (double)sysconf(_SC_CLK_TCK);
  start_clock();
  K2TreeBuilder<uint> tb(nodes, k1, k2, kl, k1_levels);
  int p;
  for (int i = 0; i < nodes + edges; ++i) {
    //int read = LoadValue<int>(&in);
    int read;
    fread(&read, sizeof(int), 1, in);
    if (read < 0) {
      p = -read - 1;
    } else {
      int q = read - 1;
      tb.AddLink(p,q);
    }
  }
  //in.close();
  fclose(in);
  unsigned t = stop_clock();
  fprintf(stderr, "%d seconds to insert.\n", t);

  start_clock(); 
  shared_ptr<K2Tree<uint> > tree = tb.Build();
  t = stop_clock();
  fprintf(stderr, "%d seconds to build.\n", t);

  ofstream out;
  out.open(out_file, ofstream::out);
  tree->Save(&out);
  out.close();

}
