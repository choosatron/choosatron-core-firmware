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
 **/

#ifndef CDAM_DATA_MANAGER_H
#define CDAM_DATA_MANAGER_H

#include "cdam_constants.h"
#include "cdam_state_controller.h"
#include "flashee-eeprom.h"

namespace cdam
{

class StateController;

class DataManager
{
    public:
        /* Public Methods */
        DataManager();

        bool initialize(StateController *aStateController);

        // Get the page offset the story begins at.
        uint32_t getStoryOffset(uint8_t aIndex);
        // Fill provided buffer with the title of the story and the provided position.
        bool getNumberedTitle(char* aBuffer, uint8_t aIndex);
        // Story selected, load the header for the story in play.
        bool loadStoryHeader(uint8_t aIndex);
        // Add the metadata for a new story.
        bool addStoryMetadata(uint8_t aPosition, uint8_t aPages);
        //bool removeStoryMetadata(uint8_t aPosition);
        bool removeAllStoryData();
        // Set and write a flag.
        bool setFlag(uint8_t aFlagIndex, uint8_t aBitIndex, bool aValue);
        // Resets metadata to default values.
        bool resetMetadata();
        // Erase entire flash memory, includes metadata.
        bool eraseFlash();

        bool readData(char* aBuffer, uint32_t aAddress, uint32_t aLength);
        bool writeData(char* aBuffer, uint32_t aAddress, uint32_t aLength);

        StateController* stateController();
        Flashee::FlashDevice* storyFlash();

        /* Public Variables */

        // Whether or not the state machine should execute it's update loop.
        bool runState;
        // Live story order.
        uint8_t liveStoryOrder[kMaxStoryCount];
        // The hard-coded firmware version
        Version firmwareVersion;
        Metadata metadata;
        StoryHeader storyHeader;
        // The index of the story currently in play.
        int8_t currentStory;
        // The number of points accumulated in the current story.
        int16_t points;

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
        // Read story header from flash.
        bool readStoryHeader(StoryHeader *aHeader, uint8_t aIndex);
        // Write metadata to flash.
        bool writeMetadata(Metadata *aMetadata);
        // Write the storyCount and usedStoryBytes to flash.
        bool writeStoryCountData(Metadata *aMetadata);

        // Check if firmware has updated.
        bool didFirmwareUpdate(Metadata *aMetadata);
        // Upgrade data since firmware is newer than the data.
        bool upgradeDataModels();

        // Test Methods
        void testMetadata();
        void setTestMetadata(Metadata *aMetadata);
        void logBinary(uint8_t aValue);
        void logStoryOffsets(Metadata *aMetadata);
        void logStoryBytes(Metadata *aMetadata);
        // For debugging: print out all the metadata values.
        void logMetadata(Metadata *aMetadata);
        void logStoryHeader(StoryHeader *aHeader);


        /* Private Variables */
        StateController* _stateControl;
        Flashee::FlashDevice* _metaFlash;
        Flashee::FlashDevice* _storyFlash;

};

}

#endif