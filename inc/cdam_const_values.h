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


// Serial Communication Commands

const uint8_t kSerialCmdListeningMode = 0x77; // 'w'
const uint8_t kSerialCmdClearWiFi = 0x78; // 'x'

const uint8_t kSerialCmdWriteFlashRaw = 0x79; // 'y'
const uint8_t kSerialCmdWriteFlashee = 0x7A; // 'z'

const uint8_t kSerialCmdKeypadInput = 0x04;
const uint8_t kSerialCmdButtonInput = 0x05;
const uint8_t kSerialCmdAdjustCredits = 0x06;
const uint8_t kSerialCmdSetCredits = 0x07;
const uint8_t kSerialCmdRemoveStory = 0x08;
const uint8_t kSerialCmdRemoveAllStories = 0x09;
const uint8_t kSerialCmdSwapStoryPositions = 0x0A;
const uint8_t kSerialCmdSetFlag = 0x0B;
const uint8_t kSerialCmdSetValue = 0x0C;
const uint8_t kSerialCmdAddStory = 0x0D;


/*const uint8_t kSerialCmdResetMetadata
const uint8_t kSerialCmdEraseFlash
const uint8_t kSerialCmdResetUnit

const uint8_t kSerialCmdGetVersion*/


}

#endif