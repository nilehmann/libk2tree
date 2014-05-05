/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */


#ifndef INCLUDE_LEAVES_COMPRESSOR_H_
#define INCLUDE_LEAVES_COMPRESSOR_H_
namespace libk2tree {

/*
 * Return vocabulary sorted by frequency. For efficiency the array words can be
 * modified.
 * @param words Concatenations of words.
 * @param size Size of each word in bytes.
 * @param cnt Number of words in the array.
 */
template<class k2tree>
void FreqVocabulary(char *words, int size, k2tree::words_cnt cnt) {
  //TODO QuickSort *words counting number of unique elements
  //TODO Generate vocabulary inserting each word in array and
  //hash.
  //TODO Return vocabulary

}

}  // namespace libk2tree
#endif  // INCLUDE_LEAVES_COMPRESSOR_H_
