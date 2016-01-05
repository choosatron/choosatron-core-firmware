// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#ifndef CDAM_PRINTER_H
#define CDAM_PRINTER_H

namespace cdam
{

// Printer Status Binary Flags
typedef enum PrinterStatus_t {
	PS_ONLINE = 0,
	PS_BUFFER_FULL,
	PS_NO_PAPER,
	PS_HIGH_VOLTAGE,
	PS_UNKNOWN_ONE, // Triggers when buffer full, doesn't reset when empty. Data loss flag?
	PS_UNKNOWN_TWO,
	PS_HIGH_TEMP,
	PS_UNKNOWN_THREE
} PrinterStatus;

class Printer : public Print {

public:
	Printer();
	void initialize();
	void updateState();
	bool statusOf(PrinterStatus aStatus);
	bool statusChanged(PrinterStatus aStatus);
	void logChangedStatus();

	void printInsertCoin(uint8_t aCoins, uint8_t aCreditValue);
	void printPressButton();
	void printTitle();
	void printEmpty();
	void printStart();
	void printStoryIntro(char* aTitle, char* aAuthor);
	//void printAuthors(char* aAuthor, char* aCredits);
	void printBigNumbers();
	void printContinue(uint8_t aCoinsToContinue);
	void printEnding(char* aCredits, char* aContact);
	void printAdminTitle();
	void printAdminMenu();
	void printAdminOne();
	void printAdminTwo();
	void printAdminThree();
	void printAdminFour();

	uint8_t wrapText(char* aBuffer, uint8_t aColumns, uint8_t aStartOffset = 0);

	void setBarcodeHeight(int aValue);
	void printBarcode(char *aText, uint8_t aType);
	void printBitmap(int aWidth, int aHeight, const uint8_t *aBitmap);
	void printBitmap(int aWidth, int aHeight, Stream *aStream);
	void printBitmap(Stream *aStream);

	void begin(int aHeatTime);
	bool available();
	void online();
	void offline();
	void sleep();
	void sleepAfter(uint8_t aSeconds);
	void wake();
	void reset();
	void setDefault();
	void setTimes(uint32_t aPrintTime, uint32_t aFeedTime);
	void setASB(bool aTurnOn);
	void feed(uint8_t aLines = 1);
	void feedRows(uint8_t aRows);
	void flush();

	void setSize(char aValue);
	void justify(char aValue);
	void normal();
	void inverseOn();
	void inverseOff();
	void upsideDownOn();
	void upsideDownOff();
	void doubleHeightOn();
	void doubleHeightOff();
	void strikeOn();
	void strikeOff();
	void boldOn();
	void boldOff();
	void underlineOn(uint8_t aWeight = 1);
    void underlineOff();

    virtual size_t write(uint8_t c);

	bool active;
	bool writing;

private:
	/////////// Not working?
	void tab();
	void setCharSpacing(int aSpacing);
	///////////
	//void timeoutSet(uint32_t aDuration);
	//void timeoutWait();
	void setMaxChunkHeight(int aValue);
	void setLineHeight(int aValue);
	void setPrintMode(uint8_t aMask);
	void unsetPrintMode(uint8_t aMask);
	void writePrintMode();
	uint8_t read();
	uint8_t peek();

	size_t write(const char *aBuffer, size_t aSize);
	void writeBytes(uint8_t a);
	void writeBytes(uint8_t a, uint8_t b);
	void writeBytes(uint8_t a, uint8_t b, uint8_t c);
	void writeBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

	uint8_t _prevByte;
	uint8_t _column;
	uint8_t _maxColumn;
	uint8_t _charHeight;
	uint8_t _lineSpacing;
	uint8_t _barcodeHeight;
	uint16_t _printMode;
	uint16_t _maxChunkHeight;
	uint32_t _dotPrintTime;
	uint32_t _dotFeedTime;
	//uint32_t _resumeTime;

	char _status;
	char _lastStatus;
	bool _statusUpdated;
};

}

#endif