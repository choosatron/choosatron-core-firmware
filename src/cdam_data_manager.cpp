#include "cdam_data_manager.h"
#include "cdam_constants.h"
#include "cdam_state_controller.h"
#include "spark_wiring_time.h"
#include "Ymodem.h"

namespace cdam
{

/* Public Methods */

DataManager::DataManager() {}

bool DataManager::initialize(StateController *aStateController) {
	_stateController = aStateController;
	this->logPrint = false;
	this->runState = true;
	this->hasCredentials = false;
	this->metadata = {'\0'};

	// Initialize story specific variables.
	this->randomPlay = false;
	this->currentStory = -1;
	this->psgCount = 0;
	this->psgSize = 0;
	this->tocOffset = 0;
	this->startOffset = 0;

	this->writeToFlashee = false;
	this->flashTarget = 0;
	this->writeIndex = 0;
	_writeInProgress = false;
	_currentAddress = 0;
	_binarySize = 0;
	_variables = NULL;

	//DEBUG("Page Size: %d", Flashee::Devices::userFlash().pageSize());
	//DEBUG("Page Count: %d", Flashee::Devices::userFlash().pageCount());
	_metaFlash = Flashee::Devices::createAddressErase(0,
	                               4 * Flashee::Devices::userFlash().pageSize());

	if (_metaFlash == NULL) {
		//ERROR("Meta Flash is NULL!");
		return false;
	}

	loadFirmwareVersion();

	if (!loadMetadata()) {
		return false;
	}

	return true;
}

bool DataManager::initStorage() {
#if HAS_SD == 1
	if (this->metadata.flags.sdCard && !initSD()) {
		ERROR("SD failed!");
		this->metadata.flags.sdCard = 0;
	}
#endif

#if HAS_SD == 1
	if (!this->metadata.flags.sdCard) {
#endif
		//DEBUG("Stories: %d, Used Pages: %d", this->metadata.storyCount, this->metadata.usedStoryPages);
		this->liveStoryCount = (this->metadata.storyCount > 10) ? 10 : this->metadata.storyCount;
		_storyFlash = Flashee::Devices::createWearLevelErase(
		                                128 * Flashee::Devices::userFlash().pageSize(),
		                                384 * Flashee::Devices::userFlash().pageSize());
		if (_storyFlash == NULL) {
			//ERROR("Story Flash is NULL!");
			return false;
		}
#if HAS_SD == 1
	}
#endif
	// Setup story order.
	memcpy(&this->liveStoryOrder, &this->metadata.storyOrder, sizeof(this->metadata.storyOrder));
	return true;
}

void DataManager::logMetadata() {
	//DEBUG("Metadata size: %d", kMetadataSize);
	//DEBUG("Metadata size: %d", sizeof(this->metadata));
	//DEBUG(CDAM_SUBTITLE);
	/*char data[kMetadataSize] = "";
	_metaFlash->read(data, 0, kMetadataSize);
	for (int i = 0; i < kMetadataSize; ++i) {
		if (data[i]<0x10) {Serial.print("0");}
			Serial.print(data[i],HEX);
			//Serial.print(" ");
	}
	Serial.println("");*/
}

void DataManager::handleSerialData() {
	if (!SPARK_FLASH_UPDATE && (_serialElapsed >= kIntervalSerialMillis)) {
		if (Serial.available()) {
			this->writeToFlashee = false;
			uint8_t cmd = Serial.read();

			if (cmd == 'c') {
				//LOG("Command Mode");
				Serial.println("Command Mode");
				_serialTimeout = 0;
				while (!Serial.available()) {
					if (_serialTimeout >= kTimeoutSerialMillis) {
						Serial.println("Timeout");
						return;
					}
				}
				cmd = Serial.read();

				switch (cmd)
				{
					case kSerialCmdListeningMode: {
						this->hasCredentials = WiFi.hasCredentials();
						WiFi.listen();
						if (this->hasCredentials != WiFi.hasCredentials()) {
							this->hasCredentials = WiFi.hasCredentials();
							this->metadata.flags.offline = this->hasCredentials ? 0 : 1;
						}
						break;
					}
					case kSerialCmdClearWiFi: {
						Serial.println("Clearing WiFi creds.");
						WiFi.disconnect();
						WiFi.clearCredentials();
						WiFi.off();
						break;
					}
					case kSerialCmdWriteFlashee: {
						this->writeToFlashee = true;
					case kSerialCmdWriteFlashRaw:
						//LOG("Write to User Flash - press '1' for default address.");
						Serial.println("Write to User Flash - Press '1' for default address.");
						// First byte determines default address or not - ascii '0' for no, '1' for address.
						_serialTimeout = 0;
						while (!Serial.available()) {
							if (_serialTimeout >= kTimeoutSerialMillis) {
								Serial.println("Timeout");
								return;
							}
						}
						uint8_t useDefault = Serial.read();
						uint32_t address = 0x80000; // Default for flash.
						if (useDefault == '1') {
							if (this->writeToFlashee) {
								address = 0;
							}
						} else {
							// An address is included to write to.
							address = (Serial.read() << 24) | (Serial.read() << 16) | (Serial.read() << 8) | Serial.read();
						}
						//Save User File sent via Ymodem tool to any address(preferrably multiple of 0x20000) in External Flash
						//echo -n u > $DEV && sz -b -v --ymodem user.file > $DEV < $DEV
						bool status = System.serialSaveFile(&Serial, address); // Can also use &Serial1, &Serial2
						if (status) {
							System.reset();
						}
						break;
					}
					case kSerialCmdKeypadInput: {

						break;
					}
					case kSerialCmdButtonInput: {

						break;
					}
					case kSerialCmdAdjustCredits: {

						break;
					}
					case kSerialCmdSetCredits: {

						break;
					}
					case kSerialCmdAddStory: {
						if ((this->metadata.storyCount + this->metadata.deletedStoryCount) >= 20) {
							return;
						}
						this->writeToFlashee = true;
						this->flashTarget = kFlashStoriesType;
						uint32_t storySize = (Serial.read() << 24) | (Serial.read() << 16) | (Serial.read() << 8) | Serial.read();
						uint8_t pages = storySize / Flashee::Devices::userFlash().pageSize();
						if (storySize % Flashee::Devices::userFlash().pageSize()) {
							pages++;
						}
						uint32_t address = this->metadata.usedStoryPages * Flashee::Devices::userFlash().pageSize();
						uint8_t storyIndex = Serial.read();

						bool status = System.serialSaveFile(&Serial, address);
						if (status) {
							if (addStoryMetadata(storyIndex, pages)) {
								if (stateController()->getState() == STATE_SELECT) {
									stateController()->changeState(STATE_INIT);
								}
							}
						}
						break;
					}
					case kSerialCmdRemoveStory: {
						int8_t index = Serial.read();
						if (removeStoryMetadata(index)) {
							if ((stateController()->getState() == STATE_SELECT) ||
								(this->currentStory == index)) {
								stateController()->changeState(STATE_INIT);
							}
							Serial.write(0x01);
						} else {
							Serial.write(0x02);
						}
						break;
					}
					case kSerialCmdRemoveAllStories: {
						removeAllStoryData();
						stateController()->changeState(STATE_INIT);
						break;
					}
					case kSerialCmdMoveStory: {
						/* TODO */
						break;
					}
					case kSerialCmdSetFlag: {
						uint8_t flagIndex = Serial.read();
						uint8_t bitIndex = Serial.read();
						bool value = Serial.read();
						setFlag(flagIndex, bitIndex, value);
						break;
					}
					case kSerialCmdSetValue: {
						uint8_t index = Serial.read();
						uint16_t value = (Serial.read() << 8) | Serial.read();
						setValue(index, value);
						break;
					}
					case kSerialCmdResetMetadata: {
						resetMetadata();
						stateController()->changeState(STATE_INIT);
						break;
					}
					case kSerialCmdEraseFlash: {
						eraseFlash();
						stateController()->changeState(STATE_INIT);
						break;
					}
					case kSerialCmdRebootUnit: {
						stateController()->changeState(STATE_INIT);
						break;
					}
					case kSerialCmdDfuMode: {
						FLASH_OTA_Update_SysFlag = 0x0000;
						Save_SystemFlags();
						BKP_WriteBackupRegister(BKP_DR10, 0x0000);
						USB_Cable_Config(DISABLE);
						NVIC_SystemReset();
						break;
					}
					case kSerialCmdGetFlag: {
						uint8_t index = Serial.read();
						char* flags = (char*)&this->metadata.flags;
						Serial.write(flags[index]);
						break;
					}
					case kSerialCmdGetValue: {
						uint8_t index = Serial.read();
						uint8_t* values = (uint8_t*)&this->metadata.values;
						Serial.write(values[index * 2]);
						Serial.write(values[(index * 2) + 1]);
						break;
					}
				}
			}
		}
	}
}

#if HAS_SD == 1
bool DataManager::initSD() {
	_card = new Sd2Card();
	_volume = new SdVolume();
	_root = new SdFile();
	if (_card->init(PIN_SD_MOSI, PIN_SD_MISO, PIN_SD_SCK, PIN_SD_CS) &&//_card.init(SPI_FULL_SPEED, PIN_SD_CS) &&
		_volume->init(_card) &&
		_root->openRoot(_volume)) {

		_storyFile = new SdFile();

		dir_t filePntr;
		char fileName[13] = {'\0'};
		uint16_t fileIndex = 0;
		uint8_t index = 0;
		int8_t dirBytes = 0;
		while ((dirBytes = _root->readDir(&filePntr)) && (index < kMaxRandStoryCount)) {
			memcpy(fileName, filePntr.name, 8);
			memcpy(fileName + 9, filePntr.name + 8, 3);
			fileName[8] = '.';
			if (strncmp(fileName + 9, "DAM", 3) == 0) {
				if (_storyFile->open(_root, fileIndex, O_READ)) {
					if (_storyFile->read() == kAsciiHeaderByte) {
						this->metadata.storyOffsets[index] = fileIndex;
						this->metadata.storyOrder[index] = index;
						index++;
					}
					_storyFile->close();
				}
			}
			fileIndex++;
		}
		if (index > 0) {
			this->metadata.storyCount = index;
			this->liveStoryCount = (index > 10) ? 10 : index;
			return true;
		}
	}
	delete _card;
	delete _volume;
	delete _root;
	return false;
}
#endif

// Getters
Flashee::FlashDevice* DataManager::flashMetadata() {
	return _metaFlash;
}

Flashee::FlashDevice* DataManager::flashStories() {
	return _storyFlash;
}
//

uint32_t DataManager::getStoryOffset(uint8_t aIndex, bool aVisibleOnly) {
#if HAS_SD == 1
	if (this->metadata.flags.sdCard) { return 0; }
#endif
	if (aVisibleOnly) {
		aIndex = this->liveStoryOrder[aIndex];
	}
	return this->metadata.storyOffsets[aIndex] * Flashee::Devices::userFlash().pageSize();
}

uint32_t DataManager::getPassageOffset(uint16_t aIndex) {
	uint32_t offset = 0;
	if (this->currentStory != -1) {
		readData(&offset, this->tocOffset + (kPassageOffsetSize * aIndex), kPassageOffsetSize);
	}
	return offset;
}

bool DataManager::getNumberedTitle(char* aBuffer, uint8_t aIndex) {
	sprintf(aBuffer, "%d. ", aIndex + 1);
	uint32_t offset = kStoryTitleOffset;
#if HAS_SD == 1
	if (this->metadata.flags.sdCard) {
		if (!_storyFile->open(_root, this->metadata.storyOffsets[this->liveStoryOrder[aIndex]], O_READ)) {
			return false;
		}
	} else
#endif
	{
		offset += getStoryOffset(aIndex);
	}
	bool result = readData(aBuffer + strlen(aBuffer), offset, kStoryTitleSize);
#if HAS_SD == 1
	if (this->metadata.flags.sdCard) {
		if (_storyFile->isOpen()) {
			_storyFile->close();
		}
	}
#endif
	return result;
}

bool DataManager::loadStory(uint8_t aIndex) {
	this->currentStory = aIndex;

#if HAS_SD == 1
	if (this->metadata.flags.sdCard) {
		if (!_storyFile->open(_root, this->metadata.storyOffsets[this->liveStoryOrder[aIndex]], O_READ)) {
			return false;
		}
	}
#endif

	if (!readStoryHeader(this->currentStory, &this->storyHeader)) {
		return false;
	}
	return true;
}

void DataManager::unloadStory() {
#if HAS_SD == 1
	if (this->metadata.flags.sdCard) {
		_storyFile->close();
	}
#endif
	this->randomPlay = false;
	this->currentStory = -1;
	this->points = 0;
	this->psgCount = 0;
	this->tocOffset = 0;
	this->startOffset = 0;
	this->psgIndex = 0;
	this->psgSize = 0;

	if ((_variables != NULL) && (this->storyHeader.variableCount > 0)) {
		delete[] _variables;
		_variables = NULL;
	}
	this->storyHeader = {'\0'};
}

int16_t DataManager::varAtIndex(uint8_t aIndex) {
	return _variables[aIndex];
}

bool DataManager::setVarAtIndex(uint8_t aIndex, int16_t aValue) {
	if (aIndex < this->storyHeader.variableCount) {
		_variables[aIndex] = aValue;
		return true;
	}
	Errors::setError(E_INVALID_VARIABLE);
	return false;
}

/* API Calls */

void DataManager::getStoryInfo(char* aBuffer, uint32_t aLength, uint8_t aIndex, StoryHeader *aHeader) {
	uint8_t pages = aHeader->storySize / Flashee::Devices::userFlash().pageSize();
	if (aHeader->storySize % Flashee::Devices::userFlash().pageSize()) {
		pages++;
	}
	snprintf(aBuffer, aLength, "%s:%s:%s:%d.%d.%d:%d:%d",
	         aHeader->uuid, aHeader->title,
	         aHeader->author, aHeader->storyVer.major,
	         aHeader->storyVer.minor, aHeader->storyVer.revision,
	         this->metadata.storyState[aIndex], pages);
}

bool DataManager::addStoryMetadata(uint8_t aIndex, uint8_t aPages) {
	// Get the current story count, see if adjustment is needed.
	int8_t count = this->metadata.storyCount;
	uint8_t totalCount = this->metadata.storyCount + this->metadata.deletedStoryCount;

	if (totalCount >= 20) {
		return false;
	}

	// If the position requested is past the existing, just set it to the next.
	if (aIndex >= count) {
		aIndex = count;
	} else {
		// Shift existing story indexes 'up', until the slot for the new story is free.
		while (count > aIndex) {
			this->metadata.storyOrder[count] = this->metadata.storyOrder[count - 1];
			count--;
			// Need to update the current story if there is one.
			if (count == this->currentStory) {
				this->currentStory++;
			}
		}
	}

	// Set the state of this story.
	this->metadata.storyState[totalCount] = kStoryStateNormal;
	// Set the offset for this story.
	this->metadata.storyOffsets[totalCount] = this->metadata.usedStoryPages;
	// Set the position for this story.
	this->metadata.storyOrder[aIndex] = totalCount;
	// Setup story order.
	memcpy(&this->liveStoryOrder, &this->metadata.storyOrder, sizeof(this->metadata.storyOrder));
	// Increase the story count.
	this->metadata.storyCount++;
	if (this->liveStoryCount < 10) {
		this->liveStoryCount++;
	}
	// Add the total story bytes to the total used.
	this->metadata.usedStoryPages += aPages;

	return writeStoryCountData(&this->metadata);
}

bool DataManager::removeStoryMetadata(uint8_t aIndex) {
	if (aIndex < this->metadata.storyCount) {
		uint8_t totalCount = this->metadata.storyCount + this->metadata.deletedStoryCount;
		uint8_t trueIndex = this->metadata.storyOrder[aIndex];
		if (this->metadata.storyCount == 1) {
			return removeAllStoryData();
		}
		if (aIndex < this->metadata.storyCount) {
			uint8_t count = this->metadata.storyCount;
			while (count > aIndex) {
				this->metadata.storyOrder[aIndex] = this->metadata.storyOrder[aIndex + 1];
				aIndex++;
				// Need to update the current story if there is one.
				if (aIndex == this->currentStory) {
					this->currentStory--;
				}
			}
			// Final increment puts aIndex out of bounds for the story count.
			this->metadata.storyOrder[aIndex - 1] = 0;
		}

		// Mark the story index as deleted.
		this->metadata.storyState[trueIndex] = kStoryStateDeleted;
		this->metadata.deletedStoryCount++;

		// If we happen to be deleting the last story, we can totally remove it.
		if (trueIndex == (totalCount - 1)) {
			// Calculate and subtract the size of the last story.
			uint8_t i = trueIndex;
			while (this->metadata.storyState[i] == kStoryStateDeleted) {
				this->metadata.usedStoryPages = this->metadata.storyOffsets[i];
				this->metadata.storyOffsets[i] = 0;
				this->metadata.storyState[i] = kStoryStateEmpty;
				this->metadata.deletedStoryCount--;
				i--;
			}
		}
		if (this->metadata.storyCount <= 10) {
			this->liveStoryCount--;
		}
		this->metadata.storyCount--;

		// Setup story order.
		memcpy(&this->liveStoryOrder, &this->metadata.storyOrder, sizeof(this->metadata.storyOrder));
		return writeStoryCountData(&this->metadata);
	}
	return false;
}

bool DataManager::removeAllStoryData() {
	// Reset the story count.
	this->metadata.storyCount = 0;
	this->metadata.deletedStoryCount = 0;
	this->liveStoryCount = 0;
	// Add the total story bytes to the total used.
	this->metadata.usedStoryPages = 0;

	for (int i = 0; i < kMaxRandStoryCount; ++i) {
		this->metadata.storyOffsets[i] = 0;
		this->metadata.storyOrder[i] = 0;
		this->metadata.storyState[i] = kStoryStateEmpty;
	}

	return writeStoryCountData(&this->metadata);
}

bool DataManager::setFlag(uint8_t aFlagIndex, uint8_t aBitIndex, bool aValue) {
	if (aValue) {
		// Access the flag byte at the appropriate offset and set it.
		(*((uint8_t *) &this->metadata + kMetadataFlagsOffset + aFlagIndex)) |= 1 << aBitIndex;
	} else {
		// Access the flag byte at the appropriate offset and clear it.
		(*((uint8_t *) &this->metadata + kMetadataFlagsOffset + aFlagIndex)) &= ~(1 << aBitIndex);
	}
	uint8_t flag = (*((uint8_t *)&this->metadata + kMetadataFlagsOffset + aFlagIndex));
	bool result = _metaFlash->writeEraseByte(flag, kMetadataFlagsOffset + aFlagIndex);
	return result;
}

bool DataManager::setValue(uint8_t aIndex, uint16_t aValue) {
	//uint16_t values = (*((uint16_t *)&this->metadata.values + (aIndex * 2)));
	uint16_t* values = (uint16_t*)&this->metadata.values;
	values[aIndex] = aValue;
	bool result = _metaFlash->write(&values[aIndex], kMetadataValuesOffset + (aIndex * 2), 2);
	return result;
}

bool DataManager::resetMetadata() {
	return initializeMetadata(&this->metadata);
}

bool DataManager::eraseFlash() {
	return Flashee::Devices::userFlash().eraseAll();
}

uint8_t DataManager::readByte(uint32_t aAddress) {
	uint8_t byte = 0;
#if HAS_SD == 1
	if (this->metadata.flags.sdCard) {
		if (_storyFile->seekSet(aAddress)) {
			byte = _storyFile->read();
		}
	} else
#endif
	{
		byte = _storyFlash->readByte(aAddress);
	}
	return byte;
}

bool DataManager::readData(void* aBuffer, uint32_t aAddress, uint32_t aLength) {
#if HAS_SD == 1
	if (this->metadata.flags.sdCard) {
		if (_storyFile->seekSet(aAddress)) {
			_storyFile->read(aBuffer, aLength);
		}
		return true;
	} else
#endif
	{
		return _storyFlash->read(aBuffer, aAddress, aLength);
	}
	return false;
}

void DataManager::writeBegin(uint32_t aAddress, uint32_t aBinarySize) {
	_writeInProgress = true;
	_currentAddress = aAddress;
	_binarySize = aBinarySize;
	//Serial1.println(aBinarySize);
}

void DataManager::writeEnd(bool aResult) {
	// if aResult == true, success!
	_writeInProgress = false;
	_currentAddress = 0;
	_binarySize = 0;
	this->writeToFlashee = false;
	this->writeIndex = 0;
	this->flashTarget = 0;
}

uint16_t DataManager::writeData(void* aBuffer, uint32_t aLength) {
	_storyFlash->write(aBuffer, _currentAddress, aLength);
	_currentAddress += aLength;
	// TODO: Read back data and compare with data just written
	// If it is the same, increment writeIndex.
	// Otherwise, erase sector and don't increment.

	if (1) { // Successfully verified written data.
		this->writeIndex += 1;
	}
	return this->writeIndex;
}

bool DataManager::writeData(void* aBuffer, uint32_t aAddress, uint32_t aLength) {
#if HAS_SD == 1
	if (this->metadata.flags.sdCard) {
		/* Currently not writing. */
		WARN("No writing data in SD mode.");
	} else
#endif
	{
		return _storyFlash->write(aBuffer, aAddress, aLength);
	}
	return false;
}

/* Accessors */

StateController* DataManager::stateController() {
	return _stateController;
}

/* Private Methods */

void DataManager::loadFirmwareVersion() {
	// Set firmware version.
	this->firmwareVersion.major = kFirmwareVersionMajor;
	this->firmwareVersion.minor = kFirmwareVersionMinor;
	this->firmwareVersion.revision = kFirmwareVersionRevision;
}

bool DataManager::loadMetadata() {
	// Load and set metadata.
	LOG("Firmware v%d.%d.%d", this->firmwareVersion.major,
		this->firmwareVersion.minor, this->firmwareVersion.revision);

	// Check for SOH
	if (_metaFlash->readByte(kMetadataBaseAddress) != kAsciiHeaderByte) {
		//DEBUG("No SOH, write fresh metadata.");
		if (!initializeMetadata(&this->metadata)) {
			//ERROR(Errors::errorString());
			return false;
		}
	} else {
		// Data exists. Read it!
		if (!readMetadata(&this->metadata)) {
			return false;
		}

		if (didFirmwareUpdate(&this->metadata)) {
			// Update to the current standard (in memory)
			if (!upgradeDataModels()) {
				Errors::setError(E_DATA_MODEL_UPGRADE_FAIL);
				return false;
			}
		}
	}
	return true;
}

bool DataManager::initializeMetadata(Metadata *aMetadata) {
	// Set the default values for a fresh Choosatron.
	*aMetadata = {'\0'};

	aMetadata->soh = kAsciiHeaderByte;
	aMetadata->firmwareVer.major = this->firmwareVersion.major;
	aMetadata->firmwareVer.minor = this->firmwareVersion.minor;
	aMetadata->firmwareVer.revision = this->firmwareVersion.revision;

	aMetadata->flags.auth = 0;
	aMetadata->flags.offline = 0;
	aMetadata->flags.demo = 0;
	aMetadata->flags.sdCard = 0;
	aMetadata->flags.multiplayer = 0;
	aMetadata->flags.arcade = 0;
	aMetadata->flags.continues = 0;
	aMetadata->flags.random = 1;

	//aMetadata->flags.rsvd2 = 0;
	aMetadata->flags.logging = 0;
	aMetadata->flags.logLocal = 0;
	//aMetadata->flags.logLive = 0;
	//aMetadata->flags.logPrint = 0;

	//aMetadata->flags.rsvd3 = 0;
	//aMetadata->flags.dictOffsetBytes = 0;

	aMetadata->values.pcbVersion = BOARD_DOT_VERSION;
	aMetadata->values.timezoneOffset = 0;
	aMetadata->values.coinsPerCredit = 0;
	aMetadata->values.coinDenomination = 0;

	aMetadata->storyCount = 0;
	aMetadata->deletedStoryCount = 0;
	aMetadata->usedStoryPages = 0;

	strncpy(aMetadata->deviceName, CDAM_TITLE, kMetadataDeviceNameSize);

	if (!writeMetadata(aMetadata)) {
		return false;
	}
	return true;
}

bool DataManager::readMetadata(Metadata *aMetadata) {
	bool result = false;
	if (_metaFlash->readByte(kMetadataBaseAddress) == kAsciiHeaderByte) {
		result = _metaFlash->read(aMetadata, kMetadataBaseAddress, kMetadataSize);
	}
	return result;
}

bool DataManager::readStoryHeader(uint8_t aIndex, StoryHeader *aHeader) {
	uint32_t offset = getStoryOffset(aIndex);

	bool result = readData((uint8_t*)aHeader, offset, kStoryHeaderSize);
	_variables = new int16_t[aHeader->variableCount]();
	offset += kStoryHeaderSize;

	result = readData(&this->psgCount, offset, kPassageCountSize) && result ? true : false;
	offset += kPassageCountSize;

	// Memory offset for where the passage memory offsets begin.
	this->tocOffset = offset;

	// Grab the offset of the second passage, which gives us the size of the first.
	result = readData(&this->psgSize, offset + kPassageOffsetSize, kPassageOffsetSize) && result ? true : false;

	offset += this->psgCount * kPassageOffsetSize;
	this->startOffset = offset;
	this->psgIndex = 0;

	return result;
}

bool DataManager::writeMetadata(Metadata *aMetadata) {
	bool result = _metaFlash->write(aMetadata, kMetadataBaseAddress, kMetadataSize);
	return result;
}

bool DataManager::writeStoryCountData(Metadata *aMetadata) {
	/*bool result = _metaFlash->write(&aMetadata->storyCount,
		                 kMetadataStoryCountOffset,
		                 kMetadataStoryCountSize + kMetadataStoryUsedPagesSize +
		                 kMetadataStoryOffsetsSize + kMetadataStoryOrderSize);*/
	bool result = true;
	result = _metaFlash->write(&aMetadata->storyCount, kMetadataStoryCountOffset, kMetadataStoryCountSize) && result ? true : false;
	result = _metaFlash->write(&aMetadata->deletedStoryCount, kMetadataDeletedStoryCountOffset, kMetadataDeletedStoryCountSize) && result ? true : false;
	result = _metaFlash->write(&aMetadata->usedStoryPages, kMetadataStoryUsedPagesOffset, kMetadataStoryUsedPagesSize) && result ? true : false;
	result = _metaFlash->write(&aMetadata->storyOffsets, kMetadataStoryOffsetsOffset, kMetadataStoryOffsetsSize) && result ? true : false;
	result = _metaFlash->write(&aMetadata->storyOrder, kMetadataStoryOrderOffset, kMetadataStoryOrderSize) && result ? true : false;
	result = _metaFlash->write(&aMetadata->storyState, kMetadataStoryStateOffset, kMetadataStoryStateSize) && result ? true : false;

	return result;
}

bool DataManager::didFirmwareUpdate(Metadata *aMetadata) {
	// Check if saved firmware version is out of date
	if (aMetadata->firmwareVer.major != this->firmwareVersion.major ||
		aMetadata->firmwareVer.minor != this->firmwareVersion.minor ||
		aMetadata->firmwareVer.revision != this->firmwareVersion.revision) {
		// First run after firmware update
		/*DEBUG("Firmware has been upgraded from v%d.%d.%d to v%d.%d.%d",
		    aMetadata->firmwareVer.major, aMetadata->firmwareVer.minor,
		    aMetadata->firmwareVer.revision, this->firmwareVersion.major,
		    this->firmwareVersion.minor, this->firmwareVersion.revision);*/
		return true;
	}
	return false;
}

bool DataManager::upgradeDataModels() {
	// Update the data models.


	// Wipe out data in flash
	// ** Do we need to 'zero' it out? **
	// Save the new metadata.
	bool result = writeMetadata(&this->metadata);
	return result;
}

/*void DataManager::testMetadata() {
	Metadata metadata = {};
	_metaFlash->eraseAll();
	readMetadata(&metadata);
	DEBUG("*** Should be empty ***");
	logMetadata(&metadata);
	setTestMetadata(&metadata);
	DEBUG("*** Should be filled in ***");
	logMetadata(&metadata);
	writeMetadata(&metadata);
	metadata = {};
	DEBUG("*** Should be empty ***");
	logMetadata(&metadata);
	readMetadata(&metadata);
	DEBUG("*** Should be AWESOME ***");
	logMetadata(&metadata);
}*/

/*void DataManager::setTestMetadata(Metadata *aMetadata) {
	*aMetadata = {};

	aMetadata->soh = kAsciiHeaderByte;

	aMetadata->firmwareVer.major = 9;
	aMetadata->firmwareVer.minor = 8;
	aMetadata->firmwareVer.revision = 76;

	aMetadata->flags.auth = 0;
	aMetadata->flags.offline = 1;
	aMetadata->flags.demo = 0;
	aMetadata->flags.sdCard = 1;
	aMetadata->flags.multiplayer = 0;
	aMetadata->flags.arcade = 1;
	aMetadata->flags.continues = 0;
	aMetadata->flags.random = 1;

	aMetadata->flags.rsvd2 = 0;
	aMetadata->flags.logging = 1;
	aMetadata->flags.logLocal = 0;
	aMetadata->flags.logLive = 1;
	aMetadata->flags.logPrint = 0;

	aMetadata->flags.rsvd3 = 0;
	aMetadata->flags.dictOffsetBytes = 1;

	aMetadata->flags.flag4 = 0x55;
	aMetadata->flags.flag5 = 0xAA;
	aMetadata->flags.flag6 = 0x55;
	aMetadata->flags.flag7 = 0xAA;
	aMetadata->flags.flag8 = 0x55;

	aMetadata->values.coinsPerCredit = 2;
	aMetadata->values.coinDenomination = 25;
	aMetadata->values.coinsToContinue = 1;
	aMetadata->values.value4 = 4;
	aMetadata->values.value5 = 5;
	aMetadata->values.value6 = 6;
	aMetadata->values.value7 = 7;
	aMetadata->values.value8 = 8;
	aMetadata->values.value9 = 9;
	aMetadata->values.value10 = 10;
	aMetadata->values.value11 = 11;
	aMetadata->values.value12 = 12;
	aMetadata->values.value13 = 13;
	aMetadata->values.value14 = 14;
	aMetadata->values.value15 = 15;
	aMetadata->values.value16 = 16;

	aMetadata->storyCount = 4;
	aMetadata->usedStoryPages = 128;
	aMetadata->storyOffsets[0] = 0;
	aMetadata->storyOffsets[1] = 10;
	aMetadata->storyOffsets[2] = 100;
	aMetadata->storyOffsets[3] = 255;
}*/

/*void DataManager::logBinary(uint8_t aValue) {
	DEBUG("0x%c%c%c%c%c%c%c%c",
	    (IsBitSet(aValue, 7) ? '1' : '0'),
	    (IsBitSet(aValue, 6) ? '1' : '0'),
	    (IsBitSet(aValue, 5) ? '1' : '0'),
	    (IsBitSet(aValue, 4) ? '1' : '0'),
	    (IsBitSet(aValue, 3) ? '1' : '0'),
	    (IsBitSet(aValue, 2) ? '1' : '0'),
	    (IsBitSet(aValue, 1) ? '1' : '0'),
	    (IsBitSet(aValue, 0) ? '1' : '0'));
}*/

/*void DataManager::logStoryOffsets(Metadata *aMetadata) {
	for (int i = 0; i < aMetadata->storyCount; ++i) {
		DEBUG("Story #: %d, Offset: %lu", i, aMetadata->storyOffsets[i]);
	}
}*/

/*void DataManager::logStoryBytes(Metadata *aMetadata) {
	for (int i = 0; i < aMetadata->storyCount; ++i) {
		DEBUG("Story #: %d, Offset: %d", i, aMetadata->storyOffsets[i]);
		if (loadStory(i)) {
			logStoryHeader(&this->storyHeader);
		}
		char data[kStoryHeaderSize] = "";
		uint32_t offset = getStoryOffset(i) * Flashee::Devices::userFlash().pageSize();
		readData(data, offset, kStoryHeaderSize);
		DEBUG("HEADER DEBUG BEGIN");
		DEBUG("HEX");
		for (int i = 0; i < kStoryHeaderSize; ++i) {
			if (data[i]<0x10) {Serial.print("0");}
	          Serial.print(data[i],HEX);
	          Serial.print(" ");
		}
		Serial.println("END");
	}
}*/

/*void DataManager::logMetadata(Metadata *aMetadata) {
	DEBUG("METADATA");
	DEBUG("Firmware v%d.%d.%d", aMetadata->firmwareVer.major,
		aMetadata->firmwareVer.minor, aMetadata->firmwareVer.revision);
	DEBUG("Offline: %s", (aMetadata->flags.offline ? "on" : "off"));
	DEBUG("Demo: %s", (aMetadata->flags.demo ? "on" : "off"));
	DEBUG("SD: %s", (aMetadata->flags.sdCard ? "on" : "off"));
	DEBUG("Multi: %s", (aMetadata->flags.multiplayer ? "on" : "off"));
	DEBUG("Arcade: %s", (aMetadata->flags.arcade ? "on" : "off"));
	DEBUG("Continues: %s", (aMetadata->flags.continues ? "on" : "off"));
	DEBUG("Random: %s", (aMetadata->flags.random ? "on" : "off"));
	DEBUG("Auth: %s", (aMetadata->flags.auth ? "on" : "off"));
	DEBUG("Logging: %s", (aMetadata->flags.logging ? "on" : "off"));
	DEBUG("Log Local: %s", (aMetadata->flags.logLocal ? "on" : "off"));
	DEBUG("Log Live: %s", (aMetadata->flags.logLive ? "on" : "off"));
	DEBUG("Log Print: %s", (aMetadata->flags.logPrint ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag2, 3) ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag2, 2) ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag2, 1) ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag2, 0) ? "on" : "off"));
	logBinary(aMetadata->flags.flag3);
	logBinary(aMetadata->flags.flag4);
	logBinary(aMetadata->flags.flag5);
	logBinary(aMetadata->flags.flag6);
	logBinary(aMetadata->flags.flag7);
	logBinary(aMetadata->flags.flag8);
	DEBUG("Coins Per Credit: %d", aMetadata->values.coinsPerCredit);
	DEBUG("Coin Denom: %d", aMetadata->values.coinDenomination);
	DEBUG("Coins to Continue: %d", aMetadata->values.coinsToContinue);
	DEBUG("Value 4: %d", aMetadata->values.value4);
	DEBUG("Value 5: %d", aMetadata->values.value5);
	DEBUG("Value 6: %d", aMetadata->values.value6);
	DEBUG("Value 7: %d", aMetadata->values.value7);
	DEBUG("Value 8: %d", aMetadata->values.value8);
	DEBUG("Value 9: %d", aMetadata->values.value9);
	DEBUG("Value 10: %d", aMetadata->values.value10);
	DEBUG("Value 11: %d", aMetadata->values.value11);
	DEBUG("Value 12: %d", aMetadata->values.value12);
	DEBUG("Value 13: %d", aMetadata->values.value13);
	DEBUG("Value 14: %d", aMetadata->values.value14);
	DEBUG("Value 15: %d", aMetadata->values.value15);
	DEBUG("Value 16: %d", aMetadata->values.value16);
	DEBUG("Story Count: %d", aMetadata->storyCount);
	DEBUG("Used Story Bytes: %d", aMetadata->usedStoryPages);
	logStoryOffsets(aMetadata);
}*/

/*void DataManager::logStoryHeader(StoryHeader *aHeader) {
	DEBUG("STORY HEADER");
	DEBUG("Binary v%d.%d.%d", aHeader->binaryVer.major,
		aHeader->binaryVer.minor, aHeader->binaryVer.revision);
	DEBUG("Scripting: %s", (aHeader->flags.scripting ? "on" : "off"));
	DEBUG("Variables: %s", (aHeader->flags.variables ? "on" : "off"));
	DEBUG("Images: %s", (aHeader->flags.images ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aHeader->flags.flag1, 4) ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aHeader->flags.flag1, 3) ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aHeader->flags.flag1, 2) ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aHeader->flags.flag1, 1) ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aHeader->flags.flag1, 0) ? "on" : "off"));

	DEBUG("Continues: %s", (aHeader->flags.continues ? "on" : "off"));
	DEBUG("Multiplayer: %s", (aHeader->flags.multiplayer ? "on" : "off"));
	DEBUG("Hide Used Choices: %s", (aHeader->flags.hideUsed ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aHeader->flags.flag2, 4) ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aHeader->flags.flag2, 3) ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aHeader->flags.flag2, 2) ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aHeader->flags.flag2, 1) ? "on" : "off"));
	DEBUG("Rsvd: %s", (IsBitSet(aHeader->flags.flag2, 0) ? "on" : "off"));

	logBinary(aHeader->flags.flag3);
	logBinary(aHeader->flags.flag4);

	DEBUG("Story Size Bytes: %d", aHeader->storySize);

	DEBUG("Story v%d.%d.%d", aHeader->storyVer.major,
		aHeader->storyVer.minor, aHeader->storyVer.revision);

	DEBUG("Lang Code: %s", aHeader->languageCode);
	DEBUG("Title: %s", aHeader->title);
	DEBUG("Subtitle: %s", aHeader->subtitle);
	DEBUG("Author: %s", aHeader->author);
	DEBUG("Credits: %s", aHeader->credits);
	DEBUG("Contact: %s", aHeader->contact);

	char timeBuffer[26];
	Time.timeStr(aHeader->publishDate).toCharArray(timeBuffer, 26);
	DEBUG("Publish Datetime: %s", timeBuffer);
}*/

}