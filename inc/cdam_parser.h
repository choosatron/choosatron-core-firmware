#ifndef CDAM_PARSER_H
#define CDAM_PARSER_H

#include "cdam_constants.h"

namespace cdam
{

class Parser {

public:
	/* Public Methods */
    Parser();

    uint8_t wrapText(char* aBuffer, uint8_t aColumns);

    /* Public Variables */
protected:

private:

};

}

#endif