#include "cdam_utils.h"

namespace cdam
{

void Utils::shuffle(uint8_t *aArray, uint8_t aElements)
{
    if (aElements > 1)
    {
        uint8_t i;
        for (i = 0; i < aElements - 1; i++)
        {
          uint8_t j = i + rand() / (RAND_MAX / (aElements - i) + 1);
          uint8_t t = aArray[j];
          aArray[j] = aArray[i];
          aArray[i] = t;
        }
    }
}

/*void Utils::shuffle(void *aArray, size_t aElements, size_t aArraySize) {
	// This if() is not needed functionally, but left per OP's style
	if (aElements > 1) {
		char *carray = (char *)aArray;
		void * aux;
		aux = malloc(aArraySize);
		size_t i;
		for (i = 1; i < aElements; ++i) {
			size_t j = rand() % (i + 1);
			j *= aArraySize;
			memcpy(aux, &carray[j], aArraySize);
			memcpy(&carray[j], &carray[i * aArraySize], aArraySize);
			memcpy(&carray[i * aArraySize], aux, aArraySize);
		}
		free(aux);
	}
}*/

}