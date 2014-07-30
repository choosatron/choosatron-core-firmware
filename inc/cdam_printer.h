// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#ifndef CDAM_PRINTER_H
#define CDAM_PRINTER_H

#include <stdint.h>
#include "spark_wiring_stream.h"
#include "spark_wiring_print.h"

#include "cdam_constants.h"
#include "csna5_thermal_printer.h"

namespace cdam
{

// Printer Status Binary Flags
typedef enum PrinterStatus_t {
    PS_ONLINE = 0,
    PS_BUFFER_FULL,
    PS_NO_PAPER,
    PS_HIGH_VOLTAGE,
    PS_UNKNOWN_ONE,
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
    /*bool statusUnknownOne();
    bool statusHighTemp();
    bool statusUnknownTwo();
    bool statusUnknownThree();
    bool statusHighVoltage();
    bool statusNoPaper();
    bool statusBufferFull();
    bool statusOnline();*/

    void begin(int heatTime=200);
    void printProgStr(const unsigned char *str);
    void logProgStr(const unsigned char *str);
    void printFile(const char *aPath, boolean aWrapped, boolean aLinefeed, byte aPrependLen, byte aOffset);
    int printWrapped(char *aMsg, byte aColumns, boolean aBufferMode);
    void printInsertCoin(byte aNextCredit);
    void printAuthor(byte aStoryId);
    void itoa(int value, char *sp, int radix);
    void printPoints(byte aTotalPoints, byte aPerfectScore);
    void printCoinToContinue();
    void printTitleMenu(char *stories);

    //void timeoutSet(unsigned long);
    void printTimeSet(unsigned long);
    void timeoutWait();
    void printerWait();

    bool available();
    byte read();
    byte peek();
    void setABS(boolean aTurnOn);
    void setPrinting(bool aPrinting);
    bool isPrinting();
    bool isReady();
    void bufferStatus();
    void printerStatus();
    // Number of chars to indent for autojump text.
    byte jumpIndent;

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
};

}

#endif