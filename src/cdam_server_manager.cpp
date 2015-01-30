#include "cdam_manager.h"
#include "cdam_constants.h"
#include "spark_wiring_wifi.h"

namespace cdam
{

const int8_t kServerReturnNotImplemented = -6;
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
const char* kServerCmdSetValue = "set_value";
const char* kServerCmdAddWifiCreds = "add_wifi_creds";

// Admin commands.
const char* kServerCmdAdminResetMetadata = "reset_metadata";
const char* kServerCmdAdminEraseFlash = "erase_flash";
const char* kServerCmdAdminResetUnit = "reset_unit";

// Data requests.
const char* kServerCmdGetVersion = "get_version"; // Get firmware version (device ID included in response).
const char* kServerCmdGetFlag = "get_flag"; // Flag index 0 - 7, each has 8 bit flags.
const char* kServerCmdGetValue = "get_value"; // Value index 0 - 15, each has 16 bit value.
const char* kServerCmdGetNames = "get_names"; // Device name and owner name, colon separated.
const char* kServerCmdGetStoryInfo = "get_story_info"; // Get the UUID, version of a story, whether or not it is deleted, and # of pages it uses.
const char* kServerCmdGetCurrentStory = "get_current_story"; // Same as get_story_info, but on current playing story.
const char* kServerCmdGetAllStoryInfo = "get_all_story_info"; // Series of bytes to report all storage.
const char* kServerCmdGetStorageReport = "get_storage_report";
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
const uint16_t kStoryDataBufferSize = 1024; // Flashee::Devices::userFlash().pageSize()
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
	_stateController = aStateController;
	this->pendingAction = false;
	this->pendingCommand = NULL;
	this->pendingArguments = NULL;

	// Setup our command function hook for the server.
	Spark.function(kServerCmd, serverCommand);
	// Expose the last command issued.
	Spark.variable(kServerVarLastCmd , this->pendingCommand, STRING);
}

