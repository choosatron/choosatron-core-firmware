#ifndef CDAM_PARSER_H
#define CDAM_PARSER_H

#include "cdam_constants.h"
//#include <vector>

namespace cdam
{

typedef enum ParseState_t {
	PARSE_IDLE,
	PARSE_ERROR,
	PARSE_UPDATES,
	PARSE_DATA,
	PARSE_CHOICE,
	PARSE_CONDITIONALS,
	PARSE_USER_INPUT,
	PARSE_ENDING
} ParseState;

class DataManager;
class HardwareManager;

class Parser {

public:
	/* Public Methods */
    void initialize();

    bool initStory(uint8_t aStoryIndex);
    ParseState parsePassage();
    //uint32_t parseBody(uint32_t aOffset);
    //void parseData(char* aBuffer);
    //uint32_t parseChoice(uint32_t aOffset, Choice* aChoice, uint8_t aVisibleChoices);
    //bool parsePassage(char* aBuffer, uint16_t aIndex);
    //bool parseValueUpdate(char* aBuffer, uint16_t aIndex);
    //bool parseChoice(char* aBuffer, uint16_t aIndex);

    /* Public Variables */
protected:

private:
	/* Private Methods */
	void cleanupAfterPassage();
	uint32_t parseData(uint32_t aOffset, char* aBuffer, uint16_t aLength);
	uint32_t parseCommand(uint32_t aOffset, char* aBuffer, uint16_t aLength);
	uint32_t parseValueUpdates(uint32_t aOffset);
    uint32_t parseConditions(uint32_t aOffset, bool &aResult);

	/* Private Variables */
    DataManager* _dataManager;
    HardwareManager* _hardwareManager;

    // Current state of the parser, allows splitting parsing work between loops.
    ParseState _state;
    // Whether or not we are currently parasing the passage body or choices.
    bool _parsingChoices;
    // The master byte offset for all data reads.
    uint32_t _offset;
    // Last column index from word wrapping / printing to keep it lined up.
    uint8_t _lastIndent;
    // Was the last passage an 'append'.
    bool _appended;
    // Buffer to parse.
    char* _buffer;
    // Total length of the data body to be parsed.
    uint16_t _dataLength;
    // Total choices in the passage being parsed.
    uint8_t _choiceCount;
    // The index of the choice being parsed.
    uint8_t _choiceIndex;
    // Incrementing count of how many of the choices are visible.
    uint8_t _visibleCount;
    // The value of the user selected choice (not index).
    uint8_t _choiceSelected;
    // Array of choice objects, visible or not.
    Choice* _choices;
    // Index array to associate choice # with the corresponding visible choice.
    uint8_t* _choiceLinks;
    //std::vector<Choice> _choices;

};

}

#endif