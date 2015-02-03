
#ifndef CDAM_UTILS_H
#define CDAM_UTILS_H

#include "cdam_constants.h"
#include <stdio.h>

namespace cdam
{

class Utils {

public:
	//static const char kEncodingTable[64];
	//static const int kModTable[3];

	static void shuffle(uint8_t *aArray, uint8_t aElements);
	//static void shuffle(void *aArray, size_t aElements, size_t aArraySize);
	static void uuidToString(const Uuid *uuid, char *str, size_t strsz);
	//static void base64Encode(char *aData, size_t aInputLength, char *aEncodedData);

};

}

#endif