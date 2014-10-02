#include "cdam_hardware_manager.h"
#include "cdam_constants.h"
#include "cdam_manager.h"

namespace cdam
{

/* Public Methods */

HardwareManager::HardwareManager() {
}

bool HardwareManager::initialize(StateController *aStateController) {
	_stateControl = aStateController;
	_coinAcceptor = NULL;
	setupHardwarePins();
	initHardware();
	return true;
}

void HardwareManager::printCoinInsertIntervalUpdate() {
	if (_printInsertElapsed > kIntervalCoinInsertMessage) {
		_printer->printInsertCoin(_coinAcceptor->coins,
		                          _coinAcceptor->coinsPerCredit);
		_printInsertElapsed = 0;
	}
}

// Calls each setup interval timer.
void HardwareManager::updateIntervalTimers() {
	printerIntervalUpdate();
	keypadIntervalUpdate();
	coinAcceptorIntervalUpdate();
}

/* Accessors */

Printer* HardwareManager::printer() {
	return _printer;
}

Keypad* HardwareManager::keypad() {
	return _keypad;
}

CoinAcceptor* HardwareManager::coinAcceptor() {
	return _coinAcceptor;
}

/* Private Methods */

void HardwareManager::setupHardwarePins() {
	// Pin for Piezo Speaker
	pinMode(PIN_PIEZO, OUTPUT);
	// Pins for RGB LED
	pinMode(PIN_RED_LED, OUTPUT);
	pinMode(PIN_GREEN_LED, OUTPUT);
	pinMode(PIN_BLUE_LED, OUTPUT);
	digitalWrite(PIN_RED_LED, HIGH);
	digitalWrite(PIN_GREEN_LED, HIGH);
	digitalWrite(PIN_BLUE_LED, HIGH);
	// Pin for Coin Acceptor
	pinMode(PIN_COIN, INPUT);
}

void HardwareManager::initHardware() {
	// Setup Printer
	_printer = new Printer();
	_printer->initialize();
	_printer->begin(160);
	_printer->setTimes(7000, 300);
	_printer->setASB(true);
	_printer->active = false;

	// Setup Keypad
	_keypad = new Keypad();
	_keypad->initialize();
	_keypad->active = false;

	// Setup Coin Acceptor
	_coinAcceptor = new CoinAcceptor();
	_coinAcceptor->initialize();
	_coinAcceptor->active = false;
}

void HardwareManager::printerIntervalUpdate() {
	if (_printerElapsed > kIntervalPrinterMillis) {
		_printer->updateState();
		_printer->logChangedStatus();
		// Allow printing to continue across loops.
		/*if (_printer->printing) {
			_printer->finishPrinting();
		}*/
		_printerElapsed = 0;
	}
}

void HardwareManager::keypadIntervalUpdate() {
	if (_keypadElapsed > kIntervalKeypadMillis) {
		_keypad->updateState();
		_keypadElapsed = 0;
	}
}

void HardwareManager::coinAcceptorIntervalUpdate() {
	if (Manager::getInstance().dataManager->metadata.flags.arcade &&
	    	_coinElapsed > kIntervalCoinAcceptorMillis) {
		_coinAcceptor->updateState();
		_coinElapsed = 0;
	}
}

}