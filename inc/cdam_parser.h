#ifndef CDAM_PARSER_H
#define CDAM_PARSER_H

#include "cdam_constants.h"
//#include <vector>

namespace cdam
{

typedef enum ParseState_t {
	PARSE_NONE,
	PARSE_ERROR,
	PARSE_PSG_UPDATES,
	PARSE_BODY_TEXT,
	PARSE_BODY_CMD,
	PARSE_CHOICE_COND,
	PARSE_CHOICE_TEXT,
	PARSE_CHOICE_CMD,
	PARSE_CHOICE_UPDATES
} ParseState;

class DataManager;
class HardwareManager;

class Parser {

public:
	/* Public Methods */
    void initialize();

    void initStory(uint8_t aStoryIndex);
    bool parsePassage();
    uint32_t parseValueUpdates(uint32_t aOffset);
    uint32_t parseConditions(uint32_t aOffset, bool aResult);
    uint32_t parseBody(uint32_t aOffset);
    uint32_t parseAndPrintData(uint32_t aOffset, uint32_t aLength);
    //void parseData(char* aBuffer);
    bool parseCommand(char* aBuffer, uint16_t aLength);
    uint32_t parseChoice(uint32_t aOffset, Choice* aChoice, uint8_t aVisibleChoices);

    uint8_t wrapText(char* aBuffer, uint8_t aColumns);

    //bool parsePassage(char* aBuffer, uint16_t aIndex);
    //bool parseValueUpdate(char* aBuffer, uint16_t aIndex);
    //bool parseChoice(char* aBuffer, uint16_t aIndex);

    /* Public Variables */
protected:

private:
	/* Private Methods */

	/* Private Variables */
    DataManager* _dataManager;
    HardwareManager* _hardwareManager;

    ParseState _state;
    uint32_t _offset;
    uint8_t _visibleChoices;
    Choice* _choices;
    uint8_t* _choiceIndex;
    //std::vector<Choice> _choices;

};

}

#endif