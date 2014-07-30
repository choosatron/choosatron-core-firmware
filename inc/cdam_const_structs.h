#ifndef CDAM_CONST_STRUCTS_H
#define CDAM_CONST_STRUCTS_H

#include <vector>

namespace cdam
{

/*#define METADATA_BASE_ADDRESS       0
#define METADATA_FIRMWARE_OFFSET    1
#define METADATA_FLAGS_OFFSET       (METADATA_FIRMWARE_OFFSET+3)
#define METADATA_FLAGS_COUNT        8 // bytes
#define METADATA_VALUES_OFFSET      (METADATA_FLAGS_OFFSET + METADATA_FLAGS_COUNT)
#define METADATA_VALUES_COUNT       16 // bytes
#define METADATA_STORYCOUNT_OFFSET  (METADATA_VALUES_OFFSET + METADATA_VALUES_COUNT)
#define METADATA_STORYSIZES_OFFSET  (METADATA_STORYCOUNT_OFFSET + 1)*/

const uint8_t  kMaxStoryCount = 10;

const uint16_t kMetadataBaseAddress = 0;
const uint16_t kMetadataFirmwareOffset = 1; // adding SOH byte
const uint16_t kMetadataFirmwareSize = 3;
const uint16_t kMetadataFlagsOffset = (kMetadataFirmwareOffset + kMetadataFirmwareSize);
const uint16_t kMetadataFlagsSize = 8; // bytes
const uint16_t kMetadataValuesOffset = (kMetadataFlagsOffset + kMetadataFlagsSize);
const uint16_t kMetadataValuesSize = 16; // bytes
const uint16_t kMetadataStoryCountOffset = (kMetadataValuesOffset + kMetadataValuesSize);
const uint16_t kMetadataStoryCountSize = 1; // bytes
const uint16_t kMetadataStoryUsedPagesOffset = (kMetadataStoryCountOffset + kMetadataStoryCountSize);
const uint16_t kMetadataStoryUsedPagesSize = 1; // bytes
const uint16_t kMetadataStoryOffsetsOffset = (kMetadataStoryUsedPagesOffset + kMetadataStoryUsedPagesSize);
const uint16_t kMetadataStoryOffsetsSize = kMaxStoryCount; // 1 bytes each.
const uint16_t kMetadataSize = (kMetadataStoryOffsetsOffset + kMetadataStoryOffsetsSize); // 4 bytes each.

//const uint16_t kStoryBaseAddress = (kMetadataStoryOffsetsOffset + kMetadataStoryOffsetsSize);

#define FLG1_OFFLINE   0x80
#define FLG1_DEMO      0x40
#define FLG1_SD        0x20
#define FLG1_MULTI     0x10
#define FLG1_ARCADE    0x08

#define FLG2_LOGGING   0x80
#define FLG2_LOG_LOCAL 0x40
#define FLG2_LOG_LIVE  0x20
#define FLG2_LOG_PRINT 0x10

/* Generic Structures */

typedef struct Version_t {
	uint8_t major;
	uint8_t minor;
	uint8_t revision;
} Version;

/* Structure definitions for Metadata */

typedef struct MetaFlags_t {
	union {
	    uint8_t flag1;
	    struct {
	    	uint8_t rsvd1 			:3;
	    	uint8_t arcade			:1;
	    	uint8_t multiplayer		:1;
	    	uint8_t sdCard			:1;
	    	uint8_t demo			:1;
	    	uint8_t offline			:1;
	    };
	};
	union {
		uint8_t flag2;
		struct {
		    uint8_t rsvd2 			:4;
		    uint8_t logPrint        :1;
		    uint8_t logLive			:1;
		    uint8_t logLocation		:1;
		    uint8_t logging 		:1;
		};
    };
	union {
		uint8_t flag3;
		struct {
	        uint8_t rsvd3 			:7;
	        uint8_t dictOffsetBytes	:1; // 0 = 2byte length dict codes, 1 = 3 bytes
		};
    };
    union {
		uint8_t flag4;
		struct {
	        uint8_t rsvd4			:8;
		};
    };
    union {
		uint8_t flag5;
		struct {
	        uint8_t rsvd5 			:8;
		};
    };
    union {
		uint8_t flag6;
		struct {
	        uint8_t rsvd6			:8;
		};
    };
    union {
		uint8_t flag7;
		struct {
	        uint8_t rsvd7 			:8;
		};
    };
    union {
		uint8_t flag8;
		struct {
	        uint8_t rsvd8			:8;
		};
    };
} MetaFlags;

typedef struct MetaValues_t {
	uint8_t coinsPerCredit;
	uint8_t coinDenomination;
	uint8_t value3;
	uint8_t value4;
	uint8_t value5;
	uint8_t value6;
	uint8_t value7;
	uint8_t value8;
	uint8_t value9;
	uint8_t value10;
	uint8_t value11;
	uint8_t value12;
	uint8_t value13;
	uint8_t value14;
	uint8_t value15;
	uint8_t value16;
} MetaValues;

typedef struct Metadata_t {
	uint8_t soh;
	Version firmwareVer;
	MetaFlags flags;
	MetaValues values;
	uint8_t storyCount;
	uint8_t usedStoryPages;
	uint8_t storyOffsets[kMaxStoryCount];
	//std::vector<uint32_t> storyOffsets;
} Metadata;


/* Structure definitions for StoryHeader */

typedef struct StoryFlags_t
{
	union {
	    uint8_t flag1;
	    struct {
	    	uint8_t rsvd1 				:5;
	    	uint8_t images				:1;
	    	uint8_t variables			:1;
	    	uint8_t scripting			:1;
	    };
	};
	union {
		uint8_t flag2;
		struct {
	        uint8_t rsvd2 			:5;
	        uint8_t hideUsed		:1;
	        uint8_t multiplayer		:1;
	        uint8_t continueGame	:1;
		};
    };
	union {
		uint8_t flag3;
		struct {
	        uint8_t rsvd3 			:8;
		};
    };
    union {
		uint8_t flag4;
		struct {
	        uint8_t rsvd4 			:8;
		};
    };
} StoryFlags;

typedef struct StoryVars_t {
	uint8_t bits; // How many 1 bit flags
	uint8_t small; // How many 8 bit variables
	uint8_t big; // How many 16 bit variables
} StoryVars;

typedef struct StoryHeader_t {
	Version binaryVersion;
	StoryFlags flags;
	StoryVars vars;
    uint32_t storySize;
    Version storyVersion;
    char languageCode[4];
    char title[64];
    char subtitle[32];
    char author[48];
    char credits[80];
    char contact[128];
    tm releaseDate;
} StoryHeader ;

}

#endif