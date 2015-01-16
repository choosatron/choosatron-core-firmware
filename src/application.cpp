/**
 ******************************************************************************
 * @file    application.cpp
 * @authors  Satish Nair, Zachary Crockett and Mohit Bhoite
 * @version V1.0.0
 * @date    05-November-2013
 * @brief   Tinker application
 ******************************************************************************
  Copyright (c) 2013 Spark Labs, Inc.  All rights reserved.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "application.h"
#include "Ymodem/Ymodem.h"
#include "cdam_main.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

/* Function prototypes -------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
cdam::Choosatron *choosatron;


/* This allows triggering DFU mode over WiFi ---------------------------------*/
#ifdef DEBUG_BUILD
/*int doDFU(String command) {
	FLASH_OTA_Update_SysFlag = 0x0000;
	Save_SystemFlags();
	BKP_WriteBackupRegister(BKP_DR10, 0x0000);
	USB_Cable_Config(DISABLE);
	NVIC_SystemReset();

	return 0;
}*/
#endif

/* This function is called once at start up ----------------------------------*/
void setup()
{
#ifdef DEBUG_BUILD
	//Spark.function("dfu", doDFU);
#endif

	choosatron = new cdam::Choosatron();
	choosatron->setup();
}

/* This function loops forever --------------------------------------------*/
void loop()
{
	choosatron->loop();
}
