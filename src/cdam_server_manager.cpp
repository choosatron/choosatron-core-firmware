#include "cdam_manager.h"
#include "cdam_constants.h"
#include "spark_wiring_wifi.h"

namespace cdam
{

const int8_t kServerReturnMaxReached = -5;
const int8_t kServerReturnInvalidIndex = -4;
const int8_t kServerReturnBusy = -3;
const int8_t kServerReturnInvalidCmd = -2;
const int8_t kServerReturnFail = -1;
const int8_t kServerReturnSuccess = 0;
const int8_t kServerReturnEventIncoming = 1;
const int8_t kServerReturnConnecting = 2;

const char* kServerVarLastCmd = "last_command";

// Commands.
const char* kServerCmd = "command";
const char* kServerCmdPing = "ping";
const char* kServerCmdKeypadInput = "keypad_input";
const char* kServerCmdButtonInput = "button_input";
const char* kServerCmdAdjustCredits = "adjust_credits";
const char* kServerCmdSetCredits = "set_credits";
const char* kServerCmdRemoveStory = "remove_story";
const char* kServerCmdRemoveAllStories = "remove_all_stories";
const char* kServerCmdSwapStoryPositions = "swap_story_pos";
const char* kServerCmdSetFlag = "set_flag";

// Admin commands.
const char* kServerCmdAdminResetMetadata = "reset_metadata";
const char* kServerCmdAdminEraseFlash = "erase_flash";
const char* kServerCmdAdminResetUnit = "reset_unit";

// Data requests.
const char* kServerCmdGetStoryCount = "get_story_count";
const char* kServerCmdGetMillis = "get_millis";
const char* kServerCmdGetSeconds = "get_seconds";
const char* kServerCmdGetFreeSpace = "get_free_space";
const char* kServerCmdGetUsedSpace = "get_used_space";
const char* kServerCmdGetCredits = "get_credits";
const char* kServerCmdGetRSSI = "get_rssi";
const char* kServerCmdGetState = "get_state"; // Publishes event.
const char* kServerCmdGetSSID = "get_ssid"; // Publishes event.
const char* kServerCmdGetGatewayIP = "get_gateway_ip"; // Publishes event.
const char* kServerCmdGetMacAddr = "get_mac_addr"; // Publishes event.
const char* kServerCmdGetSubnetMask = "get_subnet_mask"; // Publishes event.
const char* kServerCmdGetLocalIP = "get_local_ip"; // Publishes event.

// Requires TCP Connection.
const char* kServerCmdAddStory = "add_story";

const uint16_t kServerDataBufferSize = 128; // Must evening devide in Flashee pagesize (4096 currently)
const uint16_t kServerTimeout = 30000;	// ms
const char kServerArgumentDelimiter = '|';

const uint8_t kServerStoryPositionBytes = 2;
const uint8_t kServerStorySizeBytes = 7;
const uint8_t kServerStoryOctetBytes = 3;
const uint8_t kServerStoryPortBytes = 6;

/* Public Methods */

ServerManager::ServerManager() {
}

void ServerManager::initialize(StateController *aStateController) {
	_stateControl = aStateController;
	this->pendingAction = false;
	this->pendingCommand = NULL;
	this->pendingArguments = NULL;

	// Setup our command function hook for the server.
	Spark.function(kServerCmd, serverCommand);
	// Expose the last command issued.
	Spark.variable(kServerVarLastCmd , this->pendingCommand, STRING);
}

void ServerManager::handlePendingActions() {
	if (this->pendingAction) {
		// Attempt TCP connection.
		TCPClient *client = new TCPClient();
		if (client->connect(this->serverIp, this->serverPort)) {
			DEBUG("TCPClient connected");
			// Add Story Command
			if (strcmp(Manager::getInstance().serverManager->pendingCommand, kServerCmdAddStory) == 0) {
				char buffer[8] = "";
				memcpy(buffer, this->pendingArguments, kServerStorySizeBytes);
				uint32_t newStorySize = atol(buffer);
				DEBUG("Incoming story size: %lu", newStorySize);

				DEBUG("Total Pages: %d", Manager::getInstance().dataManager->metadata.usedStoryPages);
				if (newStorySize > (kFlashMaxStoryBytes -
				    (Manager::getInstance().dataManager->metadata.usedStoryPages * Flashee::Devices::userFlash().pageSize()))) {
					Errors::setError(E_SERVER_ADD_STORY_NO_SPACE);
					ERROR(Errors::errorString());
					client->write(Errors::errorString());
				} else {
					//serverMan->newStoryIndex = ((serverMan->pendingArguments[kServerStorySizeBytes] - '0') % 48) - 1;
					memset(&buffer[0], 0, sizeof(buffer));
					memcpy(buffer, this->pendingArguments + kServerStorySizeBytes, kServerStoryPositionBytes);
					uint8_t newStoryIndex = atoi(buffer);
					DEBUG("Story Index: %d", newStoryIndex);
					DEBUG("Connecting to client at %u.%u.%u.%u:%u", this->serverIp[0], this->serverIp[1], this->serverIp[2], this->serverIp[3], this->serverPort);

					if (getStoryData(client, newStorySize)) {
						// Update the data manager metadata for the new story.
						DEBUG("Index: %d, Size: %lu", newStoryIndex, newStorySize);
						uint8_t pages = newStorySize / Flashee::Devices::userFlash().pageSize();
						if (newStorySize % Flashee::Devices::userFlash().pageSize()) {
							pages++;
						}
						Manager::getInstance().dataManager->addStoryMetadata(newStoryIndex, pages);
					}
				}
			}
	 	} else {
			DEBUG("TCPClient connection failed");
			Errors::setError(E_SERVER_CONNECTION_FAIL);
			ERROR(Errors::errorString());
		}
	 	// Cleanup
 		client->stop();
	 	delete client;
	 	delete [] this->pendingCommand;
	 	delete [] this->pendingArguments;

	 	this->pendingAction = false;
	}
}

/* Static Methods */
int ServerManager::serverCommand(String aCommandAndArgs) {
	DataManager* dataMan = Manager::getInstance().dataManager;
	ServerManager* serverMan = Manager::getInstance().serverManager;
	HardwareManager* hardMan = Manager::getInstance().hardwareManager;

	int32_t returnVal = kServerReturnSuccess;

	//aCommandAndArgs.trim();
    //aCommandAndArgs.toLowerCase();

	int cmdLen = aCommandAndArgs.length();
    int delimiterPos = aCommandAndArgs.indexOf(kServerArgumentDelimiter);

	char commandAndArgs[cmdLen + 1];
	aCommandAndArgs.toCharArray(commandAndArgs, sizeof(commandAndArgs));
	DEBUG("Command and Args: %s", commandAndArgs);

    if (delimiterPos > -1) {
		int serverDelimPos = aCommandAndArgs.indexOf(kServerArgumentDelimiter, delimiterPos + 1);
    	if (serverDelimPos > -1) {
    		char serverAddress[cmdLen - serverDelimPos];
    		memcpy(serverAddress, commandAndArgs + serverDelimPos + 1, cmdLen - serverDelimPos - 1);
    		serverMan->parseServerAddress(serverAddress);
    		cmdLen = serverDelimPos;
    	}
		serverMan->pendingArguments = new char[cmdLen - delimiterPos];
		memcpy(serverMan->pendingArguments, commandAndArgs + delimiterPos + 1, cmdLen - delimiterPos - 1);
		cmdLen = delimiterPos;
	}

    serverMan->pendingCommand = new char[cmdLen + 1];
    memcpy(serverMan->pendingCommand, commandAndArgs, cmdLen);

	if (strcmp(serverMan->pendingCommand, kServerCmdPing) == 0) {
		//return kServerReturnSuccess;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdKeypadInput) == 0) {
		/* TODO */
		uint8_t keypadVal = atoi(serverMan->pendingArguments);
		DEBUG("Keypad Val: %d", keypadVal);
	} else if (strcmp(serverMan->pendingCommand, kServerCmdButtonInput) == 0) {
		/* TODO */
	} else if (strcmp(serverMan->pendingCommand, kServerCmdAdjustCredits) == 0) {
		hardMan->coinAcceptor()->addCredits(atoi(serverMan->pendingArguments));
		returnVal = hardMan->coinAcceptor()->getCredits();
	} else if (strcmp(serverMan->pendingCommand, kServerCmdSetCredits) == 0) {
		hardMan->coinAcceptor()->setCredits(atoi(serverMan->pendingArguments));
		returnVal = hardMan->coinAcceptor()->getCredits();
	/*} else if (strcmp(serverMan->pendingCommand, kServerCmdRemoveStory) == 0) {
		uint8_t storyIndex = ((aCommandAndArgs.charAt(delimiterPos + 1) - '0') % 48) - 1;
		if (storyIndex < (Manager::getInstance().dataManager->metadata.storyCount)) {
			Manager::getInstance().dataManager->removeStoryMetadata(storyIndex);
			return kServerReturnSuccess;
		}
		return kServerReturnInvalidIndex;*/
	} else if (strcmp(serverMan->pendingCommand, kServerCmdRemoveAllStories) == 0) {
		dataMan->removeAllStoryData();
	} else if (strcmp(serverMan->pendingCommand, kServerCmdSwapStoryPositions) == 0) {
		/* TODO */
	} else if (strcmp(serverMan->pendingCommand, kServerCmdSetFlag) == 0) {
		// Byte 1: Flag Index, Byte 2: Bit Index, Byte 3: 0 or 1 for OFF or ON
		uint8_t flagIndex = aCommandAndArgs.charAt(delimiterPos + 1) - '0';
		uint8_t bitIndex = aCommandAndArgs.charAt(delimiterPos + 2) - '0';
		bool value = aCommandAndArgs.charAt(delimiterPos + 3) - '0';
		DEBUG("Flag Index: %d, Bit Index: %d, Value: %d", flagIndex, bitIndex, value);
		if (!dataMan->setFlag(flagIndex, bitIndex, value)) {
			returnVal = kServerReturnFail;
		}
	} else if (strcmp(serverMan->pendingCommand, kServerCmdAdminResetMetadata) == 0) {
		if (!dataMan->resetMetadata()) {
			return kServerReturnFail;
		}
	} else if (strcmp(serverMan->pendingCommand, kServerCmdAdminEraseFlash) == 0) {
		if (!dataMan->eraseFlash()) {
			return kServerReturnFail;
		}
	} else if (strcmp(serverMan->pendingCommand, kServerCmdAdminResetUnit) == 0) {
		dataMan->stateController()->changeState(STATE_INIT);
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetStoryCount) == 0) {
		returnVal = dataMan->metadata.storyCount;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetMillis) == 0) {
		returnVal = millis();
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetSeconds) == 0) {
		returnVal = millis() / 1000;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetFreeSpace) == 0) {
		returnVal = kFlashMaxStoryBytes - (dataMan->metadata.usedStoryPages * Flashee::Devices::userFlash().pageSize());
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetUsedSpace) == 0) {
		returnVal = dataMan->metadata.usedStoryPages * Flashee::Devices::userFlash().pageSize();
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetCredits) == 0) {
		returnVal = hardMan->coinAcceptor()->getCredits();
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetRSSI) == 0) {
		returnVal = WiFi.RSSI();
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetState) == 0) {
		Spark.publish(kServerCmdGetState, dataMan->stateController()->stateString(), kServerTTLDefault, PRIVATE);
		returnVal = kServerReturnEventIncoming;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetSSID) == 0) {
		Spark.publish(kServerCmdGetSSID, WiFi.SSID(), kServerTTLDefault, PRIVATE);
		returnVal = kServerReturnEventIncoming;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetGatewayIP) == 0) {
		uint8_t *address = WiFi.gatewayIP().raw_address();
		char addr[16] = "";
		snprintf(addr, 16, "%d.%d.%d.%d", address[0], address[1], address[2], address[3]);
		Spark.publish(kServerCmdGetGatewayIP, addr, kServerTTLDefault, PRIVATE);
		returnVal = kServerReturnEventIncoming;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetMacAddr) == 0) {
		byte macVal[6];
		WiFi.macAddress(macVal);
		char macAddr[18];
		snprintf(macAddr, 18, "%02x:%02x:%02x:%02x:%02x:%02x", macVal[5], macVal[4], macVal[3], macVal[2], macVal[1], macVal[0]);
		Spark.publish(kServerCmdGetMacAddr, macAddr, kServerTTLDefault, PRIVATE);
		returnVal = kServerReturnEventIncoming;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetSubnetMask) == 0) {
		uint8_t *address = WiFi.subnetMask().raw_address();
		char addr[16] = "";
		snprintf(addr, 16, "%d.%d.%d.%d", address[0], address[1], address[2], address[3]);
		Spark.publish(kServerCmdGetSubnetMask, addr, kServerTTLDefault, PRIVATE);
		returnVal = kServerReturnEventIncoming;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetLocalIP) == 0) {
		uint8_t *address = WiFi.localIP().raw_address();
		char addr[16] = "";
		snprintf(addr, 16, "%d.%d.%d.%d", address[0], address[1], address[2], address[3]);
		Spark.publish(kServerCmdGetLocalIP, addr, kServerTTLDefault, PRIVATE);
		returnVal = kServerReturnEventIncoming;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdAddStory) == 0) {
		// We only want to deal with one server connection at a time.
		if (serverMan->pendingAction) {
			returnVal =  kServerReturnBusy;
		} else if (dataMan->metadata.storyCount >= kMaxStoryCount) {
			returnVal =  kServerReturnMaxReached;
		} else {
			serverMan->pendingAction = true;
			returnVal = kServerReturnConnecting;
		}

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
	} else {
		returnVal = kServerReturnInvalidCmd;
	}

	// No pending action, so cleanup.
	if (!serverMan->pendingAction) {
		delete [] serverMan->pendingCommand;
		delete [] serverMan->pendingArguments;
	}

	return returnVal;
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
void ServerManager::parseServerAddress(char* aAddress) {
	char buffer[6] = "";
	uint8_t index = kServerStoryOctetBytes;

	memcpy(buffer, aAddress, 3);
	Manager::getInstance().serverManager->serverIp[0] = atoi(buffer);

	memcpy(buffer, aAddress + index, 3);
	Manager::getInstance().serverManager->serverIp[1] = atoi(buffer);
	index += kServerStoryOctetBytes;

	memcpy(buffer, aAddress + index, 3);
	Manager::getInstance().serverManager->serverIp[2] = atoi(buffer);
	index += kServerStoryOctetBytes;

	memcpy(buffer, aAddress + index, 3);
	Manager::getInstance().serverManager->serverIp[3] = atoi(buffer);
	index += kServerStoryOctetBytes;

	memcpy(buffer, aAddress + index, 5);
	Manager::getInstance().serverManager->serverPort = atoi(buffer);

	DEBUG("IP: %d.%d.%d.%d:%d", Manager::getInstance().serverManager->serverIp[0], Manager::getInstance().serverManager->serverIp[1], Manager::getInstance().serverManager->serverIp[2], Manager::getInstance().serverManager->serverIp[3], Manager::getInstance().serverManager->serverPort);
}

