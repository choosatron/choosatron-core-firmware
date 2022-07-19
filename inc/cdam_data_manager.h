/**
 ******************************************************************************
* @file    cdam_data_manager.h
* @author  Jerry Belich
* @version V1.0.7
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
#include "elapsed_time.h"

#if HAS_SD == 1
#include "SdFat.h"
#endif

namespace cdam
{

class StateController;

typedef void (*ConnectCallback)(const char *ssid,
                        const char *password,
                        unsigned long security_type);

class DataManager
{
   public:
      /* Public Methods */
      DataManager();

      bool initialize(StateController *aStateController);
      bool initStorage();
      void logMetadata();
      void handleSerialData();

#if HAS_SD == 1
      bool initSD();
#endif

      // Getters for Flashee filesystems.
      Flashee::FlashDevice* flashMetadata();
      Flashee::FlashDevice* flashStories();

      // Get the page offset the story begins at.
      uint32_t getStoryOffset(uint8_t aIndex, bool aVisibleOnly = true);
      // Get the byte offset for the given passage index (in current story).
      uint32_t getPassageOffset(uint16_t aIndex);
      // Fill provided buffer with the title of the story and the provided position.
      bool getTitle(char* aBuffer, uint8_t aIndex, bool aNumbered);
      // Story selected, load the header for the story in play.
      bool loadStory(uint8_t aIndex);
      // Cleanup after the finished story.
      void unloadStory();
      // Get variable value at index.
      int16_t varAtIndex(uint8_t aIndex);
      // Set variable value at index.
      bool setVarAtIndex(uint8_t aIndex, int16_t aValue);

      /* API Calls */
      void getStoryInfo(char* aBuffer, uint32_t aLength, uint8_t aIndex, StoryHeader *aHeader);
      void sendStoryInfoSerial(uint8_t aIndex);
      // Add the metadata for a new story.
      bool addStoryMetadata(uint8_t aIndex, uint8_t aPages);
      bool removeStoryMetadata(uint8_t aIndex);
      bool removeAllStoryData();
      // Set and write a flag.
      bool setFlag(uint8_t aFlagIndex, uint8_t aBitIndex, bool aValue);
      bool setValue(uint8_t aIndex, uint16_t aValue);
      // Resets metadata to default values.
      bool resetMetadata();
      // Erase entire flash memory, includes metadata.
      bool eraseFlash();

      uint8_t readByte(uint32_t aAddress);
      bool readData(void* aBuffer, uint32_t aAddress, uint32_t aLength);

      void writeBegin(uint32_t aAddress, uint32_t aBinarySize);
      void writeEnd(bool aResult);
      uint16_t writeData(void* aBuffer, uint32_t aLength);
      bool writeData(void* aBuffer, uint32_t aAddress, uint32_t aLength);

      StateController* stateController();
      //Flashee::FlashDevice* storyFlash();

      /* Public Variables */

      // For YModem data writes, go through Flashee filesystem or raw?
      bool writeToFlashee;
      // Set which flash space target to write to / read from.
      uint8_t flashTarget; // 1 = metadata, 2 = stories, 3 = saves
      // For YModem writes to track successful writes.
      uint16_t writeIndex;
      // Send all printer data to serial instead.
      bool logPrint;
      // Whether or not the state machine should execute it's update loop.
      bool runState;
      // Did we start with WiFi credentials?
      bool hasCredentials;
      // Live story order.
      uint8_t liveStoryOrder[kMaxRandStoryCount];
      // Live story count. If not in random mode, can be max of 10.
      uint8_t liveStoryCount;
      // The hard-coded firmware version
      Version firmwareVersion;
      Metadata metadata;
      StoryHeader storyHeader;

      // Current story is being played randomly.
      bool randomPlay;
      // The index of the story currently in play.
      int8_t currentStory;
      // The number of points accumulated in the current story.
      int16_t points;
      // The number of turns taken this story.
      uint16_t turns;
      // Passage count for current story.
      uint16_t psgCount;
      // Story passage table of contents memory offset.
      uint32_t tocOffset;
      // Story data memory offset.
      uint32_t startOffset;
      // Current passage index.
      uint16_t psgIndex;
      // Current passage byte size.
      uint32_t psgSize;

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
      bool readStoryHeader(uint8_t aIndex, StoryHeader *aHeader);
      // Read variable counts and default values.
      //bool readVariables(uint8_t aIndex);
      // Write metadata to flash.
      bool writeMetadata(Metadata *aMetadata);
      // Write the storyCount and usedStoryBytes to flash.
      bool writeStoryCountData(Metadata *aMetadata);

      // Check if firmware has updated.
      bool didFirmwareUpdate(Metadata *aMetadata);
      // Upgrade data since firmware is newer than the data.
      bool upgradeDataModels();

      // Test Methods
      //void setTestMetadata(Metadata *aMetadata);
      //void logBinary(uint8_t aValue);
      //void logStoryOffsets(Metadata *aMetadata);
      //void logStoryBytes(Metadata *aMetadata);
      // For debugging: print out all the metadata values.
      //void logMetadata(Metadata *aMetadata);
      //void logStoryHeader(StoryHeader *aHeader);


      /* Private Variables */
      ElapsedMillis _serialElapsed;
      ElapsedMillis _serialTimeout;
      StateController* _stateController;
      Flashee::FlashDevice* _metaFlash;
      Flashee::FlashDevice* _storyFlash;
      bool _writeInProgress;
      uint32_t _currentAddress;
      uint32_t _binarySize;
#if HAS_SD == 1
      SdFile* _storyFile;
      Sd2Card* _card;
      SdVolume* _volume;
      SdFile* _root;
#endif
      int16_t* _variables;
};

}

#endif