void ServerManager::handlePendingActions() {
	if (!SPARK_FLASH_UPDATE && this->pendingAction) {
		// Attempt TCP connection.
		TCPClient *client = new TCPClient();
		//DEBUG("Connecting to client at %u.%u.%u.%u:%u", this->serverIp[0], this->serverIp[1], this->serverIp[2], this->serverIp[3], this->serverPort);
		if (client->connect(this->serverIp, this->serverPort)) {
			//DEBUG("TCPClient connected");
			// Add Story Command
			if (strcmp(Manager::getInstance().serverManager->pendingCommand, kServerCmdAddStory) == 0) {
				char buffer[8] = "";
				memcpy(buffer, this->pendingArguments, kServerStorySizeBytes);
				uint32_t newStorySize = atol(buffer);
				//DEBUG("Incoming story size: %lu", newStorySize);

				//DEBUG("Total Pages: %d", Manager::getInstance().dataManager->metadata.usedStoryPages);
				if (newStorySize > (kFlashMaxStoryBytes -
				    (Manager::getInstance().dataManager->metadata.usedStoryPages * Flashee::Devices::userFlash().pageSize()))) {
					Errors::setError(E_SERVER_ADD_STORY_NO_SPACE);
					//ERROR(Errors::errorString());
					client->write(Errors::errorString());
				} else {
					//serverMan->newStoryIndex = ((serverMan->pendingArguments[kServerStorySizeBytes] - '0') % 48) - 1;
					memset(&buffer[0], 0, sizeof(buffer));
					memcpy(buffer, this->pendingArguments + kServerStorySizeBytes, kServerStoryPositionBytes);
					uint8_t newStoryIndex = atoi(buffer);
					//DEBUG("Story Index: %d", newStoryIndex);

					if (getStoryData(client, newStorySize)) {
						// Update the data manager metadata for the new story.
						//DEBUG("Index: %d, Size: %lu", newStoryIndex, newStorySize);
						uint8_t pages = newStorySize / Flashee::Devices::userFlash().pageSize();
						if (newStorySize % Flashee::Devices::userFlash().pageSize()) {
							pages++;
						}
						Manager::getInstance().dataManager->addStoryMetadata(newStoryIndex, pages);
						if (_stateController->getState() == STATE_SELECT) {
							Manager::getInstance().hardwareManager->printer()->println(CDAM_SERVER_REBOOT);
							_stateController->changeState(STATE_INIT);
						}
					}
				}
			}
	 	} else {
			Errors::setError(E_SERVER_CONNECTION_FAIL);
			//ERROR(Errors::errorString());
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
	memset(&commandAndArgs[0], 0, sizeof(commandAndArgs));
	aCommandAndArgs.toCharArray(commandAndArgs, sizeof(commandAndArgs));
	//DEBUG("Command and Args: %s", commandAndArgs);

	if (delimiterPos > -1) {
		int serverDelimPos = aCommandAndArgs.indexOf(kServerArgumentDelimiter, delimiterPos + 1);
		if (serverDelimPos > -1) {
			char serverAddress[cmdLen - serverDelimPos];
			memcpy(serverAddress, commandAndArgs + serverDelimPos + 1, cmdLen - serverDelimPos - 1);
			serverMan->parseServerAddress(serverAddress);
			cmdLen = serverDelimPos;
	}
		serverMan->pendingArguments = new char[cmdLen - delimiterPos]();
		memcpy(serverMan->pendingArguments, commandAndArgs + delimiterPos + 1, cmdLen - delimiterPos - 1);
		cmdLen = delimiterPos;
	}

	serverMan->pendingCommand = new char[cmdLen + 1]();
	memcpy(serverMan->pendingCommand, commandAndArgs, cmdLen);

	if (strcmp(serverMan->pendingCommand, kServerCmdPing) == 0) {
		//return kServerReturnSuccess;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdKeypadInput) == 0) {
		KeypadEvent event = (KeypadEvent)(serverMan->pendingArguments[0] - '0');
		uint8_t value = serverMan->pendingArguments[1] - '0';
		//DEBUG("Event: %d, Val: %d", event, value);
		hardMan->keypad()->setKeypadEvent(event, value);
	} else if (strcmp(serverMan->pendingCommand, kServerCmdButtonInput) == 0) {
		ButtonEvent event = (ButtonEvent)(serverMan->pendingArguments[0] - '0');
		uint8_t btnNum = serverMan->pendingArguments[1] - '0';
		//DEBUG("Event: %d, BtnNum: %d", event, btnNum);
		hardMan->keypad()->setButtonEvent(event, btnNum);
	} else if (strcmp(serverMan->pendingCommand, kServerCmdAdjustCredits) == 0) {
		int8_t credits = serverMan->pendingArguments[0] - '0';
		if (serverMan->pendingArguments[1] == '0') {
			credits *= -1;
		}
		hardMan->coinAcceptor()->addCredits(credits);
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
		returnVal = kServerReturnNotImplemented;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdSetFlag) == 0) {
		// Byte 1: Flag Index, Byte 2: Bit Index, Byte 3: 0 or 1 for OFF or ON
		uint8_t flagIndex = serverMan->pendingArguments[0] - '0';
		uint8_t bitIndex = serverMan->pendingArguments[1] - '0';
		bool value = serverMan->pendingArguments[2] - '0';
		//DEBUG("Flag Index: %d, Bit Index: %d, Value: %d", flagIndex, bitIndex, value);
		if (!dataMan->setFlag(flagIndex, bitIndex, value)) {
			returnVal = kServerReturnFail;
		}
	} else if (strcmp(serverMan->pendingCommand, kServerCmdAdminResetMetadata) == 0) {
		if (!dataMan->resetMetadata()) {
			returnVal = kServerReturnFail;
		}
	} else if (strcmp(serverMan->pendingCommand, kServerCmdAdminEraseFlash) == 0) {
		if (!dataMan->eraseFlash()) {
			returnVal = kServerReturnFail;
		}
	} else if (strcmp(serverMan->pendingCommand, kServerCmdAdminResetUnit) == 0) {
		dataMan->stateController()->changeState(STATE_INIT);
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetVersion) == 0) {
		/* TODO */
		returnVal = kServerReturnNotImplemented;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetFlag) == 0) {
		/* TODO */
		returnVal = kServerReturnNotImplemented;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetValue) == 0) {
		/* TODO */
		returnVal = kServerReturnNotImplemented;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetNames) == 0) {
		/* TODO */
		returnVal = kServerReturnNotImplemented;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetStoryInfo) == 0) {
		/* TODO */
		returnVal = kServerReturnNotImplemented;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetCurrentStory) == 0) {
		/* TODO */
		returnVal = kServerReturnNotImplemented;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetAllStoryInfo) == 0) {
		/* TODO */
		returnVal = kServerReturnNotImplemented;
	} else if (strcmp(serverMan->pendingCommand, kServerCmdGetStorageReport) == 0) {
		/* TODO */
		returnVal = kServerReturnNotImplemented;
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
		//Spark.publish(kServerCmdGetState, dataMan->stateController()->stateString(), kServerTTLDefault, PRIVATE);
		//returnVal = kServerReturnEventIncoming;
		returnVal = dataMan->stateController()->getState();
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
#if HAS_SD == 1
		if (dataMan->metadata.flags.sdCard) {
			WARN("Adding stories in SD mode not supported.");
			returnVal = kServerReturnFail;
		}
#endif
		// We only want to deal with one server connection at a time.
		if (serverMan->pendingAction) {
			returnVal =  kServerReturnBusy;
		} else if (dataMan->metadata.storyCount >= kMaxRandStoryCount) {
			returnVal =  kServerReturnMaxReached;
		} else {
			serverMan->pendingAction = true;
			returnVal = kServerReturnConnecting;
		}
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
}

