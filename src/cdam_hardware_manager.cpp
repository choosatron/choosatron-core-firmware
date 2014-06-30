#include "cdam_hardware_manager.h"
#include "cdam_constants.h"
#include "cdam_keypad.h"

namespace cdam
{

/* Public Methods */

HardwareManager::HardwareManager() {
}

bool HardwareManager::initialize() {
	setupHardwarePins();
	initHardware();
	initLoopControl();
	return true;
}

// Calls each setup interval timer.
void HardwareManager::updateIntervalTimers() {
	keypadIntervalUpdate();
}

/* Accessors */

Keypad* HardwareManager::keypad() {
	return _keypad;
}

/* Private Methods */

void HardwareManager::setupHardwarePins() {
	pinMode(PIN_PIEZO, OUTPUT);
	pinMode(PIN_RED_LED, OUTPUT);
	pinMode(PIN_GREEN_LED, OUTPUT);
	pinMode(PIN_BLUE_LED, OUTPUT);
}

void HardwareManager::initHardware() {
	_keypad = new Keypad();
	_keypad->initialize();
	_keypad->active = false;
}

void HardwareManager::initLoopControl() {
	//_keypadElapsed = new ElapsedMillis(kKeypadIntervalMillis);
}

void HardwareManager::keypadIntervalUpdate() {
	if (_keypadElapsed > kKeypadIntervalMillis) {
		Serial.println(millis());
		_keypad->updateKeypad();
		_keypadElapsed = 0;
	}
}