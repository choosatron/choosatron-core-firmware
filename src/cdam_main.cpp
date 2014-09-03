#include "cdam_main.h"
#include "cdam_constants.h"
#include "cdam_manager.h"
#include "cdam_state_controller.h"

namespace cdam
{

Choosatron::Choosatron() {}

bool Choosatron::setup() {
	_stateController = new StateController();
	_stateController->initialize();

	return true;
}

int Choosatron::loop() {
	Manager::getInstance().hardwareManager->updateIntervalTimers();
	Manager::getInstance().serverManager->handlePendingActions();

	if (Manager::getInstance().dataManager->runState) {
		_stateController->updateState();
	}

	//Manager::getInstance().hardwareManager->keypad()->clearEvents();
	return 0;
}

}