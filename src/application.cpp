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
#include "cdam_constants.h"
#include "cdam_manager.h"
#include "cdam_state_controller.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

/* Function prototypes -------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
cdam::StateController *stateController;

/* This function is called once at start up ----------------------------------*/
void setup()
{
#ifndef DEBUG_BUILD
   Serial.begin(BAUD_RATE);
#endif
   //while(!Serial.available()) SPARK_WLAN_Loop();
   stateController = new cdam::StateController();
   stateController->initialize();
}

/* This function loops forever --------------------------------------------*/
void loop()
{
   cdam::Manager::getInstance().dataManager->handleSerialData();
   cdam::Manager::getInstance().hardwareManager->updateIntervalTimers();
   //cdam::Manager::getInstance().serverManager->handlePendingActions();
   //Spark.process();
   if (cdam::Manager::getInstance().dataManager->runState) {
      stateController->updateState();
   }
}

#ifdef DEBUG_BUILD
void debug_output_(const char *p)
{
    static boolean once = false;
    if (!once) {
        once = true;
        Serial.begin(BAUD_RATE);
    }
    Serial.print(p);
}
#endif