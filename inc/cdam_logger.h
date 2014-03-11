// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#ifndef CDAM_CONSTANTS_H
#define CDAM_CONSTANTS_H

class Logger
{
  public:
    Logger() {} ; // Constructor does nothing now as no per logger state
    void writeLine(char * line);
    void close() ;

  private:
    void open() ;

    // Static Member variables that implement the singleton
    static MySDFat logFile;
    static char state;
    static int lineCount;

};