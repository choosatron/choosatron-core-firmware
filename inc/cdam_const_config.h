#ifndef CDAM_CONST_CONFIG_H
#define CDAM_CONST_CONFIG_H

namespace cdam
{

#define HAS_SD 1

// Seconds for timeout of Published Spark Events
const uint8_t kServerTTLDefault = 60;
// How often to update the Printer state.
const uint16_t kIntervalPrinterMillis = 50;
// How often to update the Keypad state.
const uint16_t kIntervalKeypadMillis = 5;
// How often to update the Coin Acceptor state.
const uint16_t kIntervalCoinAcceptorMillis = 5;
// How often to allow printing Insert Coin message.
const uint16_t kIntervalCoinInsertMessage = 10000;
// Delay until printing out 'press any button' message.
const uint16_t kIntervalPressAnyButton = 30000;
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

/*
		The more max heating dots, the more peak current will cost
	when printing, and the faster printing speed. The max heating dots
	is 8 * (n1 + 1).
		The more heating time, the more density, but the slower printing speed.
	If heating time is too short, blank page may occur.
		The more heating interval, the more clear, but the slower printing speed.
*/
// Printer max heating dots.
const uint8_t kPrinterMaxHeatingDots = 20; // 0 - 255, Unit(8dots), Default: 7 (64 dots), (20 = balance of darkness vs no jams)
// Printer heating time.
const uint8_t kPrinterHeatingTime = 160; // 3 - 255, Unit(10uS), Default: 8 (800uS)
// Printer heating interval.
const uint8_t kPrinterHeatingInterval = 250; // 0 - 255, Unit(10uS), Default: 2 (20uS)
// Printer density - Density is 50% + 5% * n(D4 - D0)
const uint8_t kPrinterDensity = 14; // 120% (? can go higher, text is darker but fuzzy)
// Printer break time - Break time is n(D7-D5) * 250uS
const uint8_t kPrinterBreakTime = 4; // 1000uS
// Columns available for printing.
const uint8_t kPrinterColumns = 32;
const uint16_t kPrinterBaudRate = 19200;

}

#endif