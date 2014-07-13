#include "cdam_manager.h"
#include "cdam_constants.h"

namespace cdam
{

const char* kServerVarLastCmd = "last_command";

const char* kServerCmd = "command";
const String kServerCmdPing = "ping";
const String kServerCmdKeypadInput = "keypad_input";
const String kServerCmdButtonInput = "button_input";
const String kServerCmdAddCredits = "add_credits";
const String kServerCmdRemoveCredits = "remove_credits";
const String kServerCmdRemoveStory = "remove_story";
const String kServerCmdRemoveAllStories = "remove_all_stories";
const String kServerCmdAddStory = "add_story";

// Commands that cause the Choosatron to throw events.
const String kServerCmdGetState = "get_state";
const String kServerCmdGetMillis = "get_millis";
const String kServerCmdGetSeconds = "get_seconds";
const String kServerCmdGetFreeSpace = "get_free_space";
const String kServerCmdGetUsedSpace = "get_used_space";
const String kServerCmdGetCredits = "get_credits";
const String kServerCmdGetSSID = "get_ssid";
const String kServerCmdGetGatewayIP = "get_gateway_ip";
const String kServerCmdGetMacAddr = "get_mac_addr";
const String kServerCmdGetSubnetMask = "get_subnet_mask";
const String kServerCmdGetLocalIP = "get_local_ip";
const String kServerCmdGetRSSI = "get_rssi";

/* Public Methods */

ServerManager::ServerManager() {
}

void ServerManager::initialize() {
	// Setup our command function hook for the server.
	Spark.function(kServerCmd, serverCommand);
	// Expose the last command issued.
	Spark.variable(kServerVarLastCmd , this->lastCommand, STRING);

	//ServerManager::getInstance().lastCommand = "NONE";
	//strcpy("NONE", ServerManager::lastCommand);
	//DEBUG(ServerManager::lastCommand);
}

int ServerManager::serverCommand(String aCommandAndArgs) {
	String command = "";
	aCommandAndArgs.trim();
    aCommandAndArgs.toLowerCase();
    aCommandAndArgs.toCharArray(Manager::getInstance().serverManager->lastCommand, 64);
    DEBUG("Command Received: %s", Manager::getInstance().serverManager->lastCommand);

    int commaPosition = aCommandAndArgs.indexOf(",");
	if (commaPosition >- 1) {
		DEBUG("Has Arguments");
    	command = aCommandAndArgs.substring(0, commaPosition);
    	//args.substring(commaPosition+1, args.length()));//send remaining part to line 2
	} else {
		DEBUG("No Arguments");
		command = aCommandAndArgs;
	}

	//DEBUG("Cmd: %s", command);
	if (command == kServerCmdPing) {

	} else if (command == kServerCmdKeypadInput) {

	} else if (command == kServerCmdButtonInput) {

	} else if (command == kServerCmdAddCredits) {
		Manager::getInstance().dataManager->gameCredits += 1;
		return Manager::getInstance().dataManager->gameCredits;
	} else if (command == kServerCmdRemoveCredits) {

	} else if (command == kServerCmdRemoveStory) {

	} else if (command == kServerCmdRemoveAllStories) {

	} else if (command == kServerCmdAddStory) {

	} else if (command == kServerCmdGetState) {

	} else if (command == kServerCmdGetMillis) {
		return millis();
	} else if (command == kServerCmdGetSeconds) {
		return millis() / 1000;
		//Spark.publish(kServerCmdGetSeconds, "thousands", 60, PRIVATE);
	} else if (command == kServerCmdGetFreeSpace) {

	} else if (command == kServerCmdGetUsedSpace) {

	} else if (command == kServerCmdGetCredits) {
		return Manager::getInstance().dataManager->gameCredits;
	} else if (command == kServerCmdGetSSID) {

	} else if (command == kServerCmdGetGatewayIP) {

	} else if (command == kServerCmdGetMacAddr) {

	} else if (command == kServerCmdGetSubnetMask) {

	} else if (command == kServerCmdGetLocalIP) {

	} else if (command == kServerCmdGetRSSI) {

	}

	return -1;
}

}