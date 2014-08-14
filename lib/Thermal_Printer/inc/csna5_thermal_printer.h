/*************************************************************************
  This is an Arduino library for the Adafruit Thermal Printer.
  Pick one up at --> http://www.adafruit.com/products/597
  These printers use TTL serial to communicate, 2 pins are required.

  Adafruit invests time and resources providing this open source code.
  Please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution.
 *************************************************************************/

#ifndef Thermal_h
#define Thermal_h

#include "spark_wiring_usartserial.h"

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

#define SERIAL_IMPL      Serial1
#define PRINTER_PRINT(a) SERIAL_IMPL.write(a);

class CSN_Thermal : public Print {

 public:
  CSN_Thermal();

  virtual void
    init(),
    begin(int heatTime=200),
    reset(),
    setDefault(),
    test(),
    testPage(),

    normal(),
    inverseOn(),
    inverseOff(),
    upsideDownOn(),
    upsideDownOff(),
    doubleHeightOn(),
    doubleHeightOff(),
    doubleWidthOn(),
    doubleWidthOff(),
    boldOn(),
    boldOff(),
    underlineOn(uint8_t weight=1),
    underlineOff(),
    strikeOn(),
    strikeOff(),

    justify(char value),
    feed(uint8_t x=1),
    feedRows(uint8_t),
    flush(),
    online(),
    offline(),
    sleep(),
    sleepAfter(uint8_t seconds),
    wake(),

    setSize(char value),
    setLineHeight(int val=32),

    printBarcode(char * text, uint8_t type),
    setBarcodeHeight(int val=50),

    printBitmap(int w, int h, const uint8_t *bitmap, bool fromProgMem = true),
    printBitmap(int w, int h, Stream *stream),
    printBitmap(Stream *stream),

    timeoutSet(unsigned long),
    timeoutWait(),
    setTimes(unsigned long, unsigned long),
    setMaxChunkHeight(int val),

    setCharSpacing(int spacing), // Not working
    tab();                       // Not working

  bool hasPaper();

  virtual size_t write(uint8_t c);

 protected:

  uint8_t
    prevByte,      // Last character issued to printer
    column,        // Last horizontal column printed
    maxColumn,     // Page width (output 'wraps' at this point)
    charHeight,    // Height of characters, in 'dots'
    lineSpacing,   // Inter-line spacing (not line height), in dots
    barcodeHeight; // Barcode height in dots, not including text
  unsigned long
    resumeTime,    // Wait until micros() exceeds this before sending byte
    dotPrintTime,  // Time to print a single dot line, in microseconds
    dotFeedTime;   // Time to feed a single dot line, in microseconds
  int
    printMode,
    maxChunkHeight;
  void
    setPrintMode(uint8_t mask),
    unsetPrintMode(uint8_t mask),
    writePrintMode(),
    writeBytes(uint8_t a),
    writeBytes(uint8_t a, uint8_t b),
    writeBytes(uint8_t a, uint8_t b, uint8_t c),
    writeBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
};

#endif // Thermal_h
