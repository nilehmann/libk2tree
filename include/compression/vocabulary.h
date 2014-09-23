/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <nlehmann@dcc.uchile.cl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Nicolás Lehmann
 * ----------------------------------------------------------------------------
 */

#ifndef INCLUDE_COMPRESSION_VOCABULARY_H_
#define INCLUDE_COMPRESSION_VOCABULARY_H_

#include <libk2tree_basic.h>
#include <algorithm>
#include <fstream>

namespace libk2tree {
namespace compression {

class Vocabulary {
 public:
  /**
   * @param cnt Number of words in the vocabulary
   * @param size Size of each word in bytes
   */
  Vocabulary(size_t cnt, uint size);

  explicit Vocabulary(std::ifstream *in);

  void Save(std::ofstream *out);

  const uchar *operator[](size_t i) const {
    return data_ + i*size_;
  }
  const uchar *get(size_t i) const {
    return data_ + i*size_;
  }

  /**
   * Print the vocabulary
   */
  void print() const;
  /**
   * Print word on the given position
   * @param i
   */
  void print(size_t i) const;

  /**
   * Stores the given word in the specified position.
   * @param p Position to store the word.
   * @param word Word to store.
   */
  void assign(size_t p, const uchar* word);

  /**
   * Sort the vocabulary lexicographically and return the number of differents
   * words.
   * @return Number of differents words.
   */
  size_t sort() {
    return quicksort(0, cnt_);
  }

  /**
   * Return the size of the words in the vocabulary.
   *
   * @return Size in bytes of the words.
   */
  uint size() const {
    return size_;
  }

  /**
   * Returns number of words in the vocabulary.
   *
   * @return Number of words in the vocabulary.
   */
  size_t cnt() const {
    return cnt_;
  }

  ~Vocabulary();

  bool operator==(const Vocabulary &rhs) const;

 private:
  /**
   * Swap words in the vocabulary.
   * @param a Position of the first word.
   * @param b Position of the second word.
   */
  void swap(size_t a, size_t b);

  /**
   * Sort lexicographically the words between positions left and right, and
   * returns the number of differents words.
   * @param left Left position of the subarray.
   * @param right Right position of the subarray.
   * @return Number of differents words.
   */
  size_t quicksort(size_t left, size_t right);

  /** Number of words in the vocabulary*/
  size_t cnt_;
  /** Size in bytes of each word*/
  uint size_;
  /** Array storing words*/
  uchar *data_;
};

}  // namespace compression
}  // namespace libk2tree
#endif  // INCLUDE_COMPRESSION_VOCABULARY_H_
