/* Searches using  Horspool's algorithm adapted to 
search inside a text compressed with End-Tagged Dense Code.
Lightweight Natural Language Text Compression: Information Retrieval 2006

Programmed by Antonio Fariña.

Author's contact: Antonio Fariña, Databases Lab, University of
A Coruña. Campus de Elviña s/n. Spain  fari@udc.es

Copyright (C) 2006  Nieves R. Brisaboa, Gonzalo Navarro, Antonio Fariña and José R. Paramá
Author's contact: antonio.fari@gmail.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
aint with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


/*-----------------------------------------------------------------------
 Hash.h: Definition of a HashTable that uses linear hash
 ------------------------------------------------------------------------*/

#ifndef INCLUDE_COMPRESSION_HASH_H_
#define INCLUDE_COMPRESSION_HASH_H_

#include <libk2tree_basic.h>
#include <utils/utils.h>
#include <cstdio>
#include <vector>

// jump done when a collision appears
#define JUMP 101
// a small prime number, used to compute a hash function
#define SMALL_PRIME 1009
#define SEED 1159241
/* Type definitions */


namespace libk2tree {
namespace compression {
using utils::NearestPrime;

struct Nword {
  Nword() : word(NULL), len(0), weight(0), codeword(0) {}
  const uchar *word;
  uint len;
  uint weight;
  uint codeword;
};

class HashTable {
 public:
  HashTable(size_t sizeVoc, double occup_hash= 1.5);

  /**
   * Adds a word to the hash in the specific addres.
   *
   * @param aWord Word
   * @param len Length of the word
   * @param addr Position in the table to store the word.
   * @return Position in the table where the word has stored.
   */
  size_t add(const uchar *aWord, uint len, size_t addr);
  /**
   * Search a word in the table. After return returnedAddr hold an address in
   * the table. In case the word is found, the address correspond to the word
   * position. In case the word is not present, the address holds the position
   * where to store the word
   *
   * @param aWord Word
   * @param len Length of the word
   * @param returnedAddr Pointer to store the address.
   * @return Ture in case the word is present and false otherwise.
   */
  bool search(const uchar *aWord, uint len, size_t *returnedAddr) const;

  Nword &operator[](size_t i) {
    return hash_[i];
  }

  const Nword &operator[](size_t i) const {
    return hash_[i];
  }

  size_t num_elem() {
    return num_elem_;
  }

 private:
  /**  entries in the hash table.*/
  size_t tam_hash_;
  /** elements already added to the hash table.*/
  size_t num_elem_;
  /** holds a hashTable of words*/
  std::vector<Nword> hash_;

  /*------------------------------------------------------------------
   Modification of Zobel's bitwise function to have into account the 
   lenght of the key explicetely 
   ---------------------------------------------------------------- */
  size_t hashFunction(const uchar *aWord, uint len) const;

  /*------------------------------------------------------------------
   Modification of Zobel's scmp function compare two strings
   ---------------------------------------------------------------- */
  inline int strcomp(const uchar *s1, const uchar *s2,
                     uint ws1, uint ws2) const;
};

}  // namespace compression
}  // namespace libk2tree

#endif  // INCLUDE_COMPRESSION_HASH_H_
