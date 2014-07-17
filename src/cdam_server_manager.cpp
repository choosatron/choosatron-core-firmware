#include "cdam_manager.h"
#include "cdam_constants.h"
#include "spark_wiring_network.h"
#include "memory_free.h"

namespace cdam
{

const int8_t kServerReturnInvalidIndex = -4;
const int8_t kServerReturnBusy = -3;
const int8_t kServerReturnInvalidCmd = -2;
const int8_t kServerReturnFail = -1;
const int8_t kServerReturnSuccess = 0;
const int8_t kServerReturnEventIncoming = 1;
const int8_t kServerReturnConnecting = 2;

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
const char* kServerCmdSwapStoryPositions = "swap_story_pos";

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

const uint16_t kServerDataBufferSize = 128;
const uint16_t kServerTimeout = 30000;	// ms
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

	this->pendingAction = false;
	this->newStoryIndex = 0;
	this->newStorySize = 0;
}

void ServerManager::handlePendingActions() {
	if (this->pendingAction) {
		TCPClient *client = new TCPClient();
		DEBUG("Connecting to client at %u.%u.%u.%u:%u", this->serverIp[0], this->serverIp[1], this->serverIp[2], this->serverIp[3], this->serverPort);

		if (client->connect(this->serverIp, this->serverPort)) {
	    	DEBUG("TCPClient connected");
	    	if (strcmp(this->lastCommand, kServerCmdAddStory) == 0) {
	    		if (getStoryData(client, this->newStorySize)) {
	    			// Update the data manager metadata for the new story.
	    			DEBUG("Index: %d, Size: %lu", this->newStoryIndex, this->newStorySize);
	    			Manager::getInstance().dataManager->addStoryMetadata(this->newStoryIndex, this->newStorySize);
	    		}
	    	}
	    	client->stop();
	  	} else {
	    	DEBUG("TCPClient connection failed");
	    	Errors::setError(E_SERVER_CONNECTION_FAIL);
			ERROR(Errors::errorString());
	 	}
	 	free(client);

	 	this->newStoryIndex = 0;
	 	this->newStorySize = 0;
	 	this->pendingAction = false;
	}
}

/* Static Methods */
int ServerManager::serverCommand(String aCommandAndArgs) {
	ServerManager* serverMan = Manager::getInstance().serverManager;

	//aCommandAndArgs.trim();
    //aCommandAndArgs.toLowerCase();


    int cmdLen = aCommandAndArgs.length() + 1;
    int delimiterPos = aCommandAndArgs.indexOf(kServerArgumentDelimiter);

	aCommandAndArgs.toCharArray(serverMan->lastCommand, delimiterPos + 1);
    DEBUG("Command Received: %s", serverMan->lastCommand);

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
		uint8_t storyIndex = ((aCommandAndArgs.charAt(delimiterPos + 1) - '0') % 48) - 1;
		if (storyIndex < (Manager::getInstance().dataManager->metadata.storyCount)) {
			Manager::getInstance().dataManager->removeStoryMetadata(storyIndex);
			return kServerReturnSuccess;
		}
		return kServerReturnInvalidIndex;
	} else if (strcmp(command, kServerCmdRemoveAllStories) == 0) {
		Manager::getInstance().dataManager->removeAllStoryData();
		return kServerReturnSuccess;
	} else if (strcmp(command, kServerCmdAddStory) == 0) {
		// We only want to deal with one server connection at a time.
		if (serverMan->pendingAction) {
			return kServerReturnBusy;
		}

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

		serverMan->newStorySize = atol(buffer);
		DEBUG("Incoming story size: %lu", serverMan->newStorySize);

		DEBUG("Total Story Size: %lu", Manager::getInstance().dataManager->usedStoryBytes);
		if (serverMan->newStorySize > (kFlashMaxStoryBytes - Manager::getInstance().dataManager->usedStoryBytes)) {
			Errors::setError(E_SERVER_ADD_STORY_NO_SPACE);
			ERROR(Errors::errorString());
			return kServerReturnFail;
		}

		serverMan->newStoryIndex = ((aCommandAndArgs.charAt(index) - '0') % 48) - 1;
		DEBUG("Story Index: %d", serverMan->newStoryIndex);
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

		serverMan->serverIp[0] = firstOctet;
		serverMan->serverIp[1] = secondOctet;
		serverMan->serverIp[2] = thirdOctet;
		serverMan->serverIp[3] = fourthOctet;

		aCommandAndArgs.toCharArray(buffer, kServerStoryPortBytes + 1, index);
		serverMan->serverPort = atoi(buffer);

		serverMan->pendingAction = true;

		return kServerReturnConnecting;



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

	} else if (strcmp(command, kServerCmdSwapStoryPositions) == 0) {
		/* TODO */
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

/* Private Methods */
bool ServerManager::getStoryData(TCPClient *aClient, uint32_t aStorySize) {
	if (aClient->connected()) {
		aClient->write("hi");
		int startTimeMs = millis();
		// While connected, wait for data availability (with timeout)
		while ((millis() - startTimeMs) < kServerTimeout) {
			if (aClient->available()) {
				uint16_t index = 0;
				uint32_t bytesRead = 0;
				uint8_t buffer[kServerDataBufferSize + 1] = "";
				memset(&buffer[0], 0, sizeof(buffer));
				DEBUG("Story Size: %lu", aStorySize);

				while (aClient->available()) {
					// While data available, read data into buffer, then flash space based on page size
					memset(&buffer[0], 0, sizeof(buffer));
					int bytes = aClient->read(buffer, kServerDataBufferSize);
					// Write data
					bool result = Manager::getInstance().dataManager->storyFlash()->write(buffer,
						                       Manager::getInstance().dataManager->usedStoryBytes +
						                       bytesRead, bytes);
					bytesRead += bytes;
					DEBUG("%d", bytes);
					DEBUG("%d", bytesRead);
					DEBUG("%s", buffer);


					if (!result) {
						Errors::setError(E_SERVER_SOCKET_DATA_FAIL);
						ERROR(Errors::errorString());
						return false;
					}
					if (bytesRead == aStorySize) {


							DEBUG("Data Received");
							aClient->write("received");
							if (aClient->available()) {
								Errors::setError(E_SERVER_SOCKET_DATA_FAIL);
								ERROR(Errors::errorString());
							}
							char data[512] = "";
							DEBUG("Len to read: %lu", bytesRead);
							bool result = Manager::getInstance().dataManager->storyFlash()->read(data,
							                       Manager::getInstance().dataManager->usedStoryBytes,
							                       bytesRead);
							if (result) {
								DEBUG("%s", data);
								Serial.println(data);
							} else {
								ERROR("Couldn't read!");
							}
							return true;

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