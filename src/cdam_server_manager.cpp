#include "cdam_manager.h"
#include "cdam_constants.h"
#include "spark_wiring_network.h"
#include "spark_wiring_tcpclient.h"

namespace cdam
{

const int8_t kServerReturnNoCmd = -1;
const int8_t kServerReturnFail = 0;
const int8_t kServerReturnEventIncoming = 1;

const char* kServerVarLastCmd = "last_command";

const char* kServerCmd = "command";
const char* kServerCmdPing = "ping";
const char* kServerCmdKeypadInput = "keypad_input";
const char* kServerCmdButtonInput = "button_input";
const char* kServerCmdAddCredits = "add_credits";
const char* kServerCmdRemoveCredits = "remove_credits";
const char* kServerCmdRemoveStory = "remove_story";
const char* kServerCmdRemoveAllStories = "remove_all_stories";
const char* kServerCmdAddStory = "add_story";

// Commands that cause the Choosatron to throw events.
const char* kServerCmdGetState = "get_state";
const char* kServerCmdGetMillis = "get_millis";
const char* kServerCmdGetSeconds = "get_seconds";
const char* kServerCmdGetFreeSpace = "get_free_space";
const char* kServerCmdGetUsedSpace = "get_used_space";
const char* kServerCmdGetCredits = "get_credits";
const char* kServerCmdGetSSID = "get_ssid";
const char* kServerCmdGetGatewayIP = "get_gateway_ip";
const char* kServerCmdGetMacAddr = "get_mac_addr";
const char* kServerCmdGetSubnetMask = "get_subnet_mask";
const char* kServerCmdGetLocalIP = "get_local_ip";
const char* kServerCmdGetRSSI = "get_rssi";

const int kDefaultTCPPort = 80;

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
	aCommandAndArgs.trim();
    aCommandAndArgs.toLowerCase();
    aCommandAndArgs.toCharArray(Manager::getInstance().serverManager->lastCommand, 64);
    DEBUG("Command Received: %s", Manager::getInstance().serverManager->lastCommand);

    int cmdLen = aCommandAndArgs.length() + 1;
    int commaPosition = aCommandAndArgs.indexOf("|");
	if (commaPosition > -1) {
		DEBUG("Has Arguments");
		cmdLen = commaPosition + 1;
    	//command = aCommandAndArgs.substring(0, commaPosition);
    	//args.substring(commaPosition+1, args.length()));//send remaining part to line 2
	} else {
		DEBUG("No Arguments");
		//command = aCommandAndArgs.c_str();
	}
	char command[cmdLen];
	aCommandAndArgs.toCharArray(command, cmdLen);

	DEBUG("Cmd: %s", command);
	if (strcmp(command, kServerCmdPing) == 0) {

	} else if (strcmp(command, kServerCmdKeypadInput) == 0) {
		String keypadStr = aCommandAndArgs.substring(commaPosition + 1, aCommandAndArgs.length());
		uint8_t keypadVal = atoi(keypadStr.c_str());
		DEBUG("Keypad Val: %d", keypadVal);
		return keypadVal;
	} else if (strcmp(command, kServerCmdButtonInput) == 0) {

	} else if (strcmp(command, kServerCmdAddCredits) == 0) {
		Manager::getInstance().dataManager->gameCredits += 1;
		return Manager::getInstance().dataManager->gameCredits;
	} else if (strcmp(command, kServerCmdRemoveCredits) == 0) {

	} else if (strcmp(command, kServerCmdRemoveStory) == 0) {

	} else if (strcmp(command, kServerCmdRemoveAllStories) == 0) {

	} else if (strcmp(command, kServerCmdAddStory) == 0) {

		// Get server IP address/port (e.g. 1.1.1.1:80) from arguments
		String serverAddressAndPort = aCommandAndArgs.substring(commaPosition + 1, aCommandAndArgs.length());
		downloadStoryData(serverAddressAndPort);

	} else if (strcmp(command, kServerCmdGetState) == 0) {
		Spark.publish(kServerCmdGetState, Manager::getInstance().dataManager->gameStateStr(), kServerTTLDefault, PRIVATE);
		return kServerReturnEventIncoming;
	} else if (strcmp(command, kServerCmdGetMillis) == 0) {
		return millis();
	} else if (strcmp(command, kServerCmdGetSeconds) == 0) {
		return millis() / 1000;
	} else if (strcmp(command, kServerCmdGetFreeSpace) == 0) {

	} else if (strcmp(command, kServerCmdGetUsedSpace) == 0) {

	} else if (strcmp(command, kServerCmdGetCredits) == 0) {
		return Manager::getInstance().dataManager->gameCredits;
	} else if (strcmp(command, kServerCmdGetSSID) == 0) {
		Spark.publish(kServerCmdGetSSID, Network.SSID(), kServerTTLDefault, PRIVATE);
		return kServerReturnEventIncoming;
	} else if (strcmp(command, kServerCmdGetGatewayIP) == 0) {
		uint8_t *address = Network.gatewayIP().raw_address();
		char addr[16] = "";
		snprintf(addr, 16, "%d.%d.%d.%d", address[0], address[1], address[2], address[3]);
		Spark.publish(kServerCmdGetGatewayIP, addr, kServerTTLDefault, PRIVATE);
		return kServerReturnEventIncoming;
	} else if (strcmp(command, kServerCmdGetMacAddr) == 0) {
		byte macVal[6];
		Network.macAddress(macVal);
		char macAddr[18];
		snprintf(macAddr, 19, "%02x:%02x:%02x:%02x:%02x:%02x", macVal[5], macVal[4], macVal[3], macVal[2], macVal[1], macVal[0]);
		Spark.publish(kServerCmdGetMacAddr, macAddr, kServerTTLDefault, PRIVATE);
		return kServerReturnEventIncoming;
	} else if (strcmp(command, kServerCmdGetSubnetMask) == 0) {
		uint8_t *address = Network.subnetMask().raw_address();
		char addr[16] = "";
		snprintf(addr, 16, "%d.%d.%d.%d", address[0], address[1], address[2], address[3]);
		Spark.publish(kServerCmdGetSubnetMask, addr, kServerTTLDefault, PRIVATE);
		return kServerReturnEventIncoming;
	} else if (strcmp(command, kServerCmdGetLocalIP) == 0) {
		uint8_t *address = Network.localIP().raw_address();
		char addr[16] = "";
		snprintf(addr, 16, "%d.%d.%d.%d", address[0], address[1], address[2], address[3]);
		Spark.publish(kServerCmdGetLocalIP, addr, kServerTTLDefault, PRIVATE);
		return kServerReturnEventIncoming;
	} else if (strcmp(command, kServerCmdGetRSSI) == 0) {
		return Network.RSSI();
	}

