// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2013 Monkey with a Mustache Entertainment, Inc.
// All rights reserved.

#ifndef CDAM_PRINTER_H
#define CDAM_PRINTER_H

// Instead of "Arduino.h"
#include <stdint.h>
//#include <stddef.h>
//#include <stdlib.h>
#include "spark_wiring_stream.h"
#include "spark_wiring_print.h"

#include "cdam_constants.h"
#include "csn_thermal.h"

namespace cdam
{

class Printer : public CSN_Thermal {

public:
    Printer();
    void init();
    void begin(int heatTime=200);
    void printProgStr(const prog_uchar *str);
    void logProgStr(const prog_uchar *str);
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

    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buffer, size_t size);
protected:
    bool ready;
    bool printing;
    // How long we've calculated it will take to print
    unsigned long printTime;
};

}

#endif