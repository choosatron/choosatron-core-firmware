#include "cdam_parser.h"
#include "cdam_manager.h"

namespace cdam
{

/* Public Methods */

void Parser::initialize() {
	_dataManager = Manager::getInstance().dataManager;
	_hardwareManager = Manager::getInstance().hardwareManager;
}

uint8_t Parser::wrapText(char* aBuffer, uint8_t aColumns) {
	DEBUG("Title: %s, Columns: %d", aBuffer, aColumns);
	uint16_t length = strlen(aBuffer);
	DEBUG("Length: %d", length);

	// Should always represent the first char index of a line.
	uint16_t startIndex = 0;
	bool foundBreak = false;
	// Wrap until we have less than one full line.
	while ((length - startIndex) > aColumns) {
		DEBUG("Start wrapping...");
		foundBreak = false;
		uint8_t i;
  		// Search for a newline to break on.
		for (i = 0; i < aColumns; ++i) {
			if (aBuffer[startIndex + i] == '\t') {
				aBuffer[startIndex + i] = ' ';
      			WARN("Found tab.");
			} else if (aBuffer[startIndex + i] == '\r') {
      			WARN("Found carriage return.");
			} else if (aBuffer[startIndex + i] == '\n') {
				DEBUG("Found newline.");
				foundBreak = true;
				startIndex += i + 1;
				break;
			}
		}

  		// Search for a space to break on.
		if (foundBreak == false) {
			DEBUG("Looking for a space...");
			for (i = aColumns; i > 0; --i) {
				if (aBuffer[startIndex + i] == ' ') {
					DEBUG("Found space at: %d", i);
					foundBreak = true;
					aBuffer[startIndex + i] = '\n';
					startIndex += i + 1;
					break;
				}
			}
			if (foundBreak == false) {
				startIndex += aColumns;
			}
		}
	}
	DEBUG("Last bit length: %d", length - startIndex);
	return length - startIndex;
}

/* Accessors */

/* Private Methods */

}