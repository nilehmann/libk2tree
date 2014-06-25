
#ifndef DACS_DACS_H_
#define DACS_DACS_H_

extern "C" {

typedef unsigned int uint;

struct sFTRep;
typedef struct sFTRep FTRep;


// public:
	FTRep* createFT(uint *list,uint listLength);
	uint accessFT(FTRep * listRep,uint param);
	void saveFT(FTRep * listRep, FILE * flist);
	uint * decompressFT(FTRep * listRep, uint n);
	FTRep* loadFT(FILE * flist);
	void destroyFT(FTRep * listRep);
}
#include <fstream>

void SaveFT(std::ofstream *out, FTRep *rep);
FTRep *LoadFT(std::ifstream *in);
bool equalsFT(FTRep *lhs, FTRep *rhs);
#endif  // DACS_DACS_H_
