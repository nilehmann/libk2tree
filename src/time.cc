/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#include <k2tree.h>
#include <utils/array_queue.h>
#include <utils/utils.h>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <fstream>
#include <memory>
#include <vector>

#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

namespace po = boost::program_options;

using std::ifstream;
using std::shared_ptr;
using std::vector;
using libk2tree::HybridK2Tree;
using libk2tree::CompressedHybrid;
using libk2tree::CompressedPartition;
using libk2tree::K2TreePartition;
using libk2tree::utils::LoadValue;
using libk2tree::utils::ArrayQueue;

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
template<class K2Tree>
uint TimeDirect(uint *qry, uint cnt_qry, const K2Tree &tree);

template<class K2Tree>
uint TimeInverse(uint *qry, uint cnt_qry, const K2Tree &tree);

template<class K2Tree>
uint TimeRange(uint *qry, uint cnt_qry, const K2Tree &tree);

template<class K2Tree>
uint Time(uint *qry, uint cnt_qry, const K2Tree &tree);

bool compress = false, part = false;
string graph_str, queries_str;
int type;
double t;

void ParseOps(int argc, char *argv[]) {
  po::options_description ops("Usage: time [options] graph queries"
                              "Allowed options");
  ops.add_options()
    ("help,h", "Print this help")
    ("compress,c", "Loads a tree with compressed leaves")
    ("type,t", po::value<int>(&type)->default_value(1), "Type of query 1: Direct, 2: Inverse, 3: Range")
    ("partition,p", "Loads a tree with first level partition");

  po::options_description files;
  files.add_options()
    ("graph", po::value<string>(), "Input graph file")
    ("queries", po::value<string>(), "Queries file");

  po::options_description all("All options");
  all.add(ops).add(files);

  po::positional_options_description p;
  p.add("graph", 1);
  p.add("queries", 1);

  po::variables_map map;
  po::store(po::command_line_parser(argc, argv).
            options(all).positional(p).run(), map);
  po::notify(map);

  if (map.count("help")) {
    std::cout << ops;
    exit(0);
  }
  if (map.count("compress"))
    compress = true;
  if (map.count("partition"))
    part = true;


  graph_str = map["graph"].as<string>();
  queries_str = map["queries"].as<string>();
}
int main(int argc, char* argv[]){
  ParseOps(argc, argv);

  ifstream in_queries(queries_str, ifstream::in);
  uint cnt_qry = LoadValue<uint>(&in_queries);
  uint *qry = new uint[cnt_qry];
  qry = LoadValue<uint>(&in_queries, cnt_qry);
  in_queries.close();

  fprintf(stderr,"Processing %d queries\n",cnt_qry);


  uint recovered = 0;
  ifstream in(graph_str, ifstream::in);
  if (compress && part)
    recovered = Time(qry, cnt_qry, CompressedPartition(&in));
  else if(!compress && part)
    recovered = Time(qry, cnt_qry, K2TreePartition(&in));
  else if(!compress && !part)
    recovered = Time(qry, cnt_qry, HybridK2Tree(&in));
  else if(compress && !part)
    recovered = Time(qry, cnt_qry, CompressedHybrid(&in));

  in.close();

  fprintf(stderr,"Recovered Nodes: %d\n",recovered);
  fprintf(stderr,"Total time(ms): %f\n",t);
  fprintf(stderr,"Time per query: %f\n",t/cnt_qry);
  fprintf(stderr,"Time per link: %f\n",t/recovered);

  return 0;
}

template<class K2Tree>
uint Time(uint *qry, uint cnt_qry, const K2Tree &tree) {

  ticks = (double)sysconf(_SC_CLK_TCK);
  uint recovered = 0;
  start_clock();
  if (type == 1)
    recovered = TimeDirect(qry, cnt_qry, tree);
  else if (type == 2)
    recovered = TimeInverse(qry, cnt_qry, tree);
  else
    recovered = TimeRange(qry, cnt_qry, tree);
  t += stop_clock(); 
  t *= 1000; // to milliseconds
  return recovered;
}


ArrayQueue<uint> q1, q2;
template<class K2Tree>
uint TimeRange(uint *qry, uint cnt_qry, const K2Tree &tree) {
  printf("Range Query\n");
  uint i;
  uint recovered = 0;

  for(i=0;i< cnt_qry;i++) {
    //vector<vector<uint> > vec(21);
    q1.clear();
    q2.clear();
    tree.RangeQuery(qry[i], qry[i]+20, 0, tree.cnt() - 1,
    [&] (uint p, uint q) {
      //vec[p-qry[i]].push_back(q);
      q1.push(p);
      q2.push(q);
      ++recovered;
    });
  }
  return recovered;
}

template<class K2Tree>
uint TimeDirect(uint *qry, uint cnt_qry, const K2Tree &tree) {
  printf("Direct Links\n");
  uint i;
  uint recovered = 0;
  for(i=0;i< cnt_qry;i++) {
    vector<uint> vec;
    tree.DirectLinks(qry[i], [&] (uint q){
      vec.push_back(q); 
      ++recovered;
    });
  }
  return recovered;
}


template<class K2Tree>
uint TimeInverse(uint *qry, uint cnt_qry, const K2Tree &tree) {
  printf("Inverse Links\n");
  uint i;
  uint recovered = 0;
  for(i=0;i< cnt_qry;i++) {
    vector<uint> vec;
    tree.InverseLinks(qry[i], [&] (uint q){
      vec.push_back(q); 
      ++recovered;
    });
  }
  return recovered;
}
