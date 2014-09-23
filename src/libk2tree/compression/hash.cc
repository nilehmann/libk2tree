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



#include <compression/hash.h>


namespace libk2tree {
namespace compression {
using utils::NearestPrime;
using std::vector;

HashTable::HashTable(size_t sizeVoc, double occup_hash)
    : tam_hash_(NearestPrime((size_t) (occup_hash * sizeVoc))),
      num_elem_(0),
      hash_() {
  if (tam_hash_ <= JUMP)
    tam_hash_ = NearestPrime(JUMP+1);
  hash_.resize(tam_hash_);
}

size_t HashTable::add(const uchar *aWord,
                      uint len,
                      size_t addr) {
  if (addr == tam_hash_) {
    printf("Not enough memory, vocabulary exceeds maximun size !\n");
    exit(1);
  }

  hash_[addr].word = aWord;
  hash_[addr].len = len;
  hash_[addr].weight = 1;
  num_elem_++;

  return addr;
}
bool HashTable::search(const uchar *aWord,
                       uint len,
                       size_t *returnedAddr) const {
  size_t addr;
  addr = hashFunction(aWord, len);

  while ((hash_[addr].word != NULL) &&
        (strcomp(hash_[addr].word, aWord, hash_[addr].len, len) != 0))
    addr = (addr + JUMP) % tam_hash_;
  // position returned
  *returnedAddr = addr;

  // Word was not found
  if (hash_[addr].word  == NULL)
    return false;
  // Word was found
  return true;
}



/*------------------------------------------------------------------
 Modification of Zobel's bitwise function to have into account the 
 lenght of the key explicetely 
 ---------------------------------------------------------------- */
size_t HashTable::hashFunction(const uchar *aWord, uint len) const {
  uchar c;
  size_t h;

  h = SEED;

  uint i = 0;
  while (i++ < len) {
    c = *(aWord++);
    h ^= ( (h << 5) + c + (h >> 2) );
  }
  return (h&0x7fffffff) % tam_hash_;
}
/*------------------------------------------------------------------
 Modification of Zobel's scmp function compare two strings
 ---------------------------------------------------------------- */
int HashTable::strcomp(const uchar *s1, const uchar *s2,
                       uint ws1, uint ws2) const {
  if (ws1 !=ws2)
    return -1;

  uint iters;
  iters = 0;
  while (iters < ws1-1 && *s1 == *s2) {
    s1++;
    s2++;
    iters++;
  }
  return( *s1-*s2 );
}

}  // namespace compression
}  // namespace libk2tree


