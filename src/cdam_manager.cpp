#include "cdam_manager.h"

namespace cdam
{

/* Public Methods */

void Manager::initialize() {
	dataManager->initialize();
	serverManager->initialize();
	hardwareManager->initialize();
}


/* Private Methods */
Manager::Manager() {
	dataManager = new DataManager();
	serverManager = new ServerManager();
	hardwareManager = new HardwareManager();
}

}