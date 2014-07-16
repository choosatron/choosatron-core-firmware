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

#ifndef CDAM_DATA_MANAGER_H
#define CDAM_DATA_MANAGER_H

#include "cdam_constants.h"
#include "flashee-eeprom.h"


namespace cdam
{

const uint32_t kFlashMaxStoryBytes = 1048576;

// Game States
typedef enum GameState_t {
    STATE_ERROR,
    STATE_INIT,
    STATE_READY,
    STATE_SELECT,
    STATE_PLAY,
    STATE_CONTINUE,
    STATE_AUTH,
    STATE_ADMIN
} GameState;

class DataManager
{
    public:
        /* Public Methods */
        DataManager();

        bool initialize();
        const char* gameStateStr();

        Flashee::FlashDevice* storyFlash();

        /* Public Variables */
        // The hard-coded firmware version
        Version firmwareVersion;
        Metadata metadata;
        std::vector<StoryHeader> storyHeaders;


        GameState gameState;
        uint32_t usedStoryBytes;
        uint8_t gameCredits;

    private:
        /* Private Methods */

        // Check start byte and load firmware version.
        void loadFirmwareVersion();
        // Load flags, values, and basic story info.
        bool loadMetadata();
        // Set the default metadata and save it.
        bool initializeMetadata(Metadata *aMetadata);

        // Read metadata from flash.
        bool readMetadata(Metadata *aMetadata);
        // Write metadata to flash.
        bool writeMetadata(Metadata *aMetadata);

        // Check if firmware has updated.
        bool didFirmwareUpdate(Metadata *aMetadata);
        // Upgrade data since firmware is newer than the data.
        bool upgradeDataModels();

        // Test Methods
        bool testMetadata();
        void setTestMetadata(Metadata *aMetadata);
        void logBinary(uint8_t aValue);
        // For debugging: print out all the metadata values.
        void logMetadata(Metadata *aMetadata);




        /* Private Variables */
        Flashee::FlashDevice* _metaFlash;
        Flashee::FlashDevice* _storyFlash;

};

}

#endif