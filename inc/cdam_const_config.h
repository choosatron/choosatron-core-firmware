#ifndef CDAM_CONST_CONFIG_H
#define CDAM_CONST_CONFIG_H

namespace cdam
{

#define HAS_SD 1

// Seconds for timeout of Published Spark Events
const uint8_t kServerTTLDefault = 60;
// How often to update the Printer state.
const unsigned int kIntervalPrinterMillis = 50;
// How often to update the Keypad state.
const unsigned int kIntervalKeypadMillis = 5;
// How often to update the Coin Acceptor state.
const unsigned int kIntervalCoinAcceptorMillis = 5;
// How often to allow printing Insert Coin message.
const unsigned int kIntervalCoinInsertMessage = 10000;
// Delay until printing out 'press any button' message.
const unsigned int kIntervalPressAnyButton = 20000;
// Min value for coin sensing.
const uint8_t kCoinAcceptorSenseMin = 15;
// Max value for coin sensing.
const uint8_t kCoinAcceptorSenseMax = 60;
// Maximum bytes available for stories. (1MB)
const uint32_t kFlashMaxStoryBytes = 1048576;
// Passage buffer read size.
const uint16_t kPassageBufferReadSize = 496;
// Maximum stories allowed.
const uint8_t kMaxStoryCount = 10;
// Maximum stories allowed when in random mode.
const uint8_t kMaxRandStoryCount = 20;
// Columns available for printing.
const uint8_t kPrinterColumns = 32;

}

#endif