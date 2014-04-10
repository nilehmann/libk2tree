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
#include <cstdio>
#include <cmath>
#include <cstring>
#include <fstream>
#include <memory>
using std::ifstream;
using std::shared_ptr;
using libk2tree::K2Tree;
using libk2tree::utils::LoadValue;

typedef unsigned int uint;

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

int main(int argc, char* argv[]){

  if(argc<2){
    fprintf(stderr,"USAGE: %s <GRAPH>\n",argv[0]);
    return(-1);
  }

  //char *filename = (char *)malloc(sizeof(char)*20);
  ifstream in; 
  in.open(argv[1], ifstream::in);

  K2Tree tree(&in);

  ifstream in_queries;
  in_queries.open(argv[2], ifstream::in);
  uint cnt_qry = LoadValue<uint>(&in_queries);
  
  uint *qry = new uint[cnt_qry];
  qry = LoadValue<uint>(&in_queries, cnt_qry);

  fprintf(stderr,"Processing %d queries\n",cnt_qry);
  double t = 0;
  ticks= (double)sysconf(_SC_CLK_TCK);
  start_clock();
  uint i;
  uint recovered = 0;
  for(i=0;i<cnt_qry;i++) {
    //K2Tree::DirectIterator q = tree.DirectBegin(qry[i]); 
    K2Tree::RangeIterator q = tree.RangeBegin(qry[i], qry[i], 0, tree.cnt()-1); 
    //for (; q != tree.DirectEnd(); ++q,++recovered);
    for (; q != tree.RangeEnd(); ++q,++recovered);
  }
  t += stop_clock(); 
  t *= 1000; // to milliseconds

  fprintf(stderr,"Recovered Nodes: %d\n",recovered);
  fprintf(stderr,"Queries: %d\n",cnt_qry);
  fprintf(stderr,"Total time(ms): %f",t);
  fprintf(stderr,"Time per query: %f\n",t/cnt_qry);
  fprintf(stderr,"Time per link: %f\n",t/recovered);

  // destroyTreeRepresentation(trep);


  return 0;
}

