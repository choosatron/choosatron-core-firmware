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

// TODO: Start at 'a' or 0x61
const uint8_t kSerialCmdWriteFlashRaw = 0x66; // 'f'
const uint8_t kSerialCmdWriteFlashee = 0x03;
const uint8_t kSerialCmdKeypadInput = 0x04;
const uint8_t kSerialCmdButtonInput = 0x05;
const uint8_t kSerialCmdAdjustCredits = 0x06;
const uint8_t kSerialCmdSetCredits = 0x07;
const uint8_t kSerialCmdAddStory = 0x08;
const uint8_t kSerialCmdRemoveStory = 0x09;
const uint8_t kSerialCmdRemoveAllStories = 0x0A;
const uint8_t kSerialCmdMoveStory = 0x0B;
const uint8_t kSerialCmdSetFlag = 0x0C;
const uint8_t kSerialCmdSetValue = 0x0D;
const uint8_t kSerialCmdResetMetadata = 0x0E;
const uint8_t kSerialCmdEraseFlash = 0x0F;
const uint8_t kSerialCmdRebootUnit = 0x10;

const uint8_t kSerialCmdGetVersion = 0x11;
const uint8_t kSerialCmdGetFlag = 0x12;
const uint8_t kSerialCmdGetValue = 0x13;
const uint8_t kSerialCmdGetNames = 0x14;
const uint8_t kSerialCmdGetStoryInfo = 0x15;

// Flashee storage types.
const uint8_t kFlashMetadataType = 1;
const uint8_t kFlashStoriesType = 2;
const uint8_t kFlashSavesType = 3;

}

#endif