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
	//Spark.process();
	Manager::getInstance().hardwareManager->updateIntervalTimers();
	//Spark.process();
	Manager::getInstance().serverManager->handlePendingActions();
	Spark.process();
	if (Manager::getInstance().dataManager->runState) {
		_stateController->updateState();
	}

	return 0;
}

}