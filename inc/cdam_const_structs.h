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
const uint16_t kMetadataDeviceNameOffset = (kMetadataValuesOffset + kMetadataValuesSize);
const uint16_t kMetadataDeviceNameSize = 30; // bytes
const uint16_t kMetadataOwnerProfileOffset = (kMetadataDeviceNameOffset + kMetadataDeviceNameSize);
const uint16_t kMetadataOwnerProfileSize = 30; // bytes
const uint16_t kMetadataOwnerCloudOffset = (kMetadataOwnerProfileOffset + kMetadataOwnerProfileSize);
const uint16_t kMetadataOwnerCloudSize = 63; // bytes
const uint16_t kMetadataStoryCountOffset = (kMetadataOwnerCloudOffset + kMetadataOwnerCloudSize);
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

const uint16_t kMetadataSize = (kMetadataStoryStateOffset + kMetadataStoryStateSize);

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
   uint16_t pcbVersion;
   int16_t timezoneOffset;
   uint16_t coinsPerCredit;
   uint16_t coinDenomination;
   uint16_t coinsToContinue;
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
   char deviceName[kMetadataDeviceNameSize];
   char ownerProfile[kMetadataOwnerProfileSize];
   char ownerCloud[kMetadataOwnerCloudSize];
   uint8_t storyCount;
   uint8_t deletedStoryCount;
   uint8_t usedStoryPages;
   uint16_t storyOffsets[kMaxRandStoryCount];
   uint8_t storyOrder[kMaxRandStoryCount];
   uint8_t storyState[kMaxRandStoryCount]; // 0 = empty, 1 = normal, 2 = deleted
} Metadata;


/* Structure definitions for StoryHeader */

const uint16_t kStoryBinaryVersionOffset = 1; // adding SOH byte
const uint16_t kStoryBinaryVersionSize = 3;
const uint16_t kStoryUuidOffset = (kStoryBinaryVersionOffset + kStoryBinaryVersionSize);
const uint16_t kStoryUuidSize = 36; // bytes
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

