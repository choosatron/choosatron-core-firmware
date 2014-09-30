#ifndef CDAM_CONST_STRUCTS_H
#define CDAM_CONST_STRUCTS_H

//#include <vector>

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
const uint16_t kMetadataStoryOrderOffset = (kMetadataStoryOffsetsOffset + kMetadataStoryOffsetsSize);
const uint16_t kMetadataStoryOrderSize = kMaxStoryCount; // 1 bytes each.
const uint16_t kMetadataSize = (kMetadataStoryOrderOffset + kMetadataStoryOrderSize);

//const uint16_t kStoryBaseAddress = (kMetadataStoryOffsetsOffset + kMetadataStoryOffsetsSize);

/*#define FLG1_OFFLINE   0x80
#define FLG1_DEMO      0x40
#define FLG1_SD        0x20
#define FLG1_MULTI     0x10
#define FLG1_ARCADE    0x08

#define FLG2_LOGGING   0x80
#define FLG2_LOG_LOCAL 0x40
#define FLG2_LOG_LIVE  0x20
#define FLG2_LOG_PRINT 0x10*/

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
	    	uint8_t auth 			:1; // Bit 0
	    	uint8_t random          :1;
	    	uint8_t continues       :1;
	    	uint8_t arcade			:1;
	    	uint8_t multiplayer		:1;
	    	uint8_t sdCard			:1;
	    	uint8_t demo			:1;
	    	uint8_t offline			:1; // Bit 7
	    };
	};
	union {
		uint8_t flag2;
		struct {
		    uint8_t rsvd2 			:4; // Bit 0
		    uint8_t logPrint        :1;
		    uint8_t logLive			:1;
		    uint8_t logLocal		:1;
		    uint8_t logging 		:1; // Bit 7
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
	uint8_t coinsToContinue;
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
	uint8_t storyOffsets[kMaxRandStoryCount];
	uint8_t storyOrder[kMaxRandStoryCount];
	//std::vector<uint32_t> storyOffsets;
} Metadata;


/* Structure definitions for StoryHeader */

const uint16_t kStoryBinaryVersionOffset = 1; // adding SOH byte
const uint16_t kStoryBinaryVersionSize = 3;
const uint16_t kStoryFlagsOffset = (kStoryBinaryVersionOffset + kStoryBinaryVersionSize);
const uint16_t kStoryFlagsSize = 4; // bytes
const uint16_t kStorySizeOffset = (kStoryFlagsOffset + kStoryFlagsSize);
const uint16_t kStorySizeSize = 4; // bytes
const uint16_t kStoryVersionOffset = (kStorySizeOffset + kStorySizeSize);
const uint16_t kStoryVersionSize = 3; // bytes
const uint16_t kStoryRsvdOffset = (kStoryVersionOffset + kStoryVersionSize);
const uint16_t kStoryRsvdSize = 1; // bytes
const uint16_t kStoryLangCodeOffset = (kStoryRsvdOffset + kStoryRsvdSize);
const uint16_t kStoryLangCodeSize = 4; // bytes
const uint16_t kStoryTitleOffset = (kStoryLangCodeOffset + kStoryLangCodeSize);
const uint16_t kStoryTitleSize = 64; // bytes
const uint16_t kStorySubtitleOffset = (kStoryTitleOffset + kStoryTitleSize);
const uint16_t kStorySubtitleSize = 32; // bytes
const uint16_t kStoryAuthorOffset = (kStorySubtitleOffset + kStorySubtitleSize);
const uint16_t kStoryAuthorSize = 48; // bytes
const uint16_t kStoryCreditsOffset = (kStoryAuthorOffset + kStoryAuthorSize);
const uint16_t kStoryCreditsSize = 80; // bytes
const uint16_t kStoryContactOffset = (kStoryCreditsOffset + kStoryCreditsSize);
const uint16_t kStoryContactSize = 128; // bytes
const uint16_t kStoryPublishedOffset = (kStoryContactOffset + kStoryContactSize);
const uint16_t kStoryPublishedSize = 4; // bytes
const uint16_t kStoryHeaderSize = (kStoryPublishedOffset + kStoryPublishedSize);
//const uint16_t kStoryHeaderSize = 376;

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
	        uint8_t continues		:1;
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

/*typedef struct StoryVars_t {
	uint8_t small; // How many 8 bit variables
	uint8_t big; // How many 16 bit variables
	uint8_t rsvd1;
	uint8_t rsvd2;
} StoryVars;*/

typedef struct StoryHeader_t { // Total Size: 376 bytes
	uint8_t soh;
	Version binaryVer;
	StoryFlags flags;
    uint32_t storySize;
    Version storyVer;
    uint8_t rsvd;
    char languageCode[kStoryLangCodeSize];
    char title[kStoryTitleSize];
    char subtitle[kStorySubtitleSize];
    char author[kStoryAuthorSize];
    char credits[kStoryCreditsSize];
    char contact[kStoryContactSize];
    time_t publishDate;
} StoryHeader;


const uint8_t kValueTypeRaw = 0;
const uint8_t kValueTypeBool = 1;
const uint8_t kValueTypeSmall = 2;
const uint8_t kValueTypeBig = 3;

const uint8_t kOperatorEquals = 0;
const uint8_t kOperatorPlus = 1;
const uint8_t kOperatorMinus = 2;
const uint8_t kOperatorMultiply = 3;
const uint8_t kOperatorDivide = 4;
const uint8_t kOperatorModulus = 5;

const uint8_t kCompareEqual = 0;
const uint8_t kCompareGreater = 1;
const uint8_t kCompareLess = 2;
const uint8_t kCompareEqualGreater = 3;
const uint8_t kCompareEqualLess = 4;
const uint8_t kCompareModulus = 5;

typedef struct ValueSet_t {
	union {
		uint8_t varTypes;
		struct {
			//uint8_t logicGate    :1; Should add logic gates? AND, OR, XOR?
			uint8_t operatorType :4;
			uint8_t varTwoType   :2;
			uint8_t varOneType   :2;
		};
	};
	uint16_t varOne;
	uint16_t varTwo;
} ValueSet;

typedef struct Choice_t {
	union {
		uint8_t attribute;
		struct {
			uint8_t rsvd   :7;
			uint8_t append :1;
		};
	};
	bool visible;
	uint32_t updatesOffset;
	uint16_t passageIndex;
} Choice;

}

#endif