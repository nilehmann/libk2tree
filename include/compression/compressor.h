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
#include <climits>

namespace libk2tree {
namespace compression {
using std::shared_ptr;

/**
 * Creates new k2tree with the leaves compressed.
 * @param tree
 * @param build 
 */
template<class K2Tree, class Fun>
void FreqVoc(const K2Tree &tree, Fun build) {
  try {
    size_t cnt = tree.WordsCnt();
    uint size = tree.WordSize();

    Vocabulary words(cnt, size);

    size_t pos = 0;
    tree.Words([&] (const uchar *word) {
      words.assign(pos, word);
      ++pos;
    });


    // Count number of different words
    // We hope there are many repetead words. We need to encode each word in
    // a 32-bit integer.
    size_t res = words.sort();
    if (res > INT_MAX) {
      std::cerr << "[comperssion::FreqVoc] Too many different words ";
      std::cerr << "in the vocabulary\n";
      exit(1);
    }
    uint diff_cnt = (uint) res;

    // Insert words in hash
    HashTable table(diff_cnt);
    std::vector<size_t> posInHash;
    posInHash.reserve(diff_cnt);
    for (size_t i = 0; i < cnt; ++i) {
      size_t addr;
      if (!table.search(words[i], size, &addr)) {
        table.add(words[i], size, addr);
        posInHash.push_back(addr);
      } else {
        table[addr].weight += 1;
      }
    }

    // Sort words by frequency
    std::sort(posInHash.begin(), posInHash.end(), [&](size_t a, size_t b) {
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