typedef struct StoryFlags_t
{
   union {
      uint8_t flag1;
      struct {
         uint8_t rsvd1 				:1; // UNUSED
         uint8_t multiplayer			:1; // This is a multiplayer game. *REQUIRES WIFI* *NOT IMPLEMENTED*
         uint8_t realtime			:1; // Uses the realtime clock. *REQUIRES WIFI* *NOT IMPLEMENTED*
         uint8_t images				:1; // Uses bitmap printing. *NOT IMPLEMENTED*
         uint8_t variables			:1; // Uses variables for logic.
         uint8_t liveVars			:1; // Uses online variable. *REQUIRES WIFI* *NOT IMPLEMENTED*
         uint8_t saveState			:1; // Stores variable states between sessions. *NOT IMPLEMENTED*
         uint8_t scripting			:1; // Uses dynamic scripting (in copy). *NOT IMPLEMENTED*
      };
   };
   union {
      uint8_t flag2;
      struct {
         uint8_t rsvd2 			:6;
         uint8_t hideUsed		:1; // At a given passage, hide choices already made. *NOT IMPLEMENTED*
         uint8_t continues		:1; // Allows the ability to continue (jump back choices). *NOT IMPLEMENTED*
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

typedef struct StoryHeader_t { // Total Size: 394 bytes
   uint8_t soh;
   Version binaryVer;
   char uuid[kStoryUuidSize];
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
const uint8_t kOpTypeOperation = 3;
const uint8_t kOpTypeChoiceCount = 4;
const uint8_t kOpTypeTurns = 5;
const uint8_t kOpTypePsgVisits = 6; // -- Returns the number of visits to the current passage (NOT IMPLEMENTED)

static const char* const kDataTypeStr[] = {
   "",
   "RAW",
   "VAR",
   "OP",
   "CC",
   "TURNS",
   "PSG_V"
};

const uint8_t kOpEqual = 1; // Returns 0 or 1
const uint8_t kOpNotEquals = 2; // Returns 0 or 1
const uint8_t kOpGreater = 3; // Returns 0 or 1
const uint8_t kOpLess = 4; // Returns 0 or 1
const uint8_t kOpGreaterThanOrEquals = 5; // Returns 0 or 1
const uint8_t kOpLessThanOrEquals = 6; // Returns 0 or 1
const uint8_t kOpAND = 7; // Returns 0 or 1
const uint8_t kOpOR = 8; // Returns 0 or 1
const uint8_t kOpXOR = 9; // Returns 0 or 1
const uint8_t kOpNAND = 10; // Returns 0 or 1
const uint8_t kOpNOR = 11; // Returns 0 or 1
const uint8_t kOpXNOR = 12; // Returns 0 or 1
const uint8_t kOpChoiceVisible = 13; // Returns 0 or 1
const uint8_t kOpMod = 14; // Returns int16_t - remainder of division
const uint8_t kOpAssign = 15; // Returns int16_t - value of the right operand
const uint8_t kOpAdd = 16; // Returns int16_t
const uint8_t kOpSubtract = 17; // Returns int16_t
const uint8_t kOpMultiply = 18; // Returns int16_t
const uint8_t kOpDivide = 19; // Returns int16_t - non float, whole number
const uint8_t kOpRandom = 20; // Returns int16_t - takes min & max (inclusive)
const uint8_t kOpDice = 21; // Returns int16_t - take # of dice & # of sides per die
const uint8_t kOpIfStatement = 22; // If true, returns the result of right operand
const uint8_t kOpElseStatement = 23;
const uint8_t kOpNegate = 24; // Returns int16_t - positive val becomes negative, negative val becomes positive
const uint8_t kOpNot = 25; // Returns 0 or 1 - if val greater than 0, returns 0, if val is 0 or less, returns 1
const uint8_t kOpMin = 26; // Returns int16_t - the smaller of the two numbers
const uint8_t kOpMax = 27; // Returns int16_t - the larger of the two numbers
const uint8_t kOpPow = 28; // Returns int16_t - val a to the power of val b
const uint8_t kOpChoiceCount = 29; // Returns int16_t - total choices already visible
const uint8_t kOpTurns = 30; // Returns int16_t - the number of game turns since the game began

static const char* const kOpStr[] = {
   "",
   "==",
   "!=",
   ">",
   "<",
   ">=",
   "<=",
   "AND",
   "OR",
   "XOR",
   "NAND",
   "NOR",
   "XNOR",
   "ChoiceVisible",
   "%",
   "=",
   "+",
   "-",
   "*",
   "/",
   "Rand",
   "DiceRoll",
   "If",
   "Else",
   "Negate",
   "!",
   "Min",
   "Max",
   "Pow",
};

// Variable Text Logic
const uint8_t kLogicCmd = 1;        //
const uint8_t kLogicOpen = 2;       //
const uint8_t kLogicClose = 3;      //
const uint8_t kLogicVar = 4;        // Variable
const uint8_t kLogicVarPsg = 5;     // Passage Count
const uint8_t kLogicVarCond = 6;    // Variable Condition
const uint8_t kLogicSequence = 7;   //
const uint8_t kLogicSeqOnce = 8;    //
const uint8_t kLogicSeqCycle = 9;   //
const uint8_t kLogicSeqDelim = 10;  //
const uint8_t kLogicShuffle = 11;   //

// -- Based on Ink variable text.
// -- https://github.com/inkle/ink/blob/master/Documentation/WritingWithInk.md#6-variable-text
// local kLogicStr <const> = {
//    -- '{.-}', -- Logic command match string.
//    '{%s*(.-)%s*}', -- Match everything inside command curly brackets.
//    '{%s*', -- Match left logic open curly + spaces.
//    '%s*}', -- Match right logic closing curly + spaces.
//    '[', -- Variable access. [idx] = var value
//    '<', -- Passage visit variable access. <psg idx> = psg visit count
//    ':', -- If after a variable, becomes a condition with optional '|' as an OR, IF VAR THEN A OR B
//    '=', -- Sequence through content, end on final element.
//    '!', -- Sequence through content, then blank.
//    '&', -- Cycle and loop sequence content.
//    '^', -- Fall through values. Continue using first element content until a next element has content, etc.
//    '|', -- Delimiter for condition or sequences.
//    '~', -- Shuffle, randomly select from the set.
// }

// -- local kLogicMatch <const> = "{.-}" -- Logic command match string.
// local kLogicMatch <const> = '{%s*(.*)%s*}'--'{%s*(.-)%s*}' -- Match everything inside command curly brackets.
// local kLogicOpen <const> = '{' -- Match left logic open curly + spaces.
// local kLogicClose <const> = '}' -- Match right logic closing curly + spaces.
// local kLogicVarOpen <const> = '[' -- Variable access. [idx] = var value
// local kLogicVarMatch <const> = '%[%s*(%d-)%s*%]'
// local kLogicVarClose <const> = ']'
// local kLogicVarPsgOpen <const> = '<' -- Passage visit variable access. <psg idx> = psg visit count
// local kLogicVarPsgMatch <const> = '<%s*(%d-)%s*>'
// local kLogicVarPsgClose <const> = '>'
// local kLogicCondition <const> = ':' -- If after a variable, becomes a condition with optional '|' as an OR, IF VAR THEN A OR B
// local kLogicSeq <const> = '=' -- Sequence through content, end on final element.
// local kLogicSeqOnce <const> = '!' -- Sequence through content, then blank.
// local kLogicSeqCycle <const> = '&' -- Cycle / loop sequence content.
// local kLogicSeqFall <const> = '^' -- First element used until another element with content, then that is used until another with content, etc.
// local kLogicSeqDelim <const> = '|' -- Delimiter for condition or sequences.
// local kLogicShuffle <const> = '~' -- Shuffle, randomly select from the set.




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
         uint8_t rsvd      :6;
         uint8_t cont      :1;
         uint8_t append    :1;
      };
   };
} Passage;

typedef struct Choice_t {
   union {
      uint8_t attribute;
      struct {
         uint8_t hasConditions         :1;
         uint8_t hasStartContent       :1;
         uint8_t hasChoiceOnlyContent  :1;
         uint8_t isInvisibleDefault    :1;
         uint8_t onceOnly              :1;
         uint8_t rvsvd                 :3;
      };
   };
   int16_t visible;
   uint32_t updatesOffset;
   uint16_t passageIndex;
} Choice;

}

#endif