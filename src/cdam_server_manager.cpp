#include "cdam_manager.h"
#include "cdam_constants.h"
#include "spark_wiring_network.h"

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

const uint8_t kServerTimeout = 30000;	// ms
const uint16_t kServerDefaultTCPPort = 80;
const char kServerArgumentDelimiter = '|';

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
    int commaPosition = aCommandAndArgs.indexOf(kServerArgumentDelimiter);
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

		// Parser arguments
		// args: IP addr uint8_t(4 bytes), port uint16_t(2 bytes), story position uint8_t(1 byte), story size uint32_t(4 bytes), checksum uint8_t(16 bytes)
		// Validate
		// Check Storage availability (ensure there is room)
		// Set State to waiting for data
		// Create server connection
		// While connected, wait for data availability (with timeout)
		// While data available, read data into buffer, then flash space based on page size
		// Run Checksum
		// Send back status (success, failure...)
		byte ip[] = { aCommandAndArgs.charAt(commaPosition + 1), aCommandAndArgs.charAt(commaPosition + 2),
			aCommandAndArgs.charAt(commaPosition + 3), aCommandAndArgs.charAt(commaPosition + 4)};

		uint16_t port = aCommandAndArgs.charAt(commaPosition + 5) | (aCommandAndArgs.charAt(commaPosition + 6) << 8);
		DEBUG("Port: %d", port);
		uint8_t storyPos = aCommandAndArgs.charAt(commaPosition + 7);
		DEBUG("Story Pos: %d", storyPos);
		uint32_t storySize = aCommandAndArgs.charAt(commaPosition + 8) | (aCommandAndArgs.charAt(commaPosition + 9) << 8) | (aCommandAndArgs.charAt(commaPosition + 10) << 16) | (aCommandAndArgs.charAt(commaPosition + 11) << 24);
		DEBUG("Story Size: %Lu", storySize);
		DEBUG("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", aCommandAndArgs.charAt(commaPosition + 1), aCommandAndArgs.charAt(commaPosition + 2), aCommandAndArgs.charAt(commaPosition + 3), aCommandAndArgs.charAt(commaPosition + 4), aCommandAndArgs.charAt(commaPosition + 5),
		      aCommandAndArgs.charAt(commaPosition + 6), aCommandAndArgs.charAt(commaPosition + 7), aCommandAndArgs.charAt(commaPosition + 8),
		      aCommandAndArgs.charAt(commaPosition + 9), aCommandAndArgs.charAt(commaPosition + 10), aCommandAndArgs.charAt(commaPosition + 11));

		TCPClient client;
		if (client.connect(ip, port)) {
	    	DEBUG("TCPClient connected");
	    	client.write("Hey there mr. server!");
	    	// client.println("GET /search?q=unicorn HTTP/1.0");
	    	// client.println("Host: www.google.com");
	    	// client.println("Content-Length: 0");
	    	// client.println();
	    	delay(1000);
	    	while (client.available()) {
	    		DEBUG("%c", client.read());
	    		//Serial.print(client.read());
	    	}
	    	//Serial.println("");
	  	}
	  	else
	  	{
	    	Serial.println("TCPClient connection failed");
	 	}
		// Get server IP address/port (e.g. 1.1.1.1:80) from arguments
		//String serverAddressAndPort = aCommandAndArgs.substring(commaPosition + 1, aCommandAndArgs.length());


		//downloadStoryData(serverAddressAndPort);

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


TCPClient* ServerManager::connectToServer(byte server[4], uint16_t port) {
	DEBUG("Connecting to client at %u.%u.%u.%u:%u", server[0], server[1], server[2], server[3], port);

	TCPClient* client = new TCPClient();
	if (client->connect(server, port)) {
    	DEBUG("TCPClient connected");
    	return client;
  	}
  	else {
    	DEBUG("TCPClient connection failed");
    	Errors::setError(E_SERVER_CONNECTION_FAILED);
		ERROR(Errors::errorString());
		return NULL;
 	}
}

bool ServerManager::getStoryData(TCPClient *client) {
	if (client->connected()) {
		int startTimeMs = millis();
		int currentTimeMs = startTimeMs;
		// While connected, wait for data availability (with timeout)
		while ((currentTimeMs - startTimeMs) < kServerTimeout) {
			currentTimeMs = millis();
			while (client->available() == 0) {
				startTimeMs = currentTimeMs;
				// While data available, read data into buffer, then flash space based on page size
				uint8_t theByte = client->read();
			}
		}

	}
	else {
		// TODO: ERROR
		return false;
	}
}

}