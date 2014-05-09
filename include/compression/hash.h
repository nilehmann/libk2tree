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
#include <bits/utils/utils.h>
#include <cstdio>
#include <vector>


#define JUMP 101  		 //jump done when a collision appears
#define OCUP_HASH 1.5	 	 //index of occupation of the hash table
#define SMALL_PRIME 1009 // a small prime number, used to compute a hash function
#define SEED	1159241
/* Type definitions */



namespace libk2tree {
namespace compression {
using utils::NearestPrime;
using std::vector;

struct Nword {
  Nword() : word(NULL), len(0), weight(0), codeword(0) {}
  const uchar *word;
  uint len;
  uint weight;
  uint codeword;
};

class HashTable {
 public:
	
  HashTable (uint sizeVoc);
  uint add (const uchar *aWord, uint len, uint addr);
  bool search (const uchar *aWord, uint len, uint *returnedAddr) const;

  Nword  &operator[](uint i) {
    return hash[i];
  }
  
  const Nword &operator[](uint i) const {
    return hash[i];
  }

  uint numEleme() {
    return NumElem;
  }

 private:
  //  entries in the hash table.
  uint TAM_HASH;
  // elements already added to the hash table.
  uint NumElem;
  // holds a hashTable of words
  vector<Nword> hash;

  /*------------------------------------------------------------------
   Modification of Zobel's bitwise function to have into account the 
   lenght of the key explicetely 
   ---------------------------------------------------------------- */
  uint hashFunction (const uchar *aWord, uint len) const;

  /*------------------------------------------------------------------
   Modification of Zobel's scmp function compare two strings
   ---------------------------------------------------------------- */
  inline int strcomp(const uchar *s1, const uchar *s2,
                     uint ws1, uint ws2) const;
  
};

}  // namespace compression
}  // namespace libk2tree

#endif  // INCLUDE_COMPRESSION_HASH_H_
