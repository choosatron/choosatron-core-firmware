#include "cdam_data_manager.h"
#include "cdam_constants.h"
#include "cdam_state_controller.h"

#include "spark_wiring_time.h"

namespace cdam
{

/* Public Methods */

DataManager::DataManager() {}

bool DataManager::initialize(StateController *aStateController) {
	_stateControl = aStateController;
	this->currentStory = -1;
	this->runState = true;
	this->metadata = {};
	DEBUG("Page Size: %d", Flashee::Devices::userFlash().pageSize());
	DEBUG("Page Count: %d", Flashee::Devices::userFlash().pageCount());
	_metaFlash = Flashee::Devices::createAddressErase(0,
	                               4 * Flashee::Devices::userFlash().pageSize());

	if (_metaFlash == NULL) {
		ERROR("Meta Flash is NULL!");
		return false;
	}
    //testMetadata();
	//_metaFlash->eraseAll();
	DEBUG("Sizeof: %d", sizeof(this->metadata));
	DEBUG("Metadata Size: %d", kMetadataSize);
	char data[kMetadataSize] = "";
	_metaFlash->read(data, 0, kMetadataSize);
	DEBUG("META DEBUG BEGIN");
	DEBUG("HEX");
	for (int i = 0; i < kMetadataSize; ++i) {
		if (data[i]<0x10) {Serial.print("0");}
          Serial.print(data[i],HEX);
          Serial.print(" ");
	}
	Serial.println("END");
	DEBUG("META DEBUG END");

	loadFirmwareVersion();

	if (!loadMetadata()) {
		return false;
	}

	DEBUG("Stories: %d, Used Pages: %d", this->metadata.storyCount, this->metadata.usedStoryPages);
	_storyFlash = Flashee::Devices::createWearLevelErase(
	                                128 * Flashee::Devices::userFlash().pageSize(),
	                                384 * Flashee::Devices::userFlash().pageSize());
	if (_storyFlash == NULL) {
		ERROR("Story Flash is NULL!");
		return false;
	}
	// Setup story order.
	memcpy(&this->liveStoryOrder, &this->metadata.storyOrder, sizeof(this->metadata.storyOrder));

	//_storyFlash->eraseAll();
	//logStoryOffsets(&this->metadata);
	logStoryBytes(&this->metadata);
	DEBUG("Ready!");

	return true;
}

uint32_t DataManager::getStoryOffset(uint8_t aIndex) {
	return this->metadata.storyOffsets[this->liveStoryOrder[aIndex]];
}

bool DataManager::getNumberedTitle(char* aBuffer, uint8_t aIndex) {
	memset(&aBuffer[0], 0, sizeof(aBuffer));
	sprintf(aBuffer, "%d. ", aIndex + 1);

	DEBUG("Size: %d", strlen(aBuffer));
	uint32_t offset = getStoryOffset(aIndex) * Flashee::Devices::userFlash().pageSize() + kStoryTitleOffset;
	bool result = _metaFlash->read(aBuffer + strlen(aBuffer), offset, kStoryTitleSize);
	if (!result) {
		Errors::setError(E_HEADER_READ_FAIL);
		ERROR(Errors::errorString());
	}
	return result;
}

bool DataManager::loadStoryHeader(uint8_t aIndex) {
	return readStoryHeader(&this->storyHeader, aIndex);
}

bool DataManager::addStoryMetadata(uint8_t aIndex, uint8_t aPages) {
	// Get the current story count, see if adjustement is needed.
	uint8_t count = this->metadata.storyCount;

	// If the position requested is past the existing, just set it to the next.
	if (aIndex >= count) {
		aIndex = count;
	} else {
		// Shift existing story indexes 'up', until the slot for the new story is free.
		while (count > aIndex) {
			this->metadata.storyOrder[count] = this->metadata.storyOrder[count - 1];
			count--;
		}
	}

	// Set the offset for this story.
	this->metadata.storyOffsets[this->metadata.storyCount] = this->metadata.usedStoryPages;
	// Set the position for this story.
	this->metadata.storyOrder[aIndex] = this->metadata.storyCount;
	// Increase the story count.
	this->metadata.storyCount += 1;
	// Add the total story bytes to the total used.
	this->metadata.usedStoryPages += aPages;

	return writeStoryCountData(&this->metadata);
}

/*bool DataManager::removeStoryMetadata(uint8_t aIndex) {
	// Get the current story count, see if adjustement is needed.
	uint8_t count = this->metadata.storyCount;

	// If the position requested is past the existing, just set it to the next.
	if (aIndex >= count) {
		aIndex = count;
	} else {
		// Shift existing story indexes 'up', until the slot for the new story is free.
		while (count > aIndex) {
			this->metadata.storyOffsets[count] = this->metadata.storyOffsets[count - 1];
			count--;
		}
	}

	// Decrease the story count.
	this->metadata.storyCount--;
	// Decrease used story bytes.
	this->metadata.usedStoryBytes -= this->storyHeaders[aIndex].storySize;

	return writeStoryCountData(&this->metadata);
}*/

bool DataManager::removeAllStoryData() {
	// Increase the story count.
	this->metadata.storyCount = 0;
	// Add the total story bytes to the total used.
	this->metadata.usedStoryPages = 0;
	// Clear out storyOffsets.
	//memset(&this->metadata.storyOffsets[0], 0, sizeof(this->metadata.storyOffsets));

	for (int i = 0; i < kMaxStoryCount; ++i) {
		this->metadata.storyOffsets[i] = 0;
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
	uint8_t flag = (*((uint8_t *) &this->metadata + kMetadataFlagsOffset + aFlagIndex));
	Errors::clearError();
	bool result = _metaFlash->writeEraseByte(flag, kMetadataFlagsOffset + aFlagIndex);
	if (!result) {
		Errors::setError(E_METADATA_WRITE_FAIL);
		ERROR(Errors::errorString());
	}
	return result;
}

bool DataManager::resetMetadata() {
	return initializeMetadata(&this->metadata);
}

bool DataManager::eraseFlash() {
	return Flashee::Devices::userFlash().eraseAll();
}

bool DataManager::readData(uint8_t* aBuffer, uint32_t aAddress, uint32_t aLength) {
	if (this->metadata.flags.sdCard) {

	} else {
		_storyFlash->read(aBuffer, aAddress, aLength);
	}
}

bool DataManager::writeData(uint8_t* aBuffer, uint32_t aAddress, uint32_t aLength) {
	if (this->metadata.flags.sdCard) {

	} else {
		_storyFlash->write(aBuffer, aAddress, aLength);
	}
}

/* Accessors */

StateController* DataManager::stateController() {
	return _stateControl;
}

Flashee::FlashDevice* DataManager::storyFlash() {
	return _storyFlash;
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
	if (_metaFlash->readByte(kMetadataBaseAddress) != ASCII_SOH) {
		DEBUG("No SOH, write fresh metadata.");
		if (!initializeMetadata(&this->metadata)) {
			ERROR(Errors::errorString());
			return false;
		}
	} else {
		// Data exists. Read it!
		DEBUG("SOH found, read metadata.");
		if (!readMetadata(&this->metadata)) {
			ERROR(Errors::errorString());
			return false;
		}

		if (didFirmwareUpdate(&this->metadata)) {
			// Update to the current standard (in memory)
			if (!upgradeDataModels()) {
				Errors::setError(E_DATA_MODEL_UPGRADE_FAIL);
				ERROR(Errors::errorString());
				return false;
			}
		}
	}
	return true;
}

bool DataManager::initializeMetadata(Metadata *aMetadata) {
	// Set the default values for a fresh Choosatron.
	*aMetadata = {};

	aMetadata->soh = ASCII_SOH;
	aMetadata->firmwareVer.major = this->firmwareVersion.major;
	aMetadata->firmwareVer.minor = this->firmwareVersion.minor;
	aMetadata->firmwareVer.revision = this->firmwareVersion.revision;

	aMetadata->flags.rsvd1 = 0;
	aMetadata->flags.offline = 0;
	aMetadata->flags.demo = 0;
	aMetadata->flags.sdCard = 0;
	aMetadata->flags.multiplayer = 0;
	aMetadata->flags.arcade = 0;
	aMetadata->flags.continues = 0;
	aMetadata->flags.random = 0;

	aMetadata->flags.rsvd2 = 0;
	aMetadata->flags.logging = 1;
	aMetadata->flags.logLocal = 1;
	aMetadata->flags.logLive = 0;
	aMetadata->flags.logPrint = 0;

	aMetadata->flags.rsvd3 = 0;
	aMetadata->flags.dictOffsetBytes = 0;

	aMetadata->values.coinsPerCredit = 2;
	aMetadata->values.coinDenomination = 25;

	aMetadata->storyCount = 0;
	aMetadata->usedStoryPages = 0;

	if (!writeMetadata(aMetadata)) {
		return false;
	}
	return true;
}

bool DataManager::readMetadata(Metadata *aMetadata) {
	DEBUG("Size of Metadata: %d", sizeof(*aMetadata));
	if (_metaFlash->readByte(kMetadataBaseAddress) == ASCII_SOH) {
		bool result = _metaFlash->read(aMetadata, kMetadataBaseAddress, kMetadataSize);
		if (!result) {
			Errors::setError(E_METADATA_READ_FAIL);
			ERROR(Errors::errorString());
			return false;
		}
	}
	return true;
}

bool DataManager::readStoryHeader(StoryHeader *aHeader, uint8_t aIndex) {
	uint32_t offset = getStoryOffset(aIndex) * Flashee::Devices::userFlash().pageSize();

	DEBUG("StoryHeader offset: %d, size: %d", offset, kStoryHeaderSize);
	bool result = readData((uint8_t*)aHeader, offset, kStoryHeaderSize);
	if (!result) {
		Errors::setError(E_HEADER_READ_FAIL);
		ERROR(Errors::errorString());
	}
	return result;
}

bool DataManager::writeMetadata(Metadata *aMetadata) {
	DEBUG("Size of Metadata: %d", sizeof(*aMetadata));
	DEBUG("Maybe size: %d", kMetadataSize);
	bool result = _metaFlash->write(aMetadata, kMetadataBaseAddress, kMetadataSize);
	if (!result) {
		Errors::setError(E_METADATA_WRITE_FAIL);
		ERROR(Errors::errorString());
	}
	char data[kMetadataSize] = "";
	_metaFlash->read(data, 0, kMetadataSize);
	DEBUG("META DEBUG BEGIN");
	DEBUG("HEX");
	for (int i = 0; i < kMetadataSize; ++i) {
		if (data[i]<0x10) {Serial.print("0");}
          Serial.print(data[i],HEX);
          Serial.print(" ");
	}
	Serial.println("END");
	DEBUG("META DEBUG END");
	return result;
}

bool DataManager::writeStoryCountData(Metadata *aMetadata) {
	/*bool result = _metaFlash->write(&aMetadata->storyCount,
		                 kMetadataStoryCountOffset,
		                 kMetadataStoryCountSize + kMetadataStoryUsedPagesSize +
		                 kMetadataStoryOffsetsSize + kMetadataStoryOrderSize);*/
	bool result = true;
	result = _metaFlash->write(&aMetadata->storyCount, kMetadataStoryCountOffset, kMetadataStoryCountSize);
	result = _metaFlash->write(&aMetadata->usedStoryPages, kMetadataStoryUsedPagesOffset, kMetadataStoryUsedPagesSize);
	result = _metaFlash->write(&aMetadata->storyOffsets, kMetadataStoryOffsetsOffset, kMetadataStoryOffsetsSize);
	result = _metaFlash->write(&aMetadata->storyOrder, kMetadataStoryOrderOffset, kMetadataStoryOrderSize);
	if (!result) {
		Errors::setError(E_METADATA_WRITE_FAIL);
		ERROR(Errors::errorString());
	}
	char data[kMetadataSize] = "";
	_metaFlash->read(data, 0, kMetadataSize);
	DEBUG("META DEBUG BEGIN");
	DEBUG("HEX");
	for (int i = 0; i < kMetadataSize; ++i) {
		if (data[i]<0x10) {Serial.print("0");}
          Serial.print(data[i],HEX);
          Serial.print(" ");
	}
	Serial.println("END");
	DEBUG("META DEBUG END");
	return result;
}

bool DataManager::didFirmwareUpdate(Metadata *aMetadata) {
	// Check if saved firmware version is out of date
	if (aMetadata->firmwareVer.major != this->firmwareVersion.major ||
		aMetadata->firmwareVer.minor != this->firmwareVersion.minor ||
		aMetadata->firmwareVer.revision != this->firmwareVersion.revision) {
		// First run after firmware update
		DEBUG("Firmware has been upgraded from v%d.%d.%d to v%d.%d.%d",
		    aMetadata->firmwareVer.major, aMetadata->firmwareVer.minor,
		    aMetadata->firmwareVer.revision, this->firmwareVersion.major,
		    this->firmwareVersion.minor, this->firmwareVersion.revision);
		return true;
	}
	return false;
}

bool DataManager::upgradeDataModels() {
	// Update the data models.


	// Wipe out data in flash
	// ** Do we need to 'zero' it out?
	// Save the new metadata
	if (writeMetadata(&this->metadata)) {
		Errors::setError(E_METADATA_WRITE_FAIL);
		ERROR(Errors::errorString());
		return false;
	}
	return true;
}

void DataManager::testMetadata() {
#ifdef DEBUG_BUILD
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
#endif
}

void DataManager::setTestMetadata(Metadata *aMetadata) {
#ifdef DEBUG_BUILD
	*aMetadata = {};

	aMetadata->soh = ASCII_SOH;

	aMetadata->firmwareVer.major = 9;
	aMetadata->firmwareVer.minor = 8;
	aMetadata->firmwareVer.revision = 76;

	aMetadata->flags.rsvd1 = 0;
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
	/*aMetadata->storyOffsets.push_back(1);
	aMetadata->storyOffsets.push_back(255);
	aMetadata->storyOffsets.push_back(65535);
	aMetadata->storyOffsets.push_back(2147483647);*/
#endif
}

void DataManager::logBinary(uint8_t aValue) {
#ifdef DEBUG_BUILD
	DEBUG("0x%c%c%c%c%c%c%c%c",
	    (IsBitSet(aValue, 7) ? '1' : '0'),
	    (IsBitSet(aValue, 6) ? '1' : '0'),
	    (IsBitSet(aValue, 5) ? '1' : '0'),
	    (IsBitSet(aValue, 4) ? '1' : '0'),
	    (IsBitSet(aValue, 3) ? '1' : '0'),
	    (IsBitSet(aValue, 2) ? '1' : '0'),
	    (IsBitSet(aValue, 1) ? '1' : '0'),
	    (IsBitSet(aValue, 0) ? '1' : '0'));
#endif
}

void DataManager::logStoryOffsets(Metadata *aMetadata) {
#ifdef DEBUG_BUILD
	for (int i = 0; i < aMetadata->storyCount; ++i) {
		DEBUG("Story #: %d, Offset: %lu", i, aMetadata->storyOffsets[i]);
	}
#endif
}

void DataManager::logStoryBytes(Metadata *aMetadata) {
#ifdef DEBUG_BUILD
	for (int i = 0; i < aMetadata->storyCount; ++i) {
		DEBUG("Story #: %d, Offset: %d", i, aMetadata->storyOffsets[i]);
		if (loadStoryHeader(i)) {
			logStoryHeader(&this->storyHeader);
		}
		char data[kStoryHeaderSize] = "";
		uint32_t offset = getStoryOffset(i) * Flashee::Devices::userFlash().pageSize();
		_storyFlash->read(data, offset, kStoryHeaderSize);
		DEBUG("HEADER DEBUG BEGIN");
		DEBUG("HEX");
		for (int i = 0; i < kStoryHeaderSize; ++i) {
			if (data[i]<0x10) {Serial.print("0");}
	          Serial.print(data[i],HEX);
	          Serial.print(" ");
		}
		Serial.println("END");
	}
#endif
}

void DataManager::logMetadata(Metadata *aMetadata) {
#ifdef DEBUG_BUILD
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
	DEBUG("Rsvd: %s", (aMetadata->flags.rsvd1 ? "on" : "off"));
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
	/*for (int i = 0; i < aMetadata->storyCount; ++i) {
		LOG("Story Size %d: %d", (i + 1), aMetadata->storyOffsets[i]);
	}*/
#endif
}

void DataManager::logStoryHeader(StoryHeader *aHeader) {
#ifdef DEBUG_BUILD
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

	DEBUG("Story Bits: %d", aHeader->vars.bits);
	DEBUG("Story 8-bit: %d", aHeader->vars.small);
	DEBUG("Story 16-bit: %d", aHeader->vars.big);

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
#endif
}

}