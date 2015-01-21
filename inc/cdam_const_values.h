#ifndef CDAM_CONST_VALUES_H
#define CDAM_CONST_VALUES_H

namespace cdam
{

#define ASCII_NULL 0x00
// Start of Header byte, ASCII SOH
#define ASCII_SOH  0x01
#define ASCII_STX  0x02
// Word substitution byte
#define ASCII_SUB  0x1A
// Command byte, ASCII ENQ
#define ASCII_ENQ  0x05
// End of Passage byte, ASCII ETX
#define ASCII_ETX  0x03

// Start of Header byte, ASCII SOH
const uint8_t kAsciiHeaderByte = 0x01;
// Command byte, ASCII ENQ
const uint8_t kAsciiCommandByte = 0x05;

const uint8_t kPassageCountSize = 2;
const uint8_t kPassageOffsetSize = 4;
const uint8_t kOperationCountSize = 1;
const uint8_t kOperationInfoSize = 2;
const uint8_t kOperationOperandSize = 2;
const uint8_t kDataLengthSize = 2;
const uint8_t kPassageIndexSize = 2;

const uint8_t kStoryStateEmpty = 0;
const uint8_t kStoryStateNormal = 1;
const uint8_t kStoryStateDeleted = 2;

}

#endif