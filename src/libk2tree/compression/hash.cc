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

HashTable::HashTable(uint sizeVoc, double occup_hash)
    : TAM_HASH(NearestPrime(occup_hash * sizeVoc)),
      NumElem(0),
      hash() {
  if (TAM_HASH <= JUMP)
    TAM_HASH = NearestPrime(JUMP+1);
  hash.resize(TAM_HASH);
}

uint HashTable::add(const uchar *aWord,
                    uint len,
                    uint addr) {
  if (addr == TAM_HASH) {
    printf("Not enough memory, vocabulary exceeds maximun size !\n");
    exit(1);
  }

  hash[addr].word = aWord;
  hash[addr].len = len;
  hash[addr].weight = 1;
  NumElem++;

  return addr;
}
bool HashTable::search(const uchar *aWord, unsigned len,
                       uint *returnedAddr) const {
  uint addr;
  addr = hashFunction(aWord, len);

  while ((hash[addr].word != NULL) &&
        (strcomp(hash[addr].word, aWord, hash[addr].len, len) != 0))
    addr = (addr + JUMP) % TAM_HASH;
  // position returned
  *returnedAddr = addr;

  // Word was not found
  if (hash[addr].word  == NULL) {
    return false;
  }
  // Word was found
  return true;
}



/*------------------------------------------------------------------
 Modification of Zobel's bitwise function to have into account the 
 lenght of the key explicetely 
 ---------------------------------------------------------------- */
uint HashTable::hashFunction(const uchar *aWord, uint len) const {
  char c;
  uint h;

  h = SEED;

  uint i = 0;
  while (i++ < len) {
    c = *(aWord++);
    h ^= ( (h << 5) + c + (h >> 2) );
  }
  return((uint)((h&0x7fffffff) % TAM_HASH));
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


