/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <string>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>


using ::std::ifstream;
using ::std::ofstream;
using ::std::stoi;
using ::std::stol;

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <nodes> <edges>\n", argv[0]);
    exit(0);
  }

  int nodes = stoi(argv[1]);
  unsigned long edges = stol(argv[2]);



  srand(time(NULL));

  write(1, &nodes, sizeof(int));
  double *dedges = new double[nodes];
  int *vedges = new int[nodes];
  double sum = 0;
  for (int i = 0; i < nodes; ++i) {
    dedges[i] = rand()*1.0/RAND_MAX;
    sum += dedges[i];
  }
  unsigned long cnt_edges = 0;
  for (int i = 0; i < nodes; ++i) {
    vedges[i] = round(dedges[i]*edges/sum);
    cnt_edges += vedges[i];
  }
  delete [] dedges;
  

  write(1, &cnt_edges, sizeof(unsigned long));
  fprintf(stderr,"Actual edges %d\n", cnt_edges);

  for (int i = 0; i < nodes; ++i) {
    write(1, vedges + i, sizeof(int));
    int v[vedges[i]];
    for (int j = 0; j < vedges[i]; ++j)
      v[j] = rand()%nodes;

    std::sort(v, v + vedges[i]);
    for (int j = 0; j < vedges[i]; ++j)
      write(1, v+j, sizeof(int));
  }

}


