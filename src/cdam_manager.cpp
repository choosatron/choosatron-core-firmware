#include "cdam_manager.h"
#include "cdam_state_controller.h"

namespace cdam
{

/* Public Methods */

void Manager::initialize(StateController* aStateController) {
	dataManager->initialize(aStateController);
	serverManager->initialize(aStateController);
	hardwareManager->initialize(aStateController);
}


/* Private Methods */
Manager::Manager() {
	dataManager = new DataManager();
	serverManager = new ServerManager();
	hardwareManager = new HardwareManager();
}

}