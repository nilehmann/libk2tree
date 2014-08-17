/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_COMPRESSION_COMPRESSOR_H_
#define INCLUDE_COMPRESSION_COMPRESSOR_H_

#include <libk2tree_basic.h>
#include <compression/hash.h>
#include <compression/vocabulary.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>

namespace libk2tree {
namespace compression {
using std::vector;
using std::shared_ptr;

/*
 * Creates new k2tree with the leaves compressed.
 * @param tree
 */
template<class K2Tree, class Fun>
void FreqVoc(const K2Tree &tree, Fun build) {
  try {
    uint cnt = tree.WordsCnt();
    uint size = tree.WordSize();

    Vocabulary words(cnt, size);

    uint pos = 0;
    tree.Words([&] (const uchar *word) {
      words.assign(pos, word);
      ++pos;
    });


    // Count number of different words
    uint diff_cnt = words.sort();

    // Insert words in hash
    HashTable table(diff_cnt);
    vector<uint> posInHash;
    posInHash.reserve(diff_cnt);
    for (uint i = 0; i < cnt; ++i) {
      uint addr;
      if (!table.search(words[i], size, &addr)) {
        table.add(words[i], size, addr);
        posInHash.push_back(addr);
      } else {
        table[addr].weight += 1;
      }
    }


    // Sort words by frequency
    std::sort(posInHash.begin(), posInHash.end(), [&](uint a, uint b) {
      return table[a].weight > table[b].weight;
    });

    shared_ptr<Vocabulary> voc(new Vocabulary(diff_cnt, size));
    for (uint i = 0; i < diff_cnt; ++i) {
      Nword &w = table[posInHash[i]];
      w.codeword = i;
      voc->assign(i, w.word);
    }

    build(table, voc);
  } catch (std::bad_alloc ba) {
    std::cerr << "[comperssion::FreqVoc] Error:" << ba.what() << "\n";
    exit(1);
  } catch (...) {
    std::cerr << "[comperssion::FreqVoc] Error: unexpected exception\n";
    exit(1);
  }
}

}  // namespace compression
}  // namespace libk2tree
#endif  // INCLUDE_COMPRESSION_COMPRESSOR_H_
