#include "cdam_main.h"
#include "cdam_constants.h"
#include "cdam_data_manager.h"
#include "cdam_hardware_manager.h"
#include "cdam_keypad.h"

#if (OFFLINE == 1)
#include "spark_disable_wlan.h"
#include "spark_disable_cloud.h"
#endif

namespace cdam
{


// Extern C seems to mess up declaring this in the header.
Keypad *_keypad;

//IntervalTimer _timer;


const uint16_t KEYPAD_UPDATE_MILLIS = 500;
uint16_t _lastMillis;
uint16_t _delta;

Choosatron::Choosatron() {
}

bool Choosatron::setup() {
	DataManager::getInstance().initialize();

	HardwareManager::getInstance().initialize();
	HardwareManager::getInstance().keypad()->active = true;
	LOG("HI");
	DEBUG("MEOW");

	// Setup the timer interrupts for keypad updates.
	//_timer.begin(updateKeypad, 10, hmSec);
	//enableTimerInterrupt();

	//_keypad = new Keypad();
	//_keypad->initialize();
	//_keypad->active = false;

	//_lastMillis = 0;
	//_delta = 0;

	return true;
}

int Choosatron::loop() {
	HardwareManager::getInstance().updateIntervalTimers();
	//uint16_t newMillis = millis();
	//uint16_t diff = newMillis - _lastMillis;
	LOG("YUM");
	DEBUG("Test TCP BAD Every %d Usage!",1);
    LOG("The following 4 mmessages are a test of the logger....");
    LOG("Want %d more cores",2);
    WARN("Running %s on cores only %d more left","Low",3);
    DEBUG("connection closed %d",4);
    ERROR("Flash write Failed @0x%0x",5);
    LOG("Logger test Done");

	/*Serial.print(newMillis);
	Serial.print(" - ");
	Serial.print(_lastMillis);
	Serial.print(" = ");
	Serial.println(delta);*/
	/*if ((KEYPAD_UPDATE_MILLIS - _delta) <= diff) {
		_delta = diff % (KEYPAD_UPDATE_MILLIS - _delta);
		_lastMillis = newMillis;
		Serial.print("Time: ");
		Serial.print(newMillis);
		Serial.print(", Delta: ");
		Serial.println(_delta);
		//_delta = (millis - _lastMillis) - (KEYPAD_UPDATE_MILLIS - _delta);
	}*/

	return 0;
}


void Choosatron::updateKeypad() {
  _keypad->updateKeypad();
}


}