	return kServerReturnNoCmd;
}

bool ServerManager::downloadStoryData(String aServerAddressAndPort) {
	// Configure port
	uint port = kDefaultTCPPort;
	int colonIndex = aServerAddressAndPort.indexOf(":");
	if (colonIndex > -1) {
		String portStr = aServerAddressAndPort.substring(colonIndex+1, aServerAddressAndPort.length());
		port = atoi(portStr.c_str());
	} else {
		// No port supplied, set index to end of string
		colonIndex = aServerAddressAndPort.length();
	}

	// Split IP into octets
	int dotIndex = aServerAddressAndPort.indexOf(".");
	if (dotIndex == -1) {
		// Not a valid IP address
		Errors::setError(E_SERVER_INVALID_IP);
		ERROR(Errors::errorString());
		return false;
	}
	String octectStr = aServerAddressAndPort.substring(0, dotIndex);
	uint8_t firstOctet = atoi(octectStr.c_str());

	int nextDotIndex = aServerAddressAndPort.indexOf(".", dotIndex + 1);
	if (nextDotIndex == -1) {
		// Not a valid IP address
		Errors::setError(E_SERVER_INVALID_IP);
		ERROR(Errors::errorString());
		return false;
	}
	octectStr = aServerAddressAndPort.substring(dotIndex + 1, nextDotIndex);
	uint8_t secondOctet = atoi(octectStr.c_str());

	dotIndex = nextDotIndex;
	nextDotIndex = aServerAddressAndPort.indexOf(".", dotIndex + 1);
	if (nextDotIndex == -1) {
		// Not a valid IP address
		Errors::setError(E_SERVER_INVALID_IP);
		ERROR(Errors::errorString());
		return false;
	}
	octectStr = aServerAddressAndPort.substring(dotIndex + 1, nextDotIndex);
	uint8_t thirdOctet = atoi(octectStr.c_str());

	dotIndex = nextDotIndex;
	octectStr = aServerAddressAndPort.substring(dotIndex + 1, colonIndex);
	uint8_t fourthOctet = atoi(octectStr.c_str());

	DEBUG("Download story data from %u.%u.%u.%u:%u", firstOctet, secondOctet, thirdOctet, fourthOctet, port);

	byte server[] = {firstOctet, secondOctet, thirdOctet, fourthOctet};
	TCPClient client;
	if (client.connect(server, port)) {
    	DEBUG("TCPClient connected");
    	// client.println("GET /search?q=unicorn HTTP/1.0");
    	// client.println("Host: www.google.com");
    	// client.println("Content-Length: 0");
    	// client.println();
  	}
  	else
  	{
    	Serial.println("TCPClient connection failed");
 	}

}

}