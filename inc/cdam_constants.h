
#ifndef CDAM_CONSTANTS_H
#define CDAM_CONSTANTS_H

#define BIZ_CARD true

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
#define BOARD_DOT_VERSION 7

#define BAUD_RATE 9600

// BINARY SPEC VERSION REQUIRED: 1.0.7

const uint8_t kFirmwareVersionMajor = 1;
const uint8_t kFirmwareVersionMinor = 0;
const uint8_t kFirmwareVersionRevision = 7;

typedef enum {
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