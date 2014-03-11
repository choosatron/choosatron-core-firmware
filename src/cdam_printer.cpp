// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#include "cdam_printer.h"
#include "cdam_story_load.h"
#include "memory_free.h"

#include "spark_wiring_usartserial.h"

namespace cdam
{

Printer::Printer() : CSN_Thermal::CSN_Thermal() {
}

void Printer::init() {
  CSN_Thermal::init();
  this->jumpIndent = 0;
  this->ready = false;
  this->printing = false;
  this->bufferFull = false;
  this->printTime = 0;
}

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
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
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
void Printer::printProgStr(const prog_uchar *str) {
  char c;
  if (!str) {
    return;
  }
  while (c = pgm_read_byte(str)) {
#if (PRINT >= 1)
    print(c);
#else
    slog(c);
#endif
    str++;
  }

#if (PRINT >= 1)
  println("");
#else
  slogln("");
#endif
}

void Printer::logProgStr(const prog_uchar *str) {
  char c;
  if (!str) {
    return;
  }
  while (c = pgm_read_byte(str)) {
    slog(c);
    str++;
  }
  slogln("");
}

// Load up to the max number of bytes, print, and repeat until end of file.
void Printer::printFile(const char *aPath, boolean aWrapped, boolean aLinefeed, byte aPrependLen, byte aOffset = 0) {
  //CSN_Thermal::wake();
  unsigned long fileSize = StoryLoad::getFileSize(aPath);
  if (fileSize == 0) {
    slogln("File has no size!");
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
      numToRead = PRINT_COLUMNS - indent + 1;
    } else {
      numToRead = MAX_PASSAGE_BYTES;
    }
    char *text = StoryLoad::getStringFromFile(aPath, numToRead, index);
    unsigned int length = strlen(text);

#if (DEBUG && MEMORY)
    slog("\t* Free Memory: ");
    slogln(freeMemory());
    slog("\t* Text Bytesize: ");
    slogln(fileSize);
#endif

    fileSize -= length;
    index += length;
    if (aWrapped) {
      // Receive the length of the last line.
      this->jumpIndent = printWrapped(text, PRINT_COLUMNS - indent, true);
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
  //slogln(this->jumpIndent);
  //CSN_Thermal::sleep();
}

//
int Printer::printWrapped(char *aMsg, byte aColumns, boolean aBufferMode) {
  slogln(aMsg);
  int length = strlen(aMsg);
  // Not long enough to wrap.
  if (length < aColumns) {
    //slogln(F("Short line"));
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
          //slogln(F("[W] Found tab."));
        }
        if (aMsg[i] == '\r') {
          //slogln(F("[W] Found carriage return."));
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
    slogln(length);
    aMsg[length] = '\0';
    if (!aBufferMode) {
      print(aMsg);
      //delay(1000);
    }
  }
  return length;
}

void Printer::printInsertCoin(byte aNextCredit) {
  //CSN_Thermal::wake();
  CSN_Thermal::inverseOn();
  CSN_Thermal::justify('C');
  if (aNextCredit == 0) {
    //printProgStr(INSERT_COINS);
    print("CREDITS 0 - INSERT ");
    print(COINS_PER_CREDIT);
    println(" COIN(S)");
  } else {
    print("CREDITS ");
    print(aNextCredit);
    print("/");
    print(COINS_PER_CREDIT);
    println(" - INSERT COIN(S)");
  }
  CSN_Thermal::inverseOff();
  CSN_Thermal::justify('L');
  feed(2);
  delay(100);
  //CSN_Thermal::sleep();
}

void Printer::printAuthor(byte aStoryId) {
  char path[4] = {'\0'};
  path[0] = aStoryId;
  path[1] = '/';
  path[2] = 'A';
  unsigned long fileSize = StoryLoad::getFileSize(path);
  char *author = StoryLoad::getStringFromFile(path, fileSize, 0);
  CSN_Thermal::boldOn();
  print("Story by ");
  println(author);
  CSN_Thermal::boldOff();
  free(author);
}

void Printer::itoa(int value, char *sp, int radix)
{
    char tmp[16];// be careful with the length of the buffer
    char *tp = tmp;
    int i;
    unsigned v;
    int sign;

    sign = (radix == 10 && value < 0);
    if (sign)   v = -value;
    else    v = (unsigned)value;

    while (v || tp == tmp)
    {
        i = v % radix;
        v /= radix; // v/=radix uses less CPU clocks than v=v/radix does
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    if (sign)
    *sp++ = '-';
    while (tp > tmp)
    *sp++ = *--tp;
}

void Printer::printPoints(byte aTotalPoints, byte aPerfectScore) {
  if (aTotalPoints > 0) {

    //CSN_Thermal::justify('C');
    println();
    CSN_Thermal::boldOn();
    char score[31] = {'\0'};

    Printer::itoa(aTotalPoints, score, 10);
    unsigned int index = strlen(score);
    if ((aPerfectScore > 0) && (aTotalPoints <= aPerfectScore)) {
      score[index] = ' ';
      score[index+1] = '/';
      score[index+2] = ' ';
      Printer::itoa(aPerfectScore, score + index + 3, 10);
      index = strlen(score);
    }
    //memcpy(score + index, " Points", 7);
    //score[index+6] = '\0';
    //strcpy(score + index, " Points");

    score[index] = ' ';
    score[index+1] = 'P';
    score[index+2] = 'o';
    score[index+3] = 'i';
    score[index+4] = 'n';
    score[index+5] = 't';
    score[index+6] = 's';
    score[index+7] = '\0';

    println(score);
    CSN_Thermal::boldOff();
    return;
    // TRY BUILDING OUT STRING AND THEN PRINTING IT CENTERED
    /*print(_totalPoints);
    if ((_perfectScore > 0) && (_totalPoints <= _perfectScore)) {
      print(" / ");
      print(_perfectScore);
    }
    println(F(" Points"));*/
    CSN_Thermal::boldOff();
   // CSN_Thermal::justify('L');
    delay(100);
  }
}

void Printer::printCoinToContinue() {
  CSN_Thermal::boldOn();
  //CSN_Thermal::justify('C');
  print("**INSERT ");
  print(COINS_TO_CONTINUE);
  println(" COIN(S) TO CONTINUE**");
  CSN_Thermal::boldOff();
  //CSN_Thermal::justify('L');
  feed(2);
  delay(100);
}

void Printer::printTitleMenu(char *stories) {
  //CSN_Thermal::wake();
  CSN_Thermal::boldOn();
  delay(50);
  CSN_Thermal::justify('C');
  delay(50);
  CSN_Thermal::doubleHeightOn();
  delay(50);
  println(CDAM_TITLE);
  //println("Choosatronz");
  CSN_Thermal::doubleHeightOff();
  println(CDAM_SUBTITLE);
  //println("Deluxe Adventure Matrix v1.0\nby Jerry Belich - choosatron.com\n\nChoose your story below!\n");
  //delay(100);
  //printProgStr(CDAM_INTRO);
  CSN_Thermal::boldOff();
  CSN_Thermal::justify('L');
  delay(100);
  println(stories);
  feed(1);
  delay(100);
  //CSN_Thermal::sleep();
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
    slogln("PRINTING!");
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, HIGH);
  } else {
    //__enable_irq();
    slogln("NO PRINTING");
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
  }
  this->printing = aPrinting;*/
}

void Printer::setABS(boolean aTurnOn) {
  byte setting = 0b00000000;
  if (aTurnOn) {
    setting = 0b00100100;
    //setting = 0b00000000;
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
      slogln("?1: yes");
      this->bufferFull = true;
    } else if (bit == 0b000000){
      slogln("?1: no");
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
      slogln("Online: yes");
    } else if (bit == 0b000000){
      slogln("Online: no");
    }

    bit = stat & 0b000010;
    if (bit == 0b000010) { // If yes, buffer full
      slogln("?1: yes");
    } else if (bit == 0b000000){
      slogln("?1: no");
    }

    // Mask the 3 LSB, this seems to be the one we care about.
    bit = stat & 0b000100;

    // If it's set, no paper, if it's clear, we have paper.
    if (bit == 0b000100) { // CORRECT
      slogln("Paper: no");
    } else if (bit == 0b000000){
      slogln("Paper: yes");
    }

    bit = stat & 0b001000;
    if (bit == 0b001000) { // CORRECT
      slogln("Volt: v > 9.5");
    } else if (bit == 0b000000){
      slogln("Volt: normal");
    }

    bit = stat & 0b010000;
    if (bit == 0b010000) { // Printer is ready maybe?
      slogln("?4: yes");
    } else if (bit == 0b000000){
      slogln("?4: no");
    }

    bit = stat & 0b100000;
    if (bit == 0b100000) {
      slogln("?5: yes");
    } else if (bit == 0b000000){
      slogln("?5: no");
    }

    bit = stat & 0b1000000;
    if (bit == 0b1000000) {
      slogln("Temp: over 60");
    } else if (bit == 0b000000){
      slogln("Temp: normal");
    }

    bit = stat & 0b10000000;
    if (bit == 0b10000000) {
      slogln("?7: yes");
    } else if (bit == 0b000000){
      slogln("?7: no");
    }
    slogln("\n");
    }
  }
}