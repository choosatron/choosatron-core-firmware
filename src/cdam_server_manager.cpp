#include "cdam_manager.h"
#include "cdam_constants.h"
#include "spark_wiring_network.h"

namespace cdam
{

const int8_t kServerReturnInvalidCmd = -2;
const int8_t kServerReturnFail = -1;
const int8_t kServerReturnSuccess = 0;
const int8_t kServerReturnEventIncoming = 1;

const char* kServerVarLastCmd = "last_command";

const char* kServerCmd = "command";
const char* kServerCmdPing = "ping";
const char* kServerCmdKeypadInput = "keypad_input";
const char* kServerCmdButtonInput = "button_input";
const char* kServerCmdAdjustCredits = "adjust_credits";
const char* kServerCmdSetCredits = "set_credits";
const char* kServerCmdRemoveStory = "remove_story";
const char* kServerCmdRemoveAllStories = "remove_all_stories";
const char* kServerCmdAddStory = "add_story";

// Commands that cause the Choosatron to throw events.
const char* kServerCmdGetState = "get_state";
const char* kServerCmdGetStoryCount = "get_story_count";
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

const uint16_t kServerDataBufferSize = 256;
const uint16_t kServerTimeout = 30000;	// ms
const uint16_t kServerDefaultTCPPort = 80;
const char kServerArgumentDelimiter = '|';

const uint8_t kServerStoryPositionBytes = 1;
const uint8_t kServerStorySizeBytes = 7;
const uint8_t kServerStoryOctetBytes = 3;
const uint8_t kServerStoryPortBytes = 6;

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
    int delimiterPos = aCommandAndArgs.indexOf(kServerArgumentDelimiter);
	if (delimiterPos > -1) {
		DEBUG("Has Arguments");
		cmdLen = delimiterPos + 1;
	} else {
		DEBUG("No Arguments");
	}
	char command[cmdLen];
	aCommandAndArgs.toCharArray(command, cmdLen);

