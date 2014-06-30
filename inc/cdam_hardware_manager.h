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

namespace cdam
{

class Keypad;

class HardwareManager
{
    public:
        /* Public Methods */
        static HardwareManager& getInstance()
        {
            // The only instance
            // Guaranteed to be lazy initialized
            // Guaranteed that it will be destroyed correctly
            static HardwareManager instance;
            return instance;
        }

        bool initialize();

        // Accessors
        Keypad* keypad();

        // Call once per loop, check intervals.
        void updateIntervalTimers();

        /* Public Variables */

    private:
        /* Private Methods */
        HardwareManager();
        // Stop the compiler generating methods of copy the object
        HardwareManager(HardwareManager const& copy);            // Not Implemented
        HardwareManager& operator=(HardwareManager const& copy); // Not Implemented

        // Assign the Spark Core pins.
        void setupHardwarePins();
        // Check start byte and load firmware version.
        bool loadFirmwareVersion();
        // Load flags, values, and basic story info.
        bool loadMetadata();
        // Setup hardware communication objects.
        void initHardware();
        // Setup interval timers for loop control.
        void initLoopControl();
        // Check the keypad elapse time interval, react.
        void keypadIntervalUpdate();

        /* Private Variables */
        Keypad *_keypad;
        ElapsedMillis _keypadElapsed;

};

}

#endif