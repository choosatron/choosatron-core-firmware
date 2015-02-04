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

/*
 *    Converts a UUID from binary representation to a string representation.
 */

/*void Utils::uuidToString(const Uuid *uuid, char *str, size_t strsz)
{
	snprintf(str, strsz,
	     "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
	     uuid->time_low,
	     uuid->time_mid,
	     uuid->time_hi_and_version,
	     (unsigned char)uuid->clock_seq_hi_and_reserved,
	     (unsigned char)uuid->clock_seq_low,
	     (unsigned char)uuid->node[0],
	     (unsigned char)uuid->node[1],
	     (unsigned char)uuid->node[2],
	     (unsigned char)uuid->node[3],
	     (unsigned char)uuid->node[4],
	     (unsigned char)uuid->node[5]);
}*/

/*uint32_t bytesToLong(sint8_t c0, sint8_t c1, sint8_t c2, sint8_t c3) {
    return Utils::bytesToLong((uint8_t)c0, (uint8_t)c1, (uint8_t)c2, (uint8_t)c3);
}*/


// base 64 - Convert from a binary blob to a string.
/*const char Utils::kEncodingTable[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
										'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
										'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
										'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
										'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
										'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
										'w', 'x', 'y', 'z', '0', '1', '2', '3',
										'4', '5', '6', '7', '8', '9', '+', '/'};

const int Utils::kModTable[3] = {0, 2, 1};


void Utils::base64Encode(char *aData, size_t aInputLength, char *aEncodedData) {
	// From http://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c

	int outputLength = ((aInputLength - 1) / 3) * 4 + 4;
	aEncodedData[outputLength] = 0;

	for (unsigned int i = 0, j = 0; i < aInputLength;) {
		unsigned int octetA = i < aInputLength ? (unsigned char)aData[i++] : 0;
		unsigned int octetB = i < aInputLength ? (unsigned char)aData[i++] : 0;
		unsigned int octetC = i < aInputLength ? (unsigned char)aData[i++] : 0;

		unsigned int triple = (octetA << 0x10) + (octetB << 0x08) + octetC;

		aEncodedData[j++] = Utils::kEncodingTable[(triple >> 3 * 6) & 0x3F];
		aEncodedData[j++] = Utils::kEncodingTable[(triple >> 2 * 6) & 0x3F];
		aEncodedData[j++] = Utils::kEncodingTable[(triple >> 1 * 6) & 0x3F];
		aEncodedData[j++] = Utils::kEncodingTable[(triple >> 0 * 6) & 0x3F];
	}

	for (int i = 0; i < Utils::kModTable[aInputLength % 3]; i++)
		aEncodedData[outputLength - 1 - i] = '=';
	}
}*/

}