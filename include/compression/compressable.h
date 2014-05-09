/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_COMPRESSION_COMPRESSABLE_H_
#define INCLUDE_COMPRESSION_COMPRESSABLE_H_

#include <libk2tree_basic.h>
#include <compression/hash.h>
#include <compression/array.h>
#include <vector>
#include <algorithm>

namespace libk2tree {
namespace compression {
using utils::Ceil;
using std::vector;
using std::shared_ptr;

template<class Uncompressed, class Compressed>
class compressable {
 public:
  /*
   * Creates new k2tree with the leaves compressed.
   * @param tree
   */
  shared_ptr<Compressed> CompressLeaves() {
    uint cnt = WordsCnt();
    uint size = WordSize();

    Array<uchar> words(cnt, size);

    uint pos = 0;
    Words([&] (const uchar *word) {
      words.assign(pos, word);
      ++pos;
    });

    // Count number of different words
    uint diff_cnt = words.sort();

    // Insert
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


    std::sort(posInHash.begin(), posInHash.end(), [&](uint a, uint b) {
      return table[a].weight > table[b].weight;
    });

    shared_ptr<Array<uchar>> voc(new Array<uchar>(diff_cnt, size));
    for (uint i = 0; i < diff_cnt; ++i) {
      Nword &w = table[posInHash[i]];
      w.codeword = i;
      voc->assign(i, w.word);
    }


    return BuildCompressedTree(table, voc);
  }

  template<class Function>
  void Words(Function fun) {
    static_cast<const Uncompressed&>(*this).Words(fun);
  }
  uint WordsCnt() {
    return static_cast<const Uncompressed&>(*this).WordsCnt();
  }
  uint WordSize() {
    return static_cast<const Uncompressed&>(*this).WordSize();
  }
  shared_ptr<Compressed> BuildCompressedTree(const HashTable &t,
                                             shared_ptr<Array<uchar>> voc) {
    return static_cast<const Uncompressed&>(*this).BuildCompressed(t, voc);
  }
};




}  // namespace compression
}  // namespace libk2tree
#endif  // INCLUDE_COMPRESSION_COMPRESSABLE_H_
