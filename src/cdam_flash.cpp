#include "cdam_flash.h"
#include <cstring>

/* Global variables ----------------------------------------------------------*/
static uint32_t baseAddress = 0x80000;
static uint32_t maxAddress = 0x17FFFF;

void Flash::init(void) {
    sFLASH_Init();
}

/**
 * Read two bytes of data from the Flash
 * @param address The Flash address to begin reading, from 0 to 0x17FFFF
 * @return The retrieved data if successful, 0 otherwise
 */
uint8_t Flash::readByte(uint32_t address) {
    uint8_t value;
    if (readBytes(&value, address, 1)) {
        return value;
    }
    return 0;
}

/**
 * Write two bytes of data to the Flash
 * @param address The Flash address to begin writing, from 0 to 0x17FFFF
 * @return true if successful, false otherwise
 */
bool Flash::writeByte(uint32_t address, uint8_t value) {
    return writeBytes(&value, address, 1);
}

/**
 * Read bytes from the Flash
 * @param buffer  The address of the buffer which will receive the data
 * @param address The Flash address to begin reading, from 0 to 0x17FFFF
 * @param length  The number of bytes to read
 * @return true if successful, false otherwise
 */
bool Flash::readBytes(uint8_t *buffer, uint32_t address, uint32_t length) {
    // For odd addresses, read back one byte and extend the length
    uint8_t addressOffset = address&1;
    if (addressOffset) {
        address--;
        length++;
    }

    // Extend the length if it's odd
    uint8_t lengthAdjustment = length&1;
    if (lengthAdjustment) {
        length++;
    }

    // Check for errors after adjustment
    if (address + length > maxAddress) {
        return false;
    }

    // Use a temporary buffer to read the data
    uint8_t *readBuffer = new uint8_t[length];

    // Read the data
    sFLASH_ReadBuffer(readBuffer, baseAddress + address, length);

  // Copy to destination, compensating for shifted address and adjusted length
  memcpy(buffer, readBuffer + addressOffset, length - addressOffset - lengthAdjustment);

  return true;
}

/**
 * Write bytes to the Flash
 * @param buffer  The starting address of the data buffer to write
 * @param address The Flash address to begin writing, from 0 to 0x17FFFF
 * @param length  The number of bytes to write
 * @return true if successful, false otherwise
 */
bool Flash::writeBytes(uint8_t *buffer, uint32_t address, uint32_t length) {
    // For odd addresses, read back one byte and extend the length
    uint8_t addressOffset = address&1;
    if (addressOffset) {
        address--;
        length++;
    }

    // Extend the length if it's odd
    uint8_t lengthAdjustment = length&1;
    if (lengthAdjustment) {
        length++;
    }

    // Check for errors after adjustment
    if (address + length > maxAddress) {
        return false;
    }

    // Use a temporary buffer to write the data
    uint8_t *writeBuffer = new uint8_t[length];

    if (addressOffset) {
        // Address preceeds the included data
        // Read the existing byte so we can write it back
        readBytes(writeBuffer, address, 2);
    }

    if (lengthAdjustment) {
        // Length overruns the included data
        // Read the existing byte so we can write it back
        readBytes(writeBuffer + length - 2, address + length - 2, 2);
    }

  // Copy to write buffer, compensating for shifted address and adjusted length
  memcpy(writeBuffer + addressOffset, buffer, length - addressOffset - lengthAdjustment);

  // Write the data
  sFLASH_WriteBuffer(writeBuffer, baseAddress + address, length);

  return true;
}

