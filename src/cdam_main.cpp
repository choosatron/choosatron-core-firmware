#include "cdam_main.h"
#include "cdam_constants.h"
#include "cdam_data_manager.h"
#include "Cdam_server_manager.h"
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


//const uint16_t KEYPAD_UPDATE_MILLIS = 500;
//uint16_t _lastMillis;
//uint16_t _delta;

Choosatron::Choosatron() {
}

bool Choosatron::setup() {
	DataManager::getInstance().initialize();
	ServerManager::getInstance().initialize();

	HardwareManager::getInstance().initialize();
	HardwareManager::getInstance().keypad()->active = true;

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