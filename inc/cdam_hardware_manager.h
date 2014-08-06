/**
 ******************************************************************************
 * @file    cdam_data_manager.h
 * @author  Jerry Belich
 * @version V1.0.0
 * @date    30-March-2014
 * @brief   Header for cdam_data_manager.cpp module
 ******************************************************************************
  Copyright (c) 2014 Monkey with a Mustache, LLC.  All rights reserved.
 ******************************************************************************
 *
 * Limitation: Single Threaded Design
 * See: http://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf
 *      For problems associated with locking in multi threaded applications
 *
 * Limitation:
 * If you use this Singleton (A) within a destructor of another Singleton (B)
 * This Singleton (A) must be fully constructed before the constructor of (B)
 * is called.
 *
 */

#ifndef CDAM_HARDWARE_MANAGER_H
#define CDAM_HARDWARE_MANAGER_H

#include "cdam_constants.h"
#include "elapsed_time.h"
#include "cdam_keypad.h"
#include "cdam_printer.h"
#include "cdam_coin_acceptor.h"

namespace cdam
{

// class Keypad;
// class Printer;
// class CoinAcceptor;

class HardwareManager {
    public:
        /* Public Methods */
        HardwareManager();

        bool initialize();

        // Accessors
        Printer* printer();
        Keypad* keypad();
        CoinAcceptor* coinAcceptor();

        // Call once per loop, check intervals.
        void updateIntervalTimers();

        // See if enough time has passed to print again.
        void printCoinInsertIntervalUpdate();

        /* Public Variables */

    private:
        /* Private Methods */

        // Assign the Spark Core pins.
        void setupHardwarePins();
        // Check start byte and load firmware version.
        bool loadFirmwareVersion();
        // Load flags, values, and basic story info.
        bool loadMetadata();
        // Setup hardware communication objects.
        void initHardware();
        // Check the printer elapse time interval, react.
        void printerIntervalUpdate();
        // Check the keypad elapse time interval, react.
        void keypadIntervalUpdate();
        // Check the coin acceptor elapse time interval, react.
        void coinAcceptorIntervalUpdate();

        /* Private Variables */
        Printer* _printer;
        Keypad* _keypad;
        CoinAcceptor* _coinAcceptor;

        ElapsedMillis _printerElapsed;
        ElapsedMillis _keypadElapsed;
        ElapsedMillis _coinElapsed;
        ElapsedMillis _printInsertElapsed;

};

}

#endif