bool ServerManager::getStoryData(TCPClient *aClient, uint32_t aStorySize) {
	if (aClient->connected()) {
		aClient->write("READY");
		int startTimeMs = millis();

		// Wait for data availability (with timeout)
		while (!aClient->available() && ((millis() - startTimeMs) < kServerTimeout)) {}

		if (aClient->available()) {
			//uint16_t index = 0;
			uint16_t chunkBytesRead = 0;
			uint32_t totalBytesRead = 0;
			uint32_t bytesToRead = kServerDataBufferSize;
			uint8_t chunksWritten = 0;
			uint8_t buffer[kStoryDataBufferSize];
			memset(&buffer[0], 0, sizeof(buffer));
			//DEBUG("Story Size: %lu", aStorySize);

			while (aClient->available()) {
				// Keep the Spark loop running!
				Spark.process();
				// While data available, read data into buffer, then flash space based on page size
				//memset(&buffer[0], 0, sizeof(buffer));
				if ((aStorySize - totalBytesRead) < kServerDataBufferSize) {
					bytesToRead = aStorySize - totalBytesRead;
				}

				int16_t bytes = aClient->read(buffer + chunkBytesRead, bytesToRead);
				//DEBUG("Bytes read: %d", bytes);

				if (bytes > 0) {
					chunkBytesRead += bytes;
					totalBytesRead += bytes;

					if ((chunkBytesRead % kStoryDataBufferSize == 0) ||
						(totalBytesRead == aStorySize)) {
						// Write data
						//DEBUG("Writing page #: %d", (pagesWritten + 1));
						//DEBUG("Used Pages: %d", Manager::getInstance().dataManager->metadata.usedStoryPages);
						bool result = Manager::getInstance().dataManager->writeData(buffer,
						                       (Manager::getInstance().dataManager->metadata.usedStoryPages * Flashee::Devices::userFlash().pageSize()) +
						                       (chunksWritten * kStoryDataBufferSize), sizeof(buffer));

						if (!result) {
							Errors::setError(E_SERVER_SOCKET_DATA_FAIL);
							//ERROR(Errors::errorString());
							aClient->write(Errors::errorString());
							break;
						}
						memset(&buffer[0], 0, sizeof(buffer));
						chunkBytesRead = 0;
						chunksWritten += 1;
					}
					//DEBUG("Pages: %d, Total bytes: %lu", pagesWritten, totalBytesRead);
					if (totalBytesRead == aStorySize) {
						//DEBUG("Data Received");
						aClient->write("COMPLETE");
						if (aClient->available()) {
							Errors::setError(E_SERVER_SOCKET_DATA_FAIL);
							//ERROR(Errors::errorString());
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
		}/* else {
			DEBUG("Timed Out");
		}*/
	} else {
		Errors::setError(E_SERVER_CONNECTION_FAIL);
		//ERROR(Errors::errorString());
	}
	Errors::setError(E_SERVER_SOCKET_DATA_FAIL);
	//ERROR(Errors::errorString());
	aClient->write(Errors::errorString());
	return false;
}

}