	DEBUG("Cmd: %s", command);
	if (strcmp(command, kServerCmdPing) == 0) {
		return kServerReturnSuccess;
	} else if (strcmp(command, kServerCmdKeypadInput) == 0) {
		/* TODO */
		String keypadStr = aCommandAndArgs.substring(delimiterPos + 1, aCommandAndArgs.length());
		uint8_t keypadVal = atoi(keypadStr.c_str());
		DEBUG("Keypad Val: %d", keypadVal);
		return kServerReturnSuccess;
	} else if (strcmp(command, kServerCmdButtonInput) == 0) {
		/* TODO */
		return kServerReturnSuccess;
	} else if (strcmp(command, kServerCmdAdjustCredits) == 0) {
		String creditsStr = aCommandAndArgs.substring(delimiterPos + 1, aCommandAndArgs.length());
		Manager::getInstance().dataManager->gameCredits += atoi(creditsStr.c_str());
		return Manager::getInstance().dataManager->gameCredits;
	} else if (strcmp(command, kServerCmdSetCredits) == 0) {
		String creditsStr = aCommandAndArgs.substring(delimiterPos + 1, aCommandAndArgs.length());
		Manager::getInstance().dataManager->gameCredits = atoi(creditsStr.c_str());
		return Manager::getInstance().dataManager->gameCredits;
	} else if (strcmp(command, kServerCmdRemoveStory) == 0) {
		/* TODO */
		return kServerReturnSuccess;
	} else if (strcmp(command, kServerCmdRemoveAllStories) == 0) {
		/* TODO */
		return kServerReturnSuccess;
	} else if (strcmp(command, kServerCmdAddStory) == 0) {

		/* 35 bytes of arguments - In Order */
		// 10 bytes - Command Name & Delimiter
		//  7 bytes - Binary Blob Byte Size
		//  1 byte  - Story Position
		//  3 bytes - IP Address Octet One
		//  3 bytes - IP Address Octet Two
		//  3 bytes - IP Address Octet Three
		//  3 bytes - IP Address Octet Four
		//  5 bytes - Port Number
		if (aCommandAndArgs.length() != 35) {
			Errors::setError(E_SERVER_ADD_STORY_LEN_FAIL);
			ERROR(Errors::errorString());
			DEBUG("Length: %d", aCommandAndArgs.length());
			return kServerReturnFail;
		}

		uint8_t index = delimiterPos + 1;
		char buffer[8] = "";

		aCommandAndArgs.toCharArray(buffer, kServerStorySizeBytes + 1, index);
		index += kServerStorySizeBytes;

		uint32_t storySize = atol(buffer);
		DEBUG("Incoming story size: %lu", storySize);

		DEBUG("Total Story Size: %lu", Manager::getInstance().dataManager->usedStoryBytes);
		if (storySize > (kFlashMaxStoryBytes - Manager::getInstance().dataManager->usedStoryBytes)) {
			Errors::setError(E_SERVER_ADD_STORY_NO_SPACE);
			ERROR(Errors::errorString());
			return kServerReturnFail;
		}

		uint8_t storyPos = (aCommandAndArgs.charAt(index) - '0') % 48;
		DEBUG("Story Pos: %d", storyPos);
		uint8_t count = Manager::getInstance().dataManager->metadata.storyCount;
		if (storyPos > count) {
			storyPos = count + 1;
		} else {
			Manager::getInstance().dataManager->metadata.storySizes.push_back(0);
			while (count >= storyPos) {
				Manager::getInstance().dataManager->metadata.storySizes[count] = Manager::getInstance().dataManager->metadata.storySizes[count - 1];
				count--;
			}
			Manager::getInstance().dataManager->metadata.storySizes[storyPos - 1] = storySize;
		}
		index++;

		aCommandAndArgs.toCharArray(buffer, kServerStoryOctetBytes + 1, index);
		uint8_t firstOctet = atoi(buffer);
		index += kServerStoryOctetBytes;

		aCommandAndArgs.toCharArray(buffer, kServerStoryOctetBytes + 1, index);
		uint8_t secondOctet = atoi(buffer);
		index += kServerStoryOctetBytes;

		aCommandAndArgs.toCharArray(buffer, kServerStoryOctetBytes + 1, index);
		uint8_t thirdOctet = atoi(buffer);
		index += kServerStoryOctetBytes;

		aCommandAndArgs.toCharArray(buffer, kServerStoryOctetBytes + 1, index);
		uint8_t fourthOctet = atoi(buffer);
		index += kServerStoryOctetBytes;

		byte ip[] = {firstOctet, secondOctet, thirdOctet, fourthOctet};

		aCommandAndArgs.toCharArray(buffer, kServerStoryPortBytes + 1, index);
		uint16_t port = atoi(buffer);


		TCPClient *client = new TCPClient();
		DEBUG("Connecting to client at %u.%u.%u.%u:%u", ip[0], ip[1], ip[2], ip[3], port);

		if (client->connect(ip, port)) {
	    	DEBUG("TCPClient connected");
	    	Manager::getInstance().serverManager->getStoryData(client, storySize);
	    	client->stop();
	  	} else {
	    	DEBUG("TCPClient connection failed");
	    	Errors::setError(E_SERVER_CONNECTION_FAIL);
			ERROR(Errors::errorString());
			return kServerReturnFail;
	 	}
	 	free(client);

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
		/*byte ip[] = { aCommandAndArgs.charAt(delimiterPos + 1), aCommandAndArgs.charAt(delimiterPos + 2),
			aCommandAndArgs.charAt(delimiterPos + 3), aCommandAndArgs.charAt(delimiterPos + 4)};

		uint16_t port = aCommandAndArgs.charAt(delimiterPos + 5) | (aCommandAndArgs.charAt(delimiterPos + 6) << 8);
		DEBUG("Port: %d", port);
		uint8_t storyPos = aCommandAndArgs.charAt(delimiterPos + 7);
		DEBUG("Story Pos: %d", storyPos);
		uint32_t storySize = aCommandAndArgs.charAt(delimiterPos + 8) | (aCommandAndArgs.charAt(delimiterPos + 9) << 8) | (aCommandAndArgs.charAt(delimiterPos + 10) << 16) | (aCommandAndArgs.charAt(delimiterPos + 11) << 24);
		DEBUG("Story Size: %Lu", storySize);
		DEBUG("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", aCommandAndArgs.charAt(delimiterPos + 1), aCommandAndArgs.charAt(delimiterPos + 2), aCommandAndArgs.charAt(delimiterPos + 3), aCommandAndArgs.charAt(delimiterPos + 4), aCommandAndArgs.charAt(delimiterPos + 5),
		      aCommandAndArgs.charAt(delimiterPos + 6), aCommandAndArgs.charAt(delimiterPos + 7), aCommandAndArgs.charAt(delimiterPos + 8),
		      aCommandAndArgs.charAt(delimiterPos + 9), aCommandAndArgs.charAt(delimiterPos + 10), aCommandAndArgs.charAt(delimiterPos + 11));

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
	  	} else {
	    	Serial.println("TCPClient connection failed");
	 	}*/
		// Get server IP address/port (e.g. 1.1.1.1:80) from arguments
		//String serverAddressAndPort = aCommandAndArgs.substring(delimiterPos + 1, aCommandAndArgs.length());


		//downloadStoryData(serverAddressAndPort);

	} else if (strcmp(command, kServerCmdGetState) == 0) {
		Spark.publish(kServerCmdGetState, Manager::getInstance().dataManager->gameStateStr(), kServerTTLDefault, PRIVATE);
		return kServerReturnEventIncoming;
	} else if (strcmp(command, kServerCmdGetStoryCount) == 0) {
		return Manager::getInstance().dataManager->metadata.storyCount;
	} else if (strcmp(command, kServerCmdGetMillis) == 0) {
		return millis();
	} else if (strcmp(command, kServerCmdGetSeconds) == 0) {
		return millis() / 1000;
	} else if (strcmp(command, kServerCmdGetFreeSpace) == 0) {
		return kFlashMaxStoryBytes - Manager::getInstance().dataManager->usedStoryBytes;
	} else if (strcmp(command, kServerCmdGetUsedSpace) == 0) {
		return Manager::getInstance().dataManager->usedStoryBytes;
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

	return kServerReturnInvalidCmd;
}

/*TCPClient* ServerManager::connectToServer(byte server[4], uint16_t port) {
	DEBUG("Connecting to client at %u.%u.%u.%u:%u", server[0], server[1], server[2], server[3], port);

	TCPClient* client = new TCPClient();
	if (client->connect(server, port)) {
    	DEBUG("TCPClient connected");
    	return client;
  	} else {
    	DEBUG("TCPClient connection failed");
    	Errors::setError(E_SERVER_CONNECTION_FAIL);
		ERROR(Errors::errorString());
		return NULL;
 	}
}*/

bool ServerManager::getStoryData(TCPClient *aClient, uint32_t aByteCount) {
	if (aClient->connected()) {
		int startTimeMs = millis();
		// While connected, wait for data availability (with timeout)
		while ((millis() - startTimeMs) < kServerTimeout) {
			if (aClient->available()) {
				uint16_t index = 0;
				uint32_t chunks = 0;
				char buffer[kServerDataBufferSize] = "";
				while (aClient->available()) {
					// While data available, read data into buffer, then flash space based on page size
					buffer[index] = aClient->read();
					index++;
					aByteCount--;
					if ((index == kServerDataBufferSize) || (aByteCount == 0)) {
						// Write data
						Manager::getInstance().dataManager->storyFlash()->write(buffer,
						                       Manager::getInstance().dataManager->usedStoryBytes +
						                       kServerDataBufferSize * chunks, index);
						if (aByteCount == 0) {
							DEBUG("Data Received");
							if (aClient->available()) {
								Errors::setError(E_SERVER_SOCKET_DATA_FAIL);
								ERROR(Errors::errorString());
							}
							/*char data[512] = "";
							DEBUG("Len to read: %lu", kServerDataBufferSize * chunks + index);
							Manager::getInstance().dataManager->storyFlash()->read(data, 0,
							                       Manager::getInstance().dataManager->usedStoryBytes +
							                       kServerDataBufferSize * chunks + index);
							DEBUG("%s", data);
							Serial.println(data);*/
							return true;
						}
						index = 0;
						chunks++;
					}
				}
				Errors::setError(E_SERVER_SOCKET_DATA_FAIL);
				ERROR(Errors::errorString());
			}
		}
		DEBUG("Timed out waiting to receive story data.");
	} else {
		Errors::setError(E_SERVER_CONNECTION_FAIL);
		ERROR(Errors::errorString());
	}
	return false;
}

/*bool ServerManager::downloadStoryData(String aServerAddressAndPort) {
	// Configure port
    uint16_t port = kDefaultTCPPort;
    //int delimiterIndex = aServerAddressAndPort.index("");

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

}*/

}