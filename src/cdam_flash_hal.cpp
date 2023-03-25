/**
 ******************************************************************************
* @file    ota_flash_hal.c
* @author  Matthew McGowan
* @version V1.0.8
* @date    25-Sept-2014
* @brief
******************************************************************************
Copyright (c) 2013-14 Spark Labs, Inc.  All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation, either
version 3 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, see <http://www.gnu.org/licenses/>.
******************************************************************************
*/

#include "cdam_flash_hal.h"
#include "cdam_constants.h"
#include "cdam_manager.h"
#include "flashee-eeprom.h"
#include "hw_config.h"
#include <string.h>

/*uint32_t CDAM_OTA_FlashAddress()
{
   return EXTERNAL_FLASH_OTA_ADDRESS;
}

#define FLASH_MAX_SIZE          (int32_t)(INTERNAL_FLASH_END_ADDRESS - CORE_FW_ADDRESS)

uint32_t CDAM_OTA_FlashLength()
{
   return FLASH_MAX_SIZE;
}*/


bool CDAM_Write_To_Flashee()
{
   return cdam::Manager::getInstance().dataManager->writeToFlashee;
}

bool CDAM_FLASH_CheckValidAddressRange(uint32_t sFLASH_Address, uint32_t fileSize)
{
   uint32_t endAddress = sFLASH_Address + fileSize - 1;
   uint8_t flashTarget = cdam::Manager::getInstance().dataManager->flashTarget;

   if (flashTarget == cdam::kFlashMetadataType) {
      uint32_t totalBytes = cdam::kFlashMetadataPageCount * Flashee::Devices::userFlash().pageSize();
      if (endAddress < totalBytes) {
         return true;
      }
   } else if (flashTarget == cdam::kFlashStoriesType) {
      uint32_t totalBytes = cdam::kFlashStoriesPageCount * Flashee::Devices::userFlash().pageSize();
      if (endAddress < totalBytes) {
         return true;
      }
   } else if (flashTarget == cdam::kFlashSavesType) {
      // TODO: For save states, not implemented yet.
      //uint32_t totalBytes = cdam::kFlashSavesPageCount * Flashee::Devices::userFlash().pageSize();
      //if (endAddress < totalBytes) {
      //	return true;
      //}
   }
   return false;
}

void CDAM_FLASH_Begin(uint32_t sFLASH_Address, uint32_t fileSize)
{
   //bool flashee = cdam::Manager::getInstance().dataManager->writeToFlashee;
   //if (flashee) {
      cdam::Manager::getInstance().dataManager->writeBegin(sFLASH_Address, fileSize);
   //} else {
   //	FLASH_Begin(sFLASH_Address, fileSize);
   //}
}

uint16_t CDAM_FLASH_Update(uint8_t *pBuffer, uint32_t bufferSize)
{
   //bool flashee = cdam::Manager::getInstance().dataManager->writeToFlashee;
   //if (flashee) {
   uint16_t status = cdam::Manager::getInstance().dataManager->writeData(pBuffer, bufferSize);
   //} else {
   //	status = FLASH_Update(pBuffer, bufferSize);
   //}
   return status;
}

void CDAM_FLASH_End(bool aResult)
{
   //bool flashee = cdam::Manager::getInstance().dataManager->writeToFlashee;
   //if (flashee) {
      cdam::Manager::getInstance().dataManager->writeEnd(aResult);
   //} else {
   //	FLASH_End();
   //}
}


/*void parseServerAddressData(ServerAddress* server_addr, uint8_t* buf)
{
// Internet address stored on external flash may be
// either a domain name or an IP address.
// It's stored in a type-length-value encoding.
// First byte is type, second byte is length, the rest is value.

switch (buf[0])
{
   case IP_ADDRESS:
   server_addr->addr_type = IP_ADDRESS;
   server_addr->ip = (buf[2] << 24) | (buf[3] << 16) |
                  (buf[4] << 8)  |  buf[5];
   break;

   case DOMAIN_NAME:
   if (buf[1] <= EXTERNAL_FLASH_SERVER_DOMAIN_LENGTH - 2)
   {
      server_addr->addr_type = DOMAIN_NAME;
      memcpy(server_addr->domain, buf + 2, buf[1]);

      // null terminate string
      char *p = server_addr->domain + buf[1];
      *p = 0;
      break;
   }
   // else fall through to default

   default:
   server_addr->addr_type = INVALID_INTERNET_ADDRESS;
}

}

void HAL_FLASH_Read_ServerAddress(ServerAddress* server_addr)
{
   uint8_t buf[EXTERNAL_FLASH_SERVER_DOMAIN_LENGTH];
   FLASH_Read_ServerAddress_Data(buf);
   parseServerAddressData(server_addr, buf);
}


bool HAL_OTA_Flashed_GetStatus(void)
{
   return OTA_Flashed_GetStatus();
}

void HAL_OTA_Flashed_ResetStatus(void)
{
   OTA_Flashed_ResetStatus();
}

void HAL_FLASH_Read_ServerPublicKey(uint8_t *keyBuffer)
{
   FLASH_Read_ServerPublicKey(keyBuffer);
}

void HAL_FLASH_Read_CorePrivateKey(uint8_t *keyBuffer)
{
   FLASH_Read_CorePrivateKey(keyBuffer);
}*/