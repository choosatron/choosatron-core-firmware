// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#include "cdam_constants.h"
#include "cdam_manager.h"
#include "cdam_printer.h"

namespace cdam
{

// Barcode types
#define UPC_A   0
#define UPC_E   1
#define EAN13   2
#define EAN8    3
#define CODE39  4
#define I25     5
#define CODEBAR 6
#define CODE93  7
#define CODE128 8
#define CODE11  9
#define MSI    10

// === Character commands ===

#define INVERSE_MASK       (1 << 1)
#define UPDOWN_MASK        (1 << 2)
#define BOLD_MASK          (1 << 3)
#define DOUBLE_HEIGHT_MASK (1 << 4)
#define DOUBLE_WIDTH_MASK  (1 << 5)
#define STRIKE_MASK        (1 << 6)

#define SERIAL_IMPL      Serial1
#define PRINTER_PRINT(a) SERIAL_IMPL.write(a);

#define BYTE_TIME (11L * 1000000L / kPrinterBaudRate)

/* Public Methods */

Printer::Printer() {
}

void Printer::initialize() {
   this->writing = false;
   this->active = true;

   _prevByte = 0;
   _column = 0;
   _maxColumn = 0;
   _charHeight = 0;
   _lineSpacing = 0;
   _barcodeHeight = 0;
   _printMode = 0;
   _maxChunkHeight = 0;
   _dotPrintTime = 0;
   _dotFeedTime = 0;
   //_resumeTime = 0;
}

void Printer::updateState() {
   if (this->active) {
      if (available()) {
         _lastStatus = _status;
         _status = read();
         _statusUpdated = true;
         /*DEBUG("0x%c%c%c%c%c%c%c%c",
            (IsBitSet(_status, 7) ? '1' : '0'),
            (IsBitSet(_status, 6) ? '1' : '0'),
            (IsBitSet(_status, 5) ? '1' : '0'),
            (IsBitSet(_status, 4) ? '1' : '0'),
            (IsBitSet(_status, 3) ? '1' : '0'),
            (IsBitSet(_status, 2) ? '1' : '0'),
            (IsBitSet(_status, 1) ? '1' : '0'),
            (IsBitSet(_status, 0) ? '1' : '0'));*/
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
   /*if (_statusUpdated) {
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
   }*/
}

/* Hard Coded Printing */
#ifndef DEBUG_BUILD
#if BIZ_CARD
void Printer::printBizCard() {
   print('/');
   print(CDAM_LINE);
   println('\\');
   println(CDAM_JERRY_1);
   println(CDAM_JERRY_2);
   println(CDAM_JERRY_3);
   print('|');
   print(CDAM_LINE);
   println('|');
   println(CDAM_CONTACT_1);
   println(CDAM_CONTACT_2);
   print('|');
   print(CDAM_LINE);
   println('|');
   println(CDAM_CONTACT_3);
   print('|');
   print(CDAM_LINE);
   println('|');
   println(CDAM_WHAT_1);
   println(CDAM_WHAT_2);
   println(CDAM_BULLET_1);
   println(CDAM_BULLET_2);
   println(CDAM_BULLET_3);
   println(CDAM_BULLET_4);
   println(CDAM_BULLET_5);
   println(CDAM_BULLET_6);
   print('\\');
   print(CDAM_LINE);
   println('/');
   println("\n");
}
#endif
#endif

void Printer::printInsertCoin(uint8_t aCoins, uint8_t aCoinsPerCredit) {
   inverseOn();
   justify('C');

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
   inverseOff();
   justify('L');
   feed(2);
}

void Printer::printPressButton() {
   boldOn();
   justify('C');

   println(CDAM_PRESS_BUTTON);

   boldOff();
   justify('L');
   feed(2);
}

void Printer::printTitle() {
   boldOn();
   justify('C');
   doubleHeightOn();

   println(CDAM_TITLE);

   doubleHeightOff();

   println(CDAM_SUBTITLE);

   boldOff();
   justify('L');
}

void Printer::printEmpty() {
   boldOn();
   feed(1);
   println(CDAM_EMPTY);
   feed(1);
   boldOff();
}

void Printer::printStart() {
   justify('C');

   println(CDAM_START);

   justify('L');
}

void Printer::printStoryIntro(char* aTitle, char* aAuthor) {
   boldOn();
   justify('C');

   wrapText(aTitle, kPrinterColumns);
   wrapText(aAuthor, kPrinterColumns);
   println(aTitle);

   boldOff();
   println(CDAM_STORY_BY);
   println(aAuthor);
   justify('L');
}

/*void Printer::printAuthors(char* aAuthor, char* aCredits) {
   boldOn();
   justify('C');

   char buffer[31];
   snprintf(buffer, sizeof(buffer), CDAM_STORY_BY, aAuthor);

   println(buffer);

   if (sizeof(aCredits) > 0) {
      DEBUG("Printing credits...");
      snprintf(buffer, sizeof(buffer), CDAM_CREDITS, aCredits);
   }

   justify('L');
   boldOff();
}*/

void Printer::printBigNumbers() {
   boldOn();
   println(CDAM_BIG_NUMBERS);
   boldOff();
   feed(1);
}

void Printer::printContinue(uint8_t aCoinsToContinue) {
   boldOn();
   justify('C');

   char buffer[32];
   snprintf(buffer, sizeof(buffer), CDAM_ARCADE_CONTINUE, aCoinsToContinue);
   println(buffer);

   boldOff();
   justify('L');
   feed(2);
}

void Printer::printEnding(char* aCredits, char* aContact) {
   boldOn();
   justify('C');

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
   boldOff();
   println(CDAM_TEAR_GENTLY);
   justify('L');
   feed(2);
}

void Printer::printAdminTitle() {
   boldOn();
   justify('C');
   print(CDAM_TITLE);
   println(" Admin\n# toggles, hold [1] to exit.\n");
   boldOff();
}

void Printer::printAdminMenu() {
   justify('L');
   printAdminOne();
   printAdminTwo();
   printAdminThree();
   //printAdminFour();
   feed(2);
}

void Printer::printAdminOne() {
   print("1. Offline Mode:   ");
   if (Manager::getInstance().dataManager->metadata.flags.offline) {
      println(CDAM_ADMIN_ON);
   } else {
      println(CDAM_ADMIN_OFF);
   }
}

void Printer::printAdminTwo() {
   print("2. SD Default:     ");
   if (Manager::getInstance().dataManager->metadata.flags.sdCard) {
      println(CDAM_ADMIN_ON);
   } else {
      println(CDAM_ADMIN_OFF);
   }
}

void Printer::printAdminThree() {
   print("3. Random Stories: ");
   if (Manager::getInstance().dataManager->metadata.flags.random) {
      println(CDAM_ADMIN_ON);
   } else {
      println(CDAM_ADMIN_OFF);
   }
}

/*void Printer::printAdminFour() {
   print("4. Arcade Mode:    ");
   if (Manager::getInstance().dataManager->metadata.flags.arcade) {
      println(CDAM_ADMIN_ON);
   } else {
      println(CDAM_ADMIN_OFF);
   }
}*/

/* END - Hard Coded Printing */

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

void Printer::setBarcodeHeight(int aValue) { // Default is 50
   if (aValue < 1) {
      aValue = 1;
   }
   _barcodeHeight = aValue;
   writeBytes(29, 104, aValue);
}

void Printer::printBarcode(char *aText, uint8_t aType) {
   int  i = 0;
   byte c;

   writeBytes(29,  72, 2);     // Print label below barcode
   writeBytes(29, 119, 3);     // Barcode width
   writeBytes(29, 107, aType); // Barcode type (listed in .h file)
   do { // Copy string + NULL terminator
      writeBytes(c = aText[i++]);
   } while(c);
   feed(2);
}

void Printer::printBitmap(int aWidth, int aHeight, const uint8_t *aBitmap) {
   int rowBytes, rowBytesClipped, rowStart, chunkHeight, x, y, i;

   rowBytes = (aWidth + 7) / 8; // Round up to next byte boundary
   rowBytesClipped = (rowBytes >= 48) ? 48 : rowBytes; // 384 pixels max width

   for (i = rowStart = 0; rowStart < aHeight; rowStart += _maxChunkHeight) {
      // Issue up to 255 rows at a time:
      chunkHeight = aHeight - rowStart;
      if (chunkHeight > _maxChunkHeight) {
         chunkHeight = _maxChunkHeight;
      }

      writeBytes(18, 42, chunkHeight, rowBytesClipped);

      for (y = 0; y < chunkHeight; y++) {
         for (x = 0; x < rowBytesClipped; x++, i++) {
            PRINTER_PRINT(*(aBitmap + i));
         }
         i += rowBytes - rowBytesClipped;
      }
   }
   _prevByte = '\n';
}

void Printer::printBitmap(int aWidth, int aHeight, Stream *aStream) {
   int rowBytes, rowBytesClipped, rowStart, chunkHeight, x, y, i, c;

   rowBytes = (aWidth + 7) / 8; // Round up to next byte boundary
   rowBytesClipped = (rowBytes >= 48) ? 48 : rowBytes; // 384 pixels max width

   for (rowStart = 0; rowStart < aHeight; rowStart += _maxChunkHeight) {
      // Issue up to 255 rows at a time:
      chunkHeight = aHeight - rowStart;
      if(chunkHeight > _maxChunkHeight) chunkHeight = _maxChunkHeight;

      writeBytes(18, 42, chunkHeight, rowBytesClipped);

      for (y = 0; y < chunkHeight; y++) {
         for (x = 0; x < rowBytesClipped; x++) {
            while ((c = aStream->read()) < 0);
            PRINTER_PRINT((uint8_t)c);
         }
         for (i = rowBytes - rowBytesClipped; i > 0; i--) {
            while ((c = aStream->read()) < 0);
         }
      }
   }
   _prevByte = '\n';
}

void Printer::printBitmap(Stream *aStream) {
   uint8_t  tmp;
   uint16_t width, height;

   tmp =  aStream->read();
   width = (aStream->read() << 8) + tmp;

   tmp =  aStream->read();
   height = (aStream->read() << 8) + tmp;

   printBitmap(width, height, aStream);
}

void Printer::begin(int aHeatTime) {
   SERIAL_IMPL.begin(kPrinterBaudRate);

   //timeoutSet(500000L);

   wake();
   reset();

   // Description of print settings from page 23 of the manual:
   // ESC 7 n1 n2 n3 Setting Control Parameter Command
   // Decimal: 27 55 n1 n2 n3
   // Set "max heating dots", "heating time", "heating interval"
   // n1 = 0-255 Max printing dots, Unit (8dots), Default: 7 (64 dots)
   // n2 = 3-255 Heating time, Unit (10us), Default: 80 (800us)
   // n3 = 0-255 Heating interval, Unit (10us), Default: 2 (20us)
   // The more max heating dots, the more peak current will cost
   // when printing, the faster printing speed. The max heating
   // dots is 8*(n1+1).  The more heating time, the more density,
   // but the slower printing speed.  If heating time is too short,
   // blank page may occur.  The more heating interval, the more
   // clear, but the slower printing speed.

   writeBytes(27, 55);   // Esc 7 (print settings)
   writeBytes(kPrinterMaxHeatingDots);       // Heating dots (20=balance of darkness vs no jams)
   writeBytes(aHeatTime); // Library default = 255 (max)
   writeBytes(kPrinterHeatingInterval);      // Heat interval (500 uS = slower, but darker)

   // Description of print density from page 23 of the manual:
   // DC2 # n Set printing density
   // Decimal: 18 35 n
   // D4..D0 of n is used to set the printing density.  Density is
   // 50% + 5% * n(D4-D0) printing density.
   // D7..D5 of n is used to set the printing break time.  Break time
   // is n(D7-D5)*250us.
   // (Unsure of the default value for either -- not documented)

   writeBytes(18, 35); // DC2 # (print density)
   writeBytes((kPrinterBreakTime << 5) | kPrinterDensity);

   _dotPrintTime = 30000; // See comments near top of file for
   _dotFeedTime  =  2100; // an explanation of these values.
   _maxChunkHeight = 255;
}

bool Printer::available() {
   return SERIAL_IMPL.available();
}

// Take the printer back online. Subsequent print commands will be obeyed.
void Printer::online() {
   writeBytes(27, 61, 1);
}

// Take the printer offline. Print commands sent after this will be
// ignored until 'online' is called.
void Printer::offline() {
   writeBytes(27, 61, 0);
}

// Put the printer into a low-energy state immediately.
void Printer::sleep() {
   sleepAfter(1);
}

// Put the printer into a low-energy state after the given number
// of seconds.
void Printer::sleepAfter(uint8_t aSeconds) {
   writeBytes(27, 56, aSeconds);
}

// Wake the printer from a low-energy state.
void Printer::wake() {
   // Printer may have been idle for a very long time, during which the
   // micros() counter has rolled over.  To avoid shenanigans, reset the
   // timeout counter before issuing the wake command.
   //timeoutSet(0);
   writeBytes(255);
   // Datasheet recomments a 50 mS delay before issuing further commands,
   // but in practice this alone isn't sufficient (e.g. text size/style
   // commands may still be misinterpreted on wake).  A slightly longer
   // delay, interspersed with ESC chars (no-ops) seems to help.
   for (uint8_t i = 0; i < 10; i++) {
      writeBytes(27);
      //timeoutSet(10000L);
   }
}

// Reset printer to default state.
void Printer::reset() {
   _maxColumn     = kPrinterColumns;
   _charHeight    = 24;
   _lineSpacing   = 8;
   _barcodeHeight = 50;
   writeBytes(27, 64);
}

// Reset text formatting parameters.
void Printer::setDefault() {
   online();
   justify('L');
   inverseOff();
   doubleHeightOff();
   setLineHeight(32);
   boldOff();
   underlineOff();
   setBarcodeHeight(50);
   setSize('S');
}

void Printer::setTimes(uint32_t aPrintTime, uint32_t aFeedTime) {
   _dotPrintTime = aPrintTime;
   _dotFeedTime  = aFeedTime;
}

void Printer::setASB(bool aTurnOn) {
   uint8_t setting = 0b00000000;
   if (aTurnOn) {
      setting = 0b00100100;
   }
   writeBytes(29, 97, setting);
}

// 0:USA, 1:France, 2:Germany, 3:UK, 4:Denmark1
// 5:Sweden, 6:Italy, 7:Spain1, 8:Japan, 9:Norway
// 10:Denmark2, 11:Spain2, 12:Latin American, 13:Korea
void Printer::setCharSet(uint8_t aIndex) {
   writeBytes(27, 82, aIndex);
}

// Feed by the specified number of lines.
void Printer::feed(uint8_t aLines) {
   if (!Manager::getInstance().dataManager->logPrint) {
      // Datasheed claims sending bytes 27, 100, <x> will work,
      // but it feeds much more than that, so we're doing it manually.
      while (aLines--) {
         write('\n');
      }
   }
}

// Feeds by the specified number of individual pixel rows.
void Printer::feedRows(uint8_t aRows) {
   writeBytes(27, 74, aRows);
}

void Printer::flush() {
   writeBytes(12);
}

void Printer::setSize(char aValue) {
   uint8_t size = 0;

   switch(toupper(aValue)) {
      default: // Small: standard width and height
         size       = 0x00;
         _charHeight = 24;
         _maxColumn  = 32;
         break;
      case 'M': // Medium: double height
         size       = 0x01;
         _charHeight = 48;
         _maxColumn  = 32;
         break;
      case 'L': // Large: double width and height
         size       = 0x11;
         _charHeight = 48;
         _maxColumn  = 16;
         break;
   }

   writeBytes(29, 33, size, 10);
   _prevByte = '\n';
}

void Printer::justify(char aValue) {
   uint8_t justify = 0;

   switch(toupper(aValue)) {
      case 'L': justify = 0; break;
      case 'C': justify = 1; break;
      case 'R': justify = 2; break;
   }

   writeBytes(0x1B, 0x61, justify);
}

void Printer::normal() {
   _printMode = 0;
   writePrintMode();
}

void Printer::inverseOn() {
   setPrintMode(INVERSE_MASK);
}

void Printer::inverseOff() {
   unsetPrintMode(INVERSE_MASK);
}

void Printer::upsideDownOn() {
   setPrintMode(UPDOWN_MASK);
}

void Printer::upsideDownOff() {
   unsetPrintMode(UPDOWN_MASK);
}

void Printer::doubleHeightOn() {
   setPrintMode(DOUBLE_HEIGHT_MASK);
}

void Printer::doubleHeightOff() {
   unsetPrintMode(DOUBLE_HEIGHT_MASK);
}

void Printer::strikeOn(){
   setPrintMode(STRIKE_MASK);
}

void Printer::strikeOff(){
   unsetPrintMode(STRIKE_MASK);
}

void Printer::boldOn() {
   setPrintMode(BOLD_MASK);
}

void Printer::boldOff() {
   unsetPrintMode(BOLD_MASK);
}

void Printer::underlineOn(uint8_t aWeight) {
   // Underlines of different weights can be produced:
   // 0 - no underline
   // 1 - normal underline
   // 2 - thick underline
   writeBytes(27, 45, aWeight);
}

void Printer::underlineOff() {
   underlineOn(0);
}

/* Accessors */

/* Private Methods */

////////////////////// not working?
void Printer::tab() {
   PRINTER_PRINT(9);
}
void Printer::setCharSpacing(int aSpacing) {
   writeBytes(27, 32, 0, 10);
}
/////////////////////////

// This method sets the estimated completion time for a just-issued task.
/*void Printer::timeoutSet(uint32_t aDuration) {
   _resumeTime = micros() + aDuration;
}*/

// This function waits (if necessary) for the prior task to complete.
/*void Printer::timeoutWait() {
   // BAD NEWS - Should be using buffer full status NOT this.
   //while((long)(micros() - _resumeTime) < 0L); // Rollover-proof
}*/

void Printer::setMaxChunkHeight(int aValue) {
   _maxChunkHeight = aValue;
}

void Printer::setLineHeight(int aValue) {
   if (aValue < 24) {
      aValue = 24;
   }
   _lineSpacing = aValue - 24;

   // The printer doesn't take into account the current text height
   // when setting line height, making this more akin to inter-line
   // spacing.  Default line spacing is 32 (char height of 24, line
   // spacing of 8).
   writeBytes(27, 51, aValue);
}

void Printer::setPrintMode(uint8_t aMask) {
   _printMode |= aMask;
   writePrintMode();
   _charHeight = (_printMode & DOUBLE_HEIGHT_MASK) ? 48 : 24;
   _maxColumn  = (_printMode & DOUBLE_WIDTH_MASK ) ? 16 : 32;
}
void Printer::unsetPrintMode(uint8_t aMask) {
   _printMode &= ~aMask;
   writePrintMode();
   _charHeight = (_printMode & DOUBLE_HEIGHT_MASK) ? 48 : 24;
   _maxColumn  = (_printMode & DOUBLE_WIDTH_MASK ) ? 16 : 32;
}

void Printer::writePrintMode() {
   writeBytes(27, 33, _printMode);
}

uint8_t Printer::read() {
   return SERIAL_IMPL.read();
}

uint8_t Printer::peek() {
   return SERIAL_IMPL.peek();
}

size_t Printer::write(uint8_t c) {
   while (statusOf(PS_BUFFER_FULL)) {
      updateState();
      logChangedStatus();
   }
   if (c != 0x13) { // Strip carriage returns
      //timeoutWait();
      PRINTER_PRINT(c);
      uint32_t d = BYTE_TIME;
      if ((c == '\n') || (_column == _maxColumn)) { // If newline or wrap
         d += (_prevByte == '\n') ?
            ((_charHeight + _lineSpacing) * _dotFeedTime) : // Feed line
            ((_charHeight * _dotPrintTime) + (_lineSpacing * _dotFeedTime)); // Text line
         _column = 0;
         c = '\n'; // Treat wrap as newline on next pass
      } else {
         _column++;
      }
      //timeoutSet(d);
      _prevByte = c;
   }
   return 1;
}

size_t Printer::write(const char *aBuffer, size_t aSize) {
   this->writing = true;
   size_t n = 0;
   if (Manager::getInstance().dataManager->logPrint) {
      n = aSize;
   } else {
      while (aSize--) {
         n += write(*aBuffer++);
      }
   }
   this->writing = false;
   return n;
}

// The next four helper methods are used when issuing configuration
// commands, printing bitmaps or barcodes, etc.  Not when printing text.

void Printer::writeBytes(uint8_t a) {
   //timeoutWait();
   PRINTER_PRINT(a);
   //timeoutSet(BYTE_TIME);
}

void Printer::writeBytes(uint8_t a, uint8_t b) {
   //timeoutWait();
   PRINTER_PRINT(a);
   PRINTER_PRINT(b);
   //timeoutSet(2 * BYTE_TIME);
}

void Printer::writeBytes(uint8_t a, uint8_t b, uint8_t c) {
   //timeoutWait();
   PRINTER_PRINT(a);
   PRINTER_PRINT(b);
   PRINTER_PRINT(c);
   //timeoutSet(3 * BYTE_TIME);
}

void Printer::writeBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
   //timeoutWait();
   PRINTER_PRINT(a);
   PRINTER_PRINT(b);
   PRINTER_PRINT(c);
   PRINTER_PRINT(d);
   //timeoutSet(4 * BYTE_TIME);
}

}