#include "cdam_flash.h"
#include <cstring>

namespace cdam
{

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
    uint8_t value = 0xFF;
    if (read(&value, address, 1)) {
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
    return write(&value, address, 1);
}

/**
 * Read bytes from the Flash
 * @param data  The address of the data which will receive the data
 * @param address The Flash address to begin reading, from 0 to 0x17FFFF
 * @param length  The number of bytes to read
 * @return true if successful, false otherwise
 */
bool Flash::read(void* data, uint32_t address, uint32_t length) {
    // Check for errors
    if (address + length > maxAddress) {
        return false;
    }

    // Read the data
    sFLASH_ReadBuffer((uint8_t*)data, baseAddress + address, length);

    return true;
}

/**
 * Write bytes to the Flash
 * Note: sFlash_WriteBuffer NOW SUPPORTS ODD ADDRESSES AND LENGTHS
 * @param data  The starting address of the data data to write
 * @param address The Flash address to begin writing, from 0 to 0x17FFFF
 * @param length  The number of bytes to write
 * @return true if successful, false otherwise
 */
bool Flash::write(const void* data, uint32_t address, uint32_t length) {
    // Check for errors after adjustment
    if (address + length > maxAddress) {
        return false;
    }

    // Write the data
    sFLASH_WriteBuffer(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(data)),
                       baseAddress + address, length);

    return true;
}

}