#include "cdam_manager.h"
#include "cdam_constants.h"
#include "spark_wiring_network.h"
#include "spark_wiring_tcpclient.h"

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
	String command = "";
	aCommandAndArgs.trim();
    aCommandAndArgs.toLowerCase();
    aCommandAndArgs.toCharArray(Manager::getInstance().serverManager->lastCommand, 64);
    DEBUG("Command Received: %s", Manager::getInstance().serverManager->lastCommand);

    int commaPosition = aCommandAndArgs.indexOf("|");
	if (commaPosition > -1) {
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
		String keypadStr = aCommandAndArgs.substring(commaPosition + 1, aCommandAndArgs.length());
		uint8_t keypadVal = atoi(keypadStr.c_str());
		DEBUG("Keypad Val: %d", keypadVal);
		return keypadVal;
	} else if (command == kServerCmdButtonInput) {

	} else if (command == kServerCmdAddCredits) {
		Manager::getInstance().dataManager->gameCredits += 1;
		return Manager::getInstance().dataManager->gameCredits;
		return 1;
	} else if (command == kServerCmdRemoveCredits) {

	} else if (command == kServerCmdRemoveStory) {

	} else if (command == kServerCmdRemoveAllStories) {

	} else if (command == kServerCmdAddStory) {

		// Get server IP address/port (e.g. 1.1.1.1:80) from arguments
		String serverAddressAndPort = aCommandAndArgs.substring(commaPosition + 1, aCommandAndArgs.length());
		downloadStoryData(serverAddressAndPort);

	} else if (command == kServerCmdGetState) {

	} else if (command == kServerCmdGetMillis) {
		return millis();
	} else if (command == kServerCmdGetSeconds) {
		return millis() / 1000;
		return 1;
		//Spark.publish(kServerCmdGetSeconds, "thousands", kServerTTLDefault, PRIVATE);
	} else if (command == kServerCmdGetFreeSpace) {

	} else if (command == kServerCmdGetUsedSpace) {

	} else if (command == kServerCmdGetCredits) {
		return Manager::getInstance().dataManager->gameCredits;
	} else if (command == kServerCmdGetSSID) {
		//Spark.publish(kServerCmdGetSSID, Network.SSID(), kServerTTLDefault, PRIVATE);
		return 1;
	} else if (command == kServerCmdGetGatewayIP) {
		//Spark.publish(kServerCmdGetGatewayIP, Network.gatewayIP(), kServerTTLDefault, PRIVATE);
		return 1;
	} else if (command == kServerCmdGetMacAddr) {

	} else if (command == kServerCmdGetSubnetMask) {
		//Spark.publish(kServerCmdGetSSID, Network.SSID(), kServerTTLDefault, PRIVATE);
		return 1;
	} else if (command == kServerCmdGetLocalIP) {
		//Spark.publish(kServerCmdGetSSID, Network.SSID(), kServerTTLDefault, PRIVATE);
		return 1;
	} else if (command == kServerCmdGetRSSI) {
		return Network.RSSI();
	}

	return -1;
}

void ServerManager::downloadStoryData(String aServerAddressAndPort) {
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
	String octectStr = aServerAddressAndPort.substring(0, dotIndex);
	uint8_t firstOctet = atoi(octectStr.c_str());

	int nextDotIndex = aServerAddressAndPort.indexOf(".", dotIndex + 1);
	octectStr = aServerAddressAndPort.substring(dotIndex + 1, nextDotIndex);
	uint8_t secondOctet = atoi(octectStr.c_str());

	dotIndex = nextDotIndex;
	nextDotIndex = aServerAddressAndPort.indexOf(".", dotIndex + 1);
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