bool ServerManager::getStoryData(TCPClient *aClient, uint32_t aStorySize) {
	if (aClient->connected()) {
		aClient->write("READY");
		int startTimeMs = millis();

		// Wait for data availability (with timeout)
		while (!aClient->available() && ((millis() - startTimeMs) < kServerTimeout)) {}

		if (aClient->available()) {
			//uint16_t index = 0;
			uint32_t bytesRead = 0;
			uint32_t bytesToRead = kServerDataBufferSize;
			uint8_t pagesWritten = 0;
			uint8_t buffer[Flashee::Devices::userFlash().pageSize() + 1];
			memset(&buffer[0], 0, sizeof(buffer));
			DEBUG("Story Size: %lu", aStorySize);

			while (aClient->available()) {
				// While data available, read data into buffer, then flash space based on page size
				//memset(&buffer[0], 0, sizeof(buffer));
				if ((aStorySize - bytesRead) < kServerDataBufferSize) {
					bytesToRead = aStorySize - bytesRead;
				}
				int16_t bytes = aClient->read(buffer + bytesRead, bytesToRead);
				DEBUG("Bytes read: %d", bytes);

				if (bytes > 0) {
					bytesRead += bytes;

					if ((bytesRead % Flashee::Devices::userFlash().pageSize() == 0) ||
						(bytesRead == aStorySize)) {
						// Write data
						DEBUG("Writing page #: %d", (pagesWritten + 1));
						DEBUG("Used Pages: %d", Manager::getInstance().dataManager->metadata.usedStoryPages);
						bool result = Manager::getInstance().dataManager->writeData(buffer,
						                       (Manager::getInstance().dataManager->metadata.usedStoryPages * Flashee::Devices::userFlash().pageSize()) +
						                       (pagesWritten * Flashee::Devices::userFlash().pageSize()), Flashee::Devices::userFlash().pageSize());

						if (!result) {
							Errors::setError(E_SERVER_SOCKET_DATA_FAIL);
							ERROR(Errors::errorString());
							aClient->write(Errors::errorString());
							break;
						}
						memset(&buffer[0], 0, sizeof(buffer));
						pagesWritten += 1;
					}

					if (bytesRead == aStorySize) {
						DEBUG("Data Received");
						aClient->write("COMPLETE");
						if (aClient->available()) {
							Errors::setError(E_SERVER_SOCKET_DATA_FAIL);
							ERROR(Errors::errorString());
							aClient->write(Errors::errorString());
						}
						/*DEBUG("Used: %d, Pages: %d", Manager::getInstance().dataManager->metadata.usedStoryPages, pagesWritten);
						bool result = Manager::getInstance().dataManager->readData(buffer,
						                       Manager::getInstance().dataManager->metadata.usedStoryPages * Flashee::Devices::userFlash().pageSize(),
						                       pagesWritten * Flashee::Devices::userFlash().pageSize());
						if (result) {
							Serial.println((char *)buffer);
						}*/
						return true;
					}
				}
			}
		} else {
			DEBUG("Timed out waiting to receive story data.");
		}
	} else {
		Errors::setError(E_SERVER_CONNECTION_FAIL);
		ERROR(Errors::errorString());
	}
	Errors::setError(E_SERVER_SOCKET_DATA_FAIL);
	ERROR(Errors::errorString());
	aClient->write(Errors::errorString());
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