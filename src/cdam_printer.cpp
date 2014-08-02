// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#include "cdam_constants.h"
#include "cdam_manager.h"
#include "cdam_printer.h"
#include "cdam_story_load.h"

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
	this->jumpIndent = 0;
	this->ready = false;
	this->printing = false;
	this->bufferFull = false;
	this->printTime = 0;
	this->active = true;
}

void Printer::updateState() {
	if (this->active) {
		if (Printer::available()) {
			_lastStatus = _status;
			_status = Printer::read();
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
	if (statusChanged(PS_ONLINE)) {
		if (statusOf(PS_ONLINE)) {
			DEBUG("Status: Online");
		} else {
			DEBUG("Status: Offline");
		}
	}
	if (statusChanged(PS_BUFFER_FULL)) {
		if (statusOf(PS_BUFFER_FULL)) {
			DEBUG("Status: Buffer Full");
		} else {
			DEBUG("Status: Buffer Not Full");
		}
	}
	if (statusChanged(PS_NO_PAPER)) {
		if (statusOf(PS_NO_PAPER)) {
			DEBUG("Status: No Paper");
		} else {
			DEBUG("Status: Paper");
		}
	}
	if (statusChanged(PS_HIGH_VOLTAGE)) {
		if (statusOf(PS_HIGH_VOLTAGE)) {
			DEBUG("Status: High Voltage");
		} else {
			DEBUG("Status: Normal Voltage");
		}
	}
	if (statusChanged(PS_UNKNOWN_ONE)) {
		if (statusOf(PS_UNKNOWN_ONE)) {
			DEBUG("Status: Unknown One ON");
		} else {
			DEBUG("Status: Unknown One OFF");
		}
	}
	if (statusChanged(PS_UNKNOWN_TWO)) {
		if (statusOf(PS_UNKNOWN_TWO)) {
			DEBUG("Status: Unknown Two ON");
		} else {
			DEBUG("Status: Unknown Two OFF");
		}
	}
	if (statusChanged(PS_HIGH_TEMP)) {
		if (statusOf(PS_HIGH_TEMP)) {
			DEBUG("Status: High Temp");
		} else {
			DEBUG("Status: Normal Temp");
		}
	}
	if (statusChanged(PS_UNKNOWN_THREE)) {
		if (statusOf(PS_UNKNOWN_THREE)) {
			DEBUG("Status: Unknown Three ON");
		} else {
			DEBUG("Status: Unknown Three OFF");
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
	CSN_Thermal::justify('C');

	println(CDAM_PRESS_BUTTON);

	CSN_Thermal::justify('L');
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

void Printer::printMenu(char *aStories) {
	if (aStories == NULL) {
		println(CDAM_EMPTY);
		return;
	}

	println(CDAM_START);

	println(aStories);
	feed(1);
}

void Printer::printAuthors(char* aAuthor, char* aCredits) {
	CSN_Thermal::boldOn();
	CSN_Thermal::justify('C');

	char buffer[31];
	snprintf(buffer, sizeof(buffer), CDAM_AUTHOR, aAuthor);

	println(buffer);

	if (sizeof(aCredits) > 0) {
		DEBUG("Printing credits...");
		snprintf(buffer, sizeof(buffer), CDAM_CREDITS, aCredits);
	}

	CSN_Thermal::justify('L');
	CSN_Thermal::boldOff();
}

void Printer::printPoints(int16_t aPoints, int16_t aPerfectScore) {
	CSN_Thermal::boldOn();
	CSN_Thermal::justify('C');

	char buffer[20];
	if (aPerfectScore) { // 0 means there isn't a perfect score.
		snprintf(buffer, sizeof(buffer), CDAM_POINTS_OUT_OF, aPoints, aPerfectScore);
		println(buffer);

		if (aPoints == aPerfectScore) {
			println(CDAM_POINTS_PERFECT);
		}
	} else {
		snprintf(buffer, sizeof(buffer), CDAM_POINTS, aPoints);
		println(buffer);
	}

	CSN_Thermal::justify('L');
	CSN_Thermal::boldOff();
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

/*bool Printer::statusUnknownOne() {
	return IsBitSet(_status, PS_UNKNOWN_ONE);
}

bool Printer::statusHighTemp() {
	return IsBitSet(_status, PS_HIGH_TEMP);
}

bool Printer::statusUnknownTwo() {
	return IsBitSet(_status, PS_UNKNOWN_TWO);
}

bool Printer::statusUnknownThree() {
	return IsBitSet(_status, PS_UNKNOWN_THREE);
}

bool Printer::statusHighVoltage() {
	return IsBitSet(_status, PS_HIGH_VOLTAGE);
}

bool Printer::statusNoPaper() {
	return IsBitSet(_status, PS_NO_PAPER);
}

bool Printer::statusBufferFull() {
	return IsBitSet(_status, PS_BUFFER_FULL);
}

bool Printer::statusOnline() {
	return IsBitSet(_status, PS_ONLINE);
}*/

// This method sets the estimated completion time for a just-issued task.
/*void Printer::timeoutSet(unsigned long x) {
  //Serial.println("Setting Timeout");
  resumeTime = micros() + x;
}*/

// This function waits (if necessary) for the prior task to complete.
void Printer::timeoutWait() {
#if (OFFLINE == 1)
	while((long)(micros() - resumeTime) < 0L); // Rollover-proof
#endif
}

void Printer::begin(int heatTime) {
	CSN_Thermal::begin(heatTime);
	this->ready = true;
}

// Override for easier print debugging.
size_t Printer::write(uint8_t c) {
#if (PRINT >= 1)
	CSN_Thermal::write(c);
#else
	Serial.println(c);
#endif
	return 1;
}

/* default implementation: may be overridden */
size_t Printer::write(const uint8_t *buffer, size_t size)
{
	this->printing = true;
	size_t n = 0;
	while (size--) {
		n += write(*buffer++);
	}
	this->printing = false;
	return n;
}

bool Printer::isPrinting() {
	return this->printing;
}

bool Printer::isReady() {
	return this->ready;
}

// given a PROGMEM string, use Serial.print() to send it out
// this is needed to save precious memory
// thanks to todbot for this http://todbot.com/blog/category/programming/
void Printer::printProgStr(const unsigned char *str) {
	char c;
	if (!str) {
		return;
	}
	//IsBitSet(aMetadata->flags.flag2, 4)
	/*if (Manager::getInstance().dataManager->metadata.flags.flag2.logPrint) {

	}*/
	while (c = (*str)) {

#if (PRINT >= 1)
		print(c);
#else
		Serial.print(c);
#endif
		str++;
	}

#if (PRINT >= 1)
	println("");
#else
	Serial.println("");
#endif
}

void Printer::logProgStr(const unsigned char *str) {
	if (!str) {
		return;
	}
	DEBUG("%s", str);
}

// Load up to the max number of bytes, print, and repeat until end of file.
void Printer::printFile(const char *aPath, boolean aWrapped, boolean aLinefeed, byte aPrependLen, byte aOffset = 0) {
  //CSN_Thermal::wake();
	unsigned long fileSize = StoryLoad::getFileSize(aPath);
	if (fileSize == 0) {
		DEBUG("File has no size!");
		return;
	}
	fileSize -= aOffset;
	unsigned long index = aOffset;
	byte indent = 0;
	if (aWrapped) {
		indent = aPrependLen;
	}

	unsigned int numToRead = 0;

	while (fileSize > 0) {
		if (indent > 0) {
      // Read one beyond the line max in case it's a space.
			numToRead = kPrinterColumns - indent + 1;
		} else {
			numToRead = kMaxPassageBytes;
		}
		char *text = StoryLoad::getStringFromFile(aPath, numToRead, index);
		unsigned int length = strlen(text);

#if (DEBUG && MEMORY)
		slog("\t* Free Memory: ");
		DEBUG(freeMemory());
		slog("\t* Text Bytesize: ");
		DEBUG(fileSize);
#endif

		fileSize -= length;
		index += length;
		if (aWrapped) {
      // Receive the length of the last line.
			this->jumpIndent = printWrapped(text, kPrinterColumns - indent, true);
			indent = strlen(text);
			fileSize += indent;
			index -= indent;
			indent = 0;
		} else {
			this->jumpIndent = 0;
			print(text);
      //delay(1000);
		}
		free(text);
    //delay(1000);
	}

	if (aLinefeed) {
		if (aOffset > 0) {
      // Printing a choice, only 1 feed.
			feed(1);
		} else {
      // Printing a passage, 2 feeds.
			feed(2);
		}
	} else {
    // Should be a autojump.
		print(" ");
	}
  //slog(F("Jump indent: "));
  //DEBUG(this->jumpIndent);
  //CSN_Thermal::sleep();
}

//
int Printer::printWrapped(char *aMsg, byte aColumns, boolean aBufferMode) {
	DEBUG(aMsg);
	int length = strlen(aMsg);
  // Not long enough to wrap.
	if (length < aColumns) {
    //DEBUG(F("Short line"));
		print(aMsg);
		aMsg[0] = '\0';
	} else {
    // Wrap until we have less than one full line.
		while (length >= aColumns) {
			byte foundBreak = 0;
			byte i;
      // Search for a newline to break on.
			for (i = 0; i < aColumns; ++i) {
				if (aMsg[i] == '\t') {
					aMsg[i] = ' ';
          //DEBUG(F("[W] Found tab."));
				}
				if (aMsg[i] == '\r') {
          //DEBUG(F("[W] Found carriage return."));
				}
				if (aMsg[i] == '\n') {
					foundBreak = 1;
					aMsg[i] = '\0';
					println(aMsg);
          //delay(1000);
					length -= (i + 1);
          // Usage: memmove(destination, source, length);
					memmove(aMsg, aMsg + i + 1, length);
					break;
				}
			}

      // Search for a space to break on.
			if (foundBreak == 0) {
				for (i = aColumns; i > 0; --i) {
					if (aMsg[i] == ' ') {
						foundBreak = 1;
						aMsg[i] = '\0';
						println(aMsg);
            //delay(1000);
						length -= (i + 1);
            // Usage: memmove(destination, source, length);
						memmove(aMsg, aMsg + i + 1, length);
						break;
					}
				}
			}

      // There were no spaces in the line.
			if (foundBreak == 0) {
        // Save the char we will set to null.
				byte saved = aMsg[aColumns];
        // Set the null so we only print one line.
				aMsg[aColumns] = '\0';
				println(aMsg);
        //delay(1000);
        // Replace the char.
				aMsg[aColumns] = saved;
        // Subtract the # of chars printed.
				length -= aColumns;
        // Move the remaining chars (length) starting at a pointer
        // to the first char not printed (aMsg + aColumns) to a pointer
        // to the beginning of the char array's memory space (aMsg).
        // void *memmove(void *s1, const void *s2, size_t n);
        // memmove(destination, source, length);
				memmove(aMsg, aMsg + aColumns, length);
			}
		}
    //slog(F("Last bit length: "));
		DEBUG("Last bit length: %d", length);
		aMsg[length] = ASCII_NULL;
		if (!aBufferMode) {
			print(aMsg);
		}
	}
	return length;
}

boolean Printer::available() {
	return Serial1.available();
}

byte Printer::read() {
	return Serial1.read();
}

byte Printer::peek() {
	return Serial1.peek();
}

void Printer::setPrinting(bool aPrinting) {
  /*if (aPrinting) {
    //__disable_irq();
    DEBUG("PRINTING!");
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, HIGH);
  } else {
    //__enable_irq();
    DEBUG("NO PRINTING");
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
  }
  this->printing = aPrinting;*/
}

void Printer::setABS(bool aTurnOn) {
	byte setting = 0b00000000;
	if (aTurnOn) {
		setting = 0b00100100;
	}
	writeBytes(29, 97, setting);
}

void Printer::bufferStatus() {
	char stat;
	char bit;

	if (Printer::available()) {
		stat = Printer::read();
		bit = stat & 0b000010;
    if (bit == 0b000010) { // If yes, buffer full?
    	DEBUG("?1: yes");
    	this->bufferFull = true;
    } else if (bit == 0b000000){
    	DEBUG("?1: no");
    	this->bufferFull = false;
    }
}
}

void Printer::printerStatus() {
  	//writeBytes(27, 118, 0);

	char stat;
	char bit;

	if (Printer::available()) {
		stat = Printer::read();

		bit = stat & 0b000001;
		if (bit == 0b000001) {
			DEBUG("Online: yes");
		} else if (bit == 0b000000){
			DEBUG("Online: no");
		}

		bit = stat & 0b000010;
    if (bit == 0b000010) { // If yes, buffer full
    	DEBUG("?1: yes");
    } else if (bit == 0b000000){
    	DEBUG("?1: no");
    }

    // Mask the 3 LSB, this seems to be the one we care about.
    bit = stat & 0b000100;

    // If it's set, no paper, if it's clear, we have paper.
    if (bit == 0b000100) { // CORRECT
    	DEBUG("Paper: no");
    } else if (bit == 0b000000){
    	DEBUG("Paper: yes");
    }

    bit = stat & 0b001000;
    if (bit == 0b001000) { // CORRECT
    	DEBUG("Volt: v > 9.5");
    } else if (bit == 0b000000){
    	DEBUG("Volt: normal");
    }

    bit = stat & 0b010000;
    if (bit == 0b010000) { // Printer is ready maybe?
    	DEBUG("?4: yes");
    } else if (bit == 0b000000){
    	DEBUG("?4: no");
    }

    bit = stat & 0b100000;
    if (bit == 0b100000) {
    	DEBUG("?5: yes");
    } else if (bit == 0b000000){
    	DEBUG("?5: no");
    }

    bit = stat & 0b1000000;
    if (bit == 0b1000000) {
    	DEBUG("Temp: over 60");
    } else if (bit == 0b000000){
    	DEBUG("Temp: normal");
    }

    bit = stat & 0b10000000;
    if (bit == 0b10000000) {
    	DEBUG("?7: yes");
    } else if (bit == 0b000000){
    	DEBUG("?7: no");
    }
    DEBUG("\n");
}
}
}