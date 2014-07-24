#ifndef CDAM_CONST_CONFIG_H
#define CDAM_CONST_CONFIG_H

namespace cdam
{

// Seconds for timeout of Published Spark Events
const uint8_t kServerTTLDefault = 60;
// How often to update the Printer state.
const unsigned int kIntervalPrinterMillis = 50;
// How often to update the Keypad state.
const unsigned int kIntervalKeypadMillis = 5;
// How often to update the Coin Acceptor state.
const unsigned int kIntervalCoinAcceptorMillis = 5;
// Maximum bytes available for stories. (1MB)
const uint32_t kFlashMaxStoryBytes = 1048576;


// These are from the OLD way - maybe not needed.
const uint8_t kPrinterColumns = 32;
const uint16_t kMaxPassageBytes = 279;//289;

}

#endif