
#ifndef CDAM_UTILS_H
#define CDAM_UTILS_H

#include "cdam_constants.h"
#include <stdio.h>

namespace cdam
{

class Utils {

public:
	static void shuffle(uint8_t *aArray, uint8_t aElements);
	//static void shuffle(void *aArray, size_t aElements, size_t aArraySize);

};

}

#endif