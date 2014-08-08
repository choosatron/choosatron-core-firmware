#include "cdam_main.h"
#include "cdam_constants.h"
#include "cdam_manager.h"
#include "cdam_state_controller.h"

#if (OFFLINE == 1)
#include "spark_disable_wlan.h"
#include "spark_disable_cloud.h"
#endif

namespace cdam
{

Choosatron::Choosatron() {
}

bool Choosatron::setup() {
	_stateController = new StateController();
	_stateController->initialize();

	/*DataManager::getInstance().initialize();
	ServerManager::getInstance().initialize();

	HardwareManager::getInstance().initialize();
	HardwareManager::getInstance().keypad()->active = true;*/

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
	Manager::getInstance().hardwareManager->updateIntervalTimers();
	Manager::getInstance().serverManager->handlePendingActions();

	if (Manager::getInstance().dataManager->runState) {
		_stateController->updateState();
	}

	Manager::getInstance().hardwareManager->keypad()->clearEvents();

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

}