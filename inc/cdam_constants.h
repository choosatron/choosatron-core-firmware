// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2013 Monkey with a Mustache Entertainment, Inc.
// All rights reserved.

#ifndef CDAM_CONSTANTS_H
#define CDAM_CONSTANTS_H

#include "spark_wiring_string.h"
#include "spark_wiring_usbserial.h"

// Instead of "Arduino.h"
#include <stdint.h>

typedef unsigned int word;
typedef const unsigned char prog_uchar;

// Define if you want NO WLAN or Cloud
#define OFFLINE 1

// Which version of the board is this for?
//#define CHOOSATRON_2DOT0 1
//#define CHOOSATRON_2DOT3 1
#define CHOOSATRON_2DOT6 1

// Serial Debugging - NOTE: Pins 0 & 1 are NOT available with debugging on.
#define WRITE_EEPROM 0
#define PRINT 1
#define DEBUG 1
#define MEMORY 0 // Only prints if DEBUG is also on.
#define CONTINUES 0 // 0 = off, 1 = on
#define MAX_JUMP_BACK 4
#define LOG_PLAY 0
#define COIN_MODE 0 // 0 = off, 1 = on, 2 = on with continues

static prog_uchar STATE_WAIT[] = "State: WAIT";
static prog_uchar STATE_SELECT[] = "State: SELECT";
static prog_uchar STATE_PLAY[] = "State: PLAY";
static prog_uchar STATE_CONTINUE[] = "State: CONTINUE";
static prog_uchar STATE_AUTH[] = "State: AUTH";
static prog_uchar STATE_ADMIN[] = "State: ADMIN";

static const char CDAM_TITLE[] = "The Choosatron";
static const char CDAM_SUBTITLE[] = "Deluxe Adventure Matrix v1.3.1\n\nby Jerry Belich - choosatron.com\nChoose your story below!\n";
static const char CDAM_INTRO[] = "by Jerry Belich - choosatron.com\n\nChoose your story below!\n";
static const char CDAM_LOGO[] = "LOGO DATA";
static const char INSERT_COINS[] = "Credits 0 - Insert Coin(s)\n";

static const unsigned int PRINT_COLUMNS = 32;
static const unsigned long MAX_PASSAGE_BYTES = 279;//289;

static const char RESET_PASS = '6';

#define PIEZO_PIN A0
#define RED_PIN A1
#define GREEN_PIN A6
#define BLUE_PIN A7

typedef enum {LED_NOT_READY, LED_READY, LED_INPUT, LED_PRINTING, LED_ERROR} LED_STATE; 

//#define BAUD_RATE 9600
#define BAUD_RATE 19200
// Milliseconds between "Please Insert Coin" response.
#define INSERT_COIN_DELAY 10000
// Value in cents of currently accepted coins.
#define COIN_VALUE 0.05
// Cost to play a new game.
#define COINS_PER_CREDIT 1
// Cost to continue a game.
// NEVER HAVE LARGER THAN COINS_PER_CREDIT
#define COINS_TO_CONTINUE 1
// Milliseconds before continue opportunity is lost.
static const unsigned long TIME_TO_CONTINUE = 60000;
// No key read.
static const char ANALOGUE_NO_KEY = '\0';

// Default Gameplay Log File
//#define PLAY_LOG "PLAY_LOG.TXT"

// Application States
typedef enum {WAIT_STATE, SELECT_STATE, PLAY_STATE, CONTINUE_STATE, AUTH_STATE, ADMIN_STATE, ERROR_STATE} GameState;
// WAIT: No game in progress, waiting for credits.
// SELECT: Credits accepted, selecting game.
// PLAY: Game in play.
// CONTINUE: Game end state, waiting for continue or reset.
// AUTH: Waiting for admin authorization.
// ADMIN: Admin state.

// Coin Acceptor Pin
#define COIN_PIN D6

// Helper for Debug Logging
#if (DEBUG == 1)
#define swrite(M) \
Serial.write(M)
#define slog(M) \
Serial.print(M)
#define slogln(M) \
Serial.println(M)
#else
#define swrite(M)
#define slog(M)
#define slogln(M)
#endif

#endif
