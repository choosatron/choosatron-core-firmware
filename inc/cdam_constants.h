
#ifndef CDAM_CONSTANTS_H
#define CDAM_CONSTANTS_H

#include "debug.h"
#include "spark_wiring.h"
#include "spark_wiring_usbserial.h"
#include "cdam_errors.h"
#include "cdam_const_config.h"
#include "cdam_const_structs.h"
#include "cdam_const_values.h"
#include "cdam_const_strings.h"
#include "cdam_utils.h"

namespace cdam
{

#define IsBitSet(val, bit) ((val) & (1 << (bit)) ? true : false)

// 0, 3, or 6 are cutoffs for rerouted buttons pins.
#define BOARD_DOT_VERSION 6

#define BAUD_RATE 19200

const uint8_t kFirmwareVersionMajor = 0;
const uint8_t kFirmwareVersionMinor = 0;
const uint8_t kFirmwareVersionRevision = 1;

typedef enum {
#if (BOARD_DOT_VERSION >= 6)
	PIN_BTN_ONE = D1,
	PIN_BTN_TWO = D2,
	PIN_BTN_THREE = D3,
	PIN_BTN_FOUR = D4,
#elif (BOARD_DOT_VERSION >= 3)
	PIN_BTN_ONE = D2,
	PIN_BTN_TWO = D3,
	PIN_BTN_THREE = D4,
	PIN_BTN_FOUR = D5,
#elif (BOARD_DOT_VERSION == 0)
	PIN_BTN_ONE = D4,
	PIN_BTN_TWO = D5,
	PIN_BTN_THREE = D2,
	PIN_BTN_FOUR = D3,
#endif
	PIN_PIEZO = A0,
	PIN_RED_LED = A1,
	PIN_GREEN_LED = A6,
	PIN_BLUE_LED = A7,
	PIN_COIN = D6,
	PIN_SD_CD = D7,
	PIN_SD_SCK = A3,
	PIN_SD_MISO = A4, // SD_DO
	PIN_SD_MOSI = A5, // SD_DI
	PIN_SD_CS = A2
} HardwarePin;

}

#endif