/**
 ******************************************************************************
 * @file    cdam_flash.h
 * @author  Dan Kane
 * @version V1.0.0
 * @date    30-March-2014
 * @brief   Header for cdam_flash.cpp module
 ******************************************************************************
  Copyright (c) 2014 Monkey with a Mustache, LLC.  All rights reserved.
 ******************************************************************************
 */

#ifndef CDAM_FLASH_H
#define CDAM_FLASH_H

#include <stdint.h>
extern "C" {
#include "sst25vf_spi.h"
}

namespace cdam
{

class Flash
{
	public:
	  	static void init(void);
	  	static uint8_t readByte(uint32_t address);
	  	static bool writeByte(uint32_t address, uint8_t value);
	  	static bool read(void* data, uint32_t address, uint32_t length);
	 	static bool write(const void* data, uint32_t address, uint32_t length);
};

}

#endif