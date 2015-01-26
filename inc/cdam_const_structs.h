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
const uint16_t kMetadataValuesSize = 32; // bytes
const uint16_t kMetadataStoryCountOffset = (kMetadataValuesOffset + kMetadataValuesSize);
const uint16_t kMetadataStoryCountSize = 1; // bytes
const uint16_t kMetadataDeletedStoryCountOffset = (kMetadataStoryCountOffset + kMetadataStoryCountSize);
const uint16_t kMetadataDeletedStoryCountSize = 1; // bytes
const uint16_t kMetadataStoryUsedPagesOffset = (kMetadataDeletedStoryCountOffset + kMetadataDeletedStoryCountSize);
const uint16_t kMetadataStoryUsedPagesSize = 1; // bytes
const uint16_t kMetadataStoryOffsetsOffset = (kMetadataStoryUsedPagesOffset + kMetadataStoryUsedPagesSize);
const uint16_t kMetadataStoryOffsetsSize = kMaxRandStoryCount * 2; // 2 bytes each (2 bytes needed for SD card file index)
const uint16_t kMetadataStoryOrderOffset = (kMetadataStoryOffsetsOffset + kMetadataStoryOffsetsSize);
const uint16_t kMetadataStoryOrderSize = kMaxRandStoryCount; // 1 byte each.
const uint16_t kMetadataStoryStateOffset = (kMetadataStoryOrderOffset + kMetadataStoryOrderSize);
const uint16_t kMetadataStoryStateSize = kMaxRandStoryCount; // 1 byte each.
const uint16_t kMetadataDeviceNameOffset = (kMetadataStoryStateOffset + kMetadataStoryStateSize);
const uint16_t kMetadataDeviceNameSize = 22; // bytes
const uint16_t kMetadataOwnerNameOffset = (kMetadataDeviceNameOffset + kMetadataDeviceNameSize);
const uint16_t kMetadataOwnerNameSize = 40; // bytes
const uint16_t kMetadataSize = (kMetadataOwnerNameOffset + kMetadataOwnerNameSize);

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
		int8_t flag1;
		struct {
			uint8_t auth 			:1; // Bit 0
			uint8_t random			:1;
			uint8_t continues		:1;
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
			uint8_t logPrint		:1;
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
	uint16_t coinsPerCredit;
	uint16_t coinDenomination;
	uint16_t coinsToContinue;
	uint16_t value4;
	uint16_t value5;
	uint16_t value6;
	uint16_t value7;
	uint16_t value8;
	uint16_t value9;
	uint16_t value10;
	uint16_t value11;
	uint16_t value12;
	uint16_t value13;
	uint16_t value14;
	uint16_t value15;
	uint16_t value16;
} MetaValues;

typedef struct Metadata_t {
	uint8_t soh;
	Version firmwareVer;
	MetaFlags flags;
	MetaValues values;
	uint8_t storyCount;
	uint8_t deletedStoryCount;
	uint8_t usedStoryPages;
	uint16_t storyOffsets[kMaxRandStoryCount];
	uint8_t storyOrder[kMaxRandStoryCount];
	uint8_t storyState[kMaxRandStoryCount]; // 0 = empty, 1 = normal, 2 = deleted
	char deviceName[kMetadataDeviceNameSize];
	char ownerName[kMetadataOwnerNameSize];
} Metadata;


/* Structure definitions for StoryHeader */

const uint16_t kStoryBinaryVersionOffset = 1; // adding SOH byte
const uint16_t kStoryBinaryVersionSize = 3;
const uint16_t kStoryUuidOffset = (kStoryBinaryVersionOffset + kStoryBinaryVersionSize);
const uint16_t kStoryUuidSize = 16; // bytes
const uint16_t kStoryFlagsOffset = (kStoryUuidOffset + kStoryUuidSize);
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
const uint16_t kStoryVarCountOffset = (kStoryPublishedOffset + kStoryPublishedSize);
const uint16_t kStoryVarCountSize = 2; // bytes
const uint16_t kStoryHeaderSize = (kStoryVarCountOffset + kStoryVarCountSize);
//const uint16_t kStoryHeaderSize = 394;

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

typedef struct Uuid_t {
	uint32_t time_low;
	uint16_t time_mid;
	uint16_t time_hi_and_version;
	uint8_t clock_seq_hi_and_reserved;
	uint8_t clock_seq_low;
	uint8_t node[6];
} Uuid;

/*typedef struct StoryVars_t {
	uint8_t small; // How many 8 bit variables
	uint8_t big; // How many 16 bit variables
	uint8_t rsvd1;
	uint8_t rsvd2;
} StoryVars;*/

typedef struct StoryHeader_t { // Total Size: 394 bytes
	uint8_t soh;
	Version binaryVer;
	Uuid uuid;
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
    uint16_t variableCount;
} StoryHeader;

const uint8_t kOpTypeRaw = 1;
const uint8_t kOpTypeVar = 2;
const uint8_t kOpTypeOperation  = 3;

const uint8_t kOpEqualTo = 1; // Returns 0 or 1
const uint8_t kOpNotEqualTo = 2; // Returns 0 or 1
const uint8_t kOpGreaterThan = 3; // Returns 0 or 1
const uint8_t kOpLessThan = 4; // Returns 0 or 1
const uint8_t kOpEqualGreater = 5; // Returns 0 or 1
const uint8_t kOpEqualLess = 6; // Returns 0 or 1
const uint8_t kOpAND = 7; // Returns 0 or 1
const uint8_t kOpOR = 8; // Returns 0 or 1
const uint8_t kOpXOR = 9; // Returns 0 or 1
const uint8_t kOpNAND = 10; // Returns 0 or 1
const uint8_t kOpNOR = 11; // Returns 0 or 1
const uint8_t kOpXNOR = 12; // Returns 0 or 1
const uint8_t kOpChoiceVisible = 13; // Returns 0 or 1
const uint8_t kOpModulus = 14; // Returns int16_t - remainder of division
const uint8_t kOpSet = 15; // Returns int16_t - value of the right operand
const uint8_t kOpPlus = 16; // Returns int16_t
const uint8_t kOpMinus = 17; // Returns int16_t
const uint8_t kOpMultiply = 18; // Returns int16_t
const uint8_t kOpDivide = 19; // Returns int16_t - non float, whole number
const uint8_t kOpRandom = 20; // Returns int16_t - takes min & max (inclusive)
const uint8_t kOpDiceRoll = 21; // Returns int16_t - take # of dice & # of sides per die
const uint8_t kOpIfStatement = 22; // Returns 0 if false, result of right operand if true

typedef struct Operation_t {
	union {
		uint8_t operandTypes;
		struct {
			uint8_t rightType :4;
			uint8_t leftType  :4;
		};
	};
	uint8_t operationType;
	int16_t leftOperand;
	int16_t rightOperand;
} Operation;

typedef struct Passage_t {
	union {
		uint8_t attribute;
		struct {
			uint8_t rsvd   :7;
			uint8_t append :1;
		};
	};
} Passage;

typedef struct Choice_t {
	union {
		uint8_t attribute;
		struct {
			uint8_t rsvd   :7;
			uint8_t append :1;
		};
	};
	int16_t visible;
	uint32_t updatesOffset;
	uint16_t passageIndex;
} Choice;

}

#endif