#ifndef CDAM_PARSER_H
#define CDAM_PARSER_H

#include "cdam_constants.h"

namespace cdam
{

class DataManager;
class HardwareManager;

class Parser {

public:
	/* Public Methods */
    void initialize();

    uint8_t wrapText(char* aBuffer, uint8_t aColumns);

    bool parsePassage(char* aBuffer, uint16_t aIndex);
    bool parseValueUpdate(char* aBuffer, uint16_t aIndex);
    bool parseChoice(char* aBuffer, uint16_t aIndex);

    /* Public Variables */
protected:

private:
	/* Private Methods */

	/* Private Variables */
    DataManager* _dataManager;
    HardwareManager* _hardwareManager;

};

}

#endif