// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#include "cdam_constants.h"
#include "cdam_manager.h"
#include "cdam_printer.h"

namespace cdam
{

/*#define PS_UNKNOWN_ONE   7
#define PS_HIGH_TEMP     6
#define PS_UNKNOWN_TWO   5
#define PS_UNKNOWN_THREE 4
#define PS_HIGH_VOLTAGE  3
#define PS_NO_PAPER      2
#define PS_BUFFER_FULL   1
#define PS_ONLINE        0*/

/*#define PS_UNKNOWN_ONE   0x80
#define PS_HIGH_TEMP     0x40
#define PS_UNKNOWN_TWO   0x20
#define PS_UNKNOWN_THREE 0x10
#define PS_HIGH_VOLTAGE  0x08
#define PS_NO_PAPER      0x04
#define PS_BUFFER_FULL   0x02
#define PS_ONLINE        0x01*/

Printer::Printer() : CSN_Thermal::CSN_Thermal() {
}

void Printer::initialize() {
	CSN_Thermal::init();
	this->ready = false;
	this->printing = false;
	this->printTime = 0;
	this->active = true;
}

void Printer::updateState() {
	if (this->active) {
		if (available()) {
			_lastStatus = _status;
			_status = read();
			_statusUpdated = true;
			DEBUG("0x%c%c%c%c%c%c%c%c",
				(IsBitSet(_status, 7) ? '1' : '0'),
				(IsBitSet(_status, 6) ? '1' : '0'),
				(IsBitSet(_status, 5) ? '1' : '0'),
				(IsBitSet(_status, 4) ? '1' : '0'),
				(IsBitSet(_status, 3) ? '1' : '0'),
				(IsBitSet(_status, 2) ? '1' : '0'),
				(IsBitSet(_status, 1) ? '1' : '0'),
				(IsBitSet(_status, 0) ? '1' : '0'));
		}
	} else {
		_lastStatus = ASCII_NULL;
		_status = ASCII_NULL;
	}
}

bool Printer::statusOf(PrinterStatus aStatus) {
	return IsBitSet(_status, aStatus);
}

bool Printer::statusChanged(PrinterStatus aStatus) {
	if (IsBitSet(_lastStatus, aStatus) != IsBitSet(_status, aStatus)) {
		return true;
	}
	return false;
}

void Printer::logChangedStatus() {
	if (_statusUpdated) {
		_statusUpdated = false;
		if (statusChanged(PS_ONLINE)) {
			if (statusOf(PS_ONLINE)) {
				DEBUG("Online");
			} else {
				DEBUG("Offline");
			}
		}
		if (statusChanged(PS_BUFFER_FULL)) {
			if (statusOf(PS_BUFFER_FULL)) {
				DEBUG("Buffer Full");
			} else {
				DEBUG("Buffer Not Full");
			}
		}
		if (statusChanged(PS_NO_PAPER)) {
			if (statusOf(PS_NO_PAPER)) {
				DEBUG("No Paper");
			} else {
				DEBUG("Paper");
			}
		}
		if (statusChanged(PS_HIGH_VOLTAGE)) {
			if (statusOf(PS_HIGH_VOLTAGE)) {
				DEBUG("High Voltage");
			} else {
				DEBUG("Normal Voltage");
			}
		}
		if (statusChanged(PS_UNKNOWN_ONE)) {
			if (statusOf(PS_UNKNOWN_ONE)) {
				DEBUG("Unknown 1 ON");
			} else {
				// Occurred once when I was printing stories
				// like crazy, and it ran out of paper and
				// appeared to start to jam.
				DEBUG("Unknown 1 OFF");
			}
		}
		if (statusChanged(PS_UNKNOWN_TWO)) {
			if (statusOf(PS_UNKNOWN_TWO)) {
				DEBUG("Unknown 2 ON");
			} else {
				DEBUG("Unknown 2 OFF");
			}
		}
		if (statusChanged(PS_HIGH_TEMP)) {
			if (statusOf(PS_HIGH_TEMP)) {
				DEBUG("High Temp");
			} else {
				DEBUG("Normal Temp");
			}
		}
		if (statusChanged(PS_UNKNOWN_THREE)) {
			if (statusOf(PS_UNKNOWN_THREE)) {
				DEBUG("Unknown 3 ON");
			} else {
				DEBUG("Unknown 3 OFF");
			}
		}
	}
}

void Printer::printInsertCoin(uint8_t aCoins, uint8_t aCoinsPerCredit) {
	CSN_Thermal::inverseOn();
	CSN_Thermal::justify('C');

	char buffer[31];
	snprintf(buffer, sizeof(buffer), CDAM_ARCADE_CREDITS, aCoins, aCoinsPerCredit);
	println(buffer);
	/*if (aNextCredit == 0) {
		print("CREDITS 0 - INSERT ");
		println(" COIN(S)");
	} else {
		print("CREDITS ");
		print(aNextCredit);
		print("/");
		print(COINS_PER_CREDIT);
		println(" - INSERT COIN(S)");
	}*/
	CSN_Thermal::inverseOff();
	CSN_Thermal::justify('L');
	feed(2);
}

void Printer::printPressButton() {
	CSN_Thermal::boldOn();
	CSN_Thermal::justify('C');

	println(CDAM_PRESS_BUTTON);

	CSN_Thermal::boldOff();
	CSN_Thermal::justify('L');
	feed(2);
}

void Printer::printTitle() {
	CSN_Thermal::boldOn();
	CSN_Thermal::justify('C');
	CSN_Thermal::doubleHeightOn();

	println(CDAM_TITLE);

	CSN_Thermal::doubleHeightOff();

	println(CDAM_SUBTITLE);

	CSN_Thermal::boldOff();
	CSN_Thermal::justify('L');
}

void Printer::printEmpty() {
	CSN_Thermal::boldOn();
	feed(1);
	println(CDAM_EMPTY);
	feed(1);
	CSN_Thermal::boldOff();
}

void Printer::printStart() {
	CSN_Thermal::justify('C');

	println(CDAM_START);

	CSN_Thermal::justify('L');
}

void Printer::printStoryIntro(char* aTitle, char* aAuthor) {
	CSN_Thermal::boldOn();
	CSN_Thermal::justify('C');

	wrapText(aTitle, kPrinterColumns);
	wrapText(aAuthor, kPrinterColumns);
	println(aTitle);

	CSN_Thermal::boldOff();
	println(CDAM_STORY_BY);
	println(aAuthor);
	CSN_Thermal::justify('L');
}

/*void Printer::printAuthors(char* aAuthor, char* aCredits) {
	CSN_Thermal::boldOn();
	CSN_Thermal::justify('C');

	char buffer[31];
	snprintf(buffer, sizeof(buffer), CDAM_STORY_BY, aAuthor);

	println(buffer);

	if (sizeof(aCredits) > 0) {
		DEBUG("Printing credits...");
		snprintf(buffer, sizeof(buffer), CDAM_CREDITS, aCredits);
	}

	CSN_Thermal::justify('L');
	CSN_Thermal::boldOff();
}*/

void Printer::printBigNumbers() {
	CSN_Thermal::boldOn();
	println(CDAM_BIG_NUMBERS);
	CSN_Thermal::boldOff();
	feed(1);
}

void Printer::printContinue(uint8_t aCoinsToContinue) {
	CSN_Thermal::boldOn();
	CSN_Thermal::justify('C');

	char buffer[32];
	snprintf(buffer, sizeof(buffer), CDAM_ARCADE_CONTINUE, aCoinsToContinue);
	println(buffer);

	CSN_Thermal::boldOff();
  	CSN_Thermal::justify('L');
	feed(2);
}

void Printer::printEnding(char* aCredits, char* aContact) {
	CSN_Thermal::boldOn();
	CSN_Thermal::justify('C');

	bool printedExtra = false;
	if (strlen(aCredits)) {
		println(aCredits);
		printedExtra = true;
	}
	if (strlen(aContact)) {
		println(aContact);
		printedExtra = true;
	}
	if (printedExtra) {
		feed(1);
	}
	CSN_Thermal::boldOff();
	println(CDAM_TEAR_GENTLY);
	CSN_Thermal::justify('L');
	feed(2);
}

uint8_t Printer::wrapText(char* aBuffer, uint8_t aColumns, uint8_t aStartOffset) {
	//DEBUG("Text: %s, Columns: %d", aBuffer, aColumns);
	uint16_t length = strlen(aBuffer);
	//DEBUG("Length: %d, startOffset: %d", length, aStartOffset);

	// Should always represent the first char index of a line.
	uint16_t startIndex = 0;
	uint8_t columns = aColumns - aStartOffset;
	// Wrap until we have less than one full line.
	while ((length - startIndex) > columns) {
		columns = aColumns;
		bool foundBreak = false;
		uint8_t i;
		// Search for a newline to break on.
		for (i = 0; i < (aColumns - aStartOffset); ++i) {
			if (aBuffer[startIndex + i] == '\t') {
				aBuffer[startIndex + i] = ' ';
				//WARN("Found tab.");
			} else if (aBuffer[startIndex + i] == '\r') {
				//WARN("Found carriage return.");
			} else if (aBuffer[startIndex + i] == '\n') {
				// Found Newline
				foundBreak = true;
				startIndex += i + 1;
				break;
			}
		}

		// Search for a space to break on.
		if (foundBreak == false) {
			for (i = (aColumns - aStartOffset); i >= 0; --i) {
				if (aBuffer[startIndex + i] == ' ') {
					// Found Space
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
		aStartOffset = 0;
	}
	return (length - startIndex) % aColumns;
}

void Printer::begin(int heatTime) {
	CSN_Thermal::begin(heatTime);
	this->ready = true;
}

// Override for easier print debugging.
size_t Printer::write(uint8_t c) {
	while (statusOf(PS_BUFFER_FULL)) {
		updateState();
		logChangedStatus();
	}
	CSN_Thermal::write(c);
	return 1;
}

/* default implementation: may be overridden */
size_t Printer::write(const char *buffer, size_t size) {
	this->printing = true;
	size_t n = 0;
	if (Manager::getInstance().dataManager->logPrint) {
		n = size;
	} else {
		while (size--) {
			n += write(*buffer++);
		}
	}
	this->printing = false;
	return n;
}

bool Printer::available() {
	return Serial1.available();
}

void Printer::feed(uint8_t x) {
	if (!Manager::getInstance().dataManager->logPrint) {
		while (x--) {
			write('\n');
		}
	}
}

uint8_t Printer::read() {
	return Serial1.read();
}

/*uint8_t Printer::peek() {
	return Serial1.peek();
}*/

void Printer::setASB(bool aTurnOn) {
	uint8_t setting = 0b00000000;
	if (aTurnOn) {
		setting = 0b00100100;
	}
	writeBytes(29, 97, setting);
}

}