// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#ifndef CDAM_PRINTER_H
#define CDAM_PRINTER_H

//#include <stdint.h>
//#include "cdam_constants.h"
#include "csna5_thermal_printer.h"

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

class Printer : public CSN_Thermal {

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
    void printAuthors(char* aAuthor, char* aCredits);
    void printPoints(int16_t aPoints, int16_t aPerfectScore);
    void printContinue(uint8_t aCoinsToContinue);

    void begin(int heatTime=200);

    //void printFile(const char *aPath, bool aWrapped, bool aLinefeed, uint8_t aPrependLen, uint8_t aOffset);
    //int printWrapped(char *aMsg, uint8_t aColumns, bool aBufferMode);


    //void timeoutSet(unsigned long);
    void printTimeSet(unsigned long);
    void timeoutWait();
    void printerWait();

    bool available();
    uint8_t read();
    uint8_t peek();
    void setABS(bool aTurnOn);
    bool isPrinting();
    bool isReady();
    // Number of chars to indent for autojump text.
    uint8_t jumpIndent;

    bool bufferFull;

    bool active;

    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buffer, size_t size);
protected:
    bool ready;
    bool printing;
    // How long we've calculated it will take to print
    unsigned long printTime;
private:
    char _status;
    char _lastStatus;
    bool _statusUpdated;
};

}

#endif