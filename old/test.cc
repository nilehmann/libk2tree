/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <cmath>
#include <string>
#include <fstream>
#include <cstdio>
#include <sys/times.h>
#include <ctime>
#include <unistd.h>


using ::std::stoi;

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

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <PLAIN GRAPH> S\n", argv[0]);
    exit(0);
  }
  char *in_file = argv[1];
  int S = stoi(argv[2]);

  //ifstream in;
  //in.open(in_file, ifstream::in);
  FILE *in;
  in = fopen (in_file, "r");

  unsigned int nodes;
  unsigned long edges;
  fread(&nodes, sizeof(unsigned int),1 , in);
  fread(&edges, sizeof(unsigned long), 1, in); 

  printf("nodes: %d, edges: %ld\n", nodes, edges);
  ticks = (double)sysconf(_SC_CLK_TCK);
  start_clock();
  for (unsigned long i = 0; i < S && i < nodes ; ++i) {
    //int read = LoadValue<int>(&in);
    unsigned int cnt;
    fread(&cnt, sizeof(int), 1, in);
    fprintf(stderr, "%d %d: ", i, cnt);
    if (cnt == 0) {
      fprintf(stderr,"\n");
      continue;
    }
    int n;
    int sum = 0;
    do {
      fread(&n, sizeof(int), 1, in);
      fprintf(stderr,"%d ", n);
      sum++;
    } while (n < S && sum < cnt);
    fprintf(stderr,"\n");
    fseek(in, (cnt - sum)*sizeof(int), SEEK_CUR);
  }
  //in.close();
  fclose(in);
  unsigned t = stop_clock();
  fprintf(stderr, "%d seconds.\n", t);
}
