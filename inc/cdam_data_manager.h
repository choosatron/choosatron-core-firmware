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

namespace cdam
{

typedef struct Version_t
{
	uint8_t major;
	uint8_t minor;
	uint8_t revision;
} Version;

typedef struct GlobalFlags_t {
	union {
	    uint8_t flag1;
	    struct {
	        uint8_t offline			:1;
	        uint8_t demo			:1;
	        uint8_t sdCard			:1;
	        uint8_t multiPlayer		:1;
	        uint8_t arcade			:1;
	    	uint8_t rsvd 			:3;
	    };
	};
	union {
		uint8_t flag2;
		struct {
			uint8_t logging 		:1;
	        uint8_t logLocation		:1;
	        uint8_t logLive			:1;
	        uint8_t rsvd 			:5;
		};
    };
	union {
		uint8_t flag3;
		struct {
	        uint8_t dictOffsetBytes	:1;		// 0=2bytes, 1=3bytes
	        uint8_t rsvd 			:7;
		};
    };
    union {
		uint8_t flag4;
		struct {
	        uint8_t rsvd 			:8;
		};
    };
    union {
		uint8_t flag5;
		struct {
	        uint8_t rsvd 			:8;
		};
    };
    union {
		uint8_t flag6;
		struct {
	        uint8_t rsvd 			:8;
		};
    };
    union {
		uint8_t flag7;
		struct {
	        uint8_t rsvd 			:8;
		};
    };
    union {
		uint8_t flag8;
		struct {
	        uint8_t rsvd 			:8;
		};
    };
} GlobalFlags;

class DataManager
{
    private:
        // Private Constructor
        DataManager();
        // Stop the compiler generating methods of copy the object
        DataManager(DataManager const& copy);            // Not Implemented
        DataManager& operator=(DataManager const& copy); // Not Implemented

    public:
        static DataManager& getInstance()
        {
            // The only instance
            // Guaranteed to be lazy initialized
            // Guaranteed that it will be destroyed correctly
            static DataManager instance;
            return instance;
        }

        bool initilize();

        // Check start byte and load firmware version.
        bool loadFirmwareVersion(*aError);
        // Load flags, values, and basic story info.
        bool loadMetaData(*aError);

        String firmwareVersion();
};

}

#endif