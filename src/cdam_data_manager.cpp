#include "cdam_data_manager.h"
#include "cdam_constants.h"
#include "cdam_flash.h"

namespace cdam
{

/* Public Methods */

DataManager::DataManager() {
}

bool DataManager::initialize() {
	//Serial.begin(BAUD_RATE);
	//Flash::init();

	this->metadata = {};
	LOG("Page Size: %d", Flashee::Devices::userFlash().pageSize());
	LOG("Page Count: %d", Flashee::Devices::userFlash().pageCount());
	_metaFlash = Flashee::Devices::createAddressErase(0, 4*4096);
	//_metaFlash->writeString("Hello World!", 0);
	//char buf[13];
    //_metaFlash->read(buf, 0, 13);
    //LOG("BLAH");
    //LOG("%d", _metaFlash->length());
    //LOG(buf);

    //testMetadata();
	loadMetadata(&this->metadata);
	logMetadata(&this->metadata);

	//_metaFlash->eraseAll();

	/*if (!loadFirmwareVersion()) {
		return false;
	}
	if (!loadMetadata()) {
		return false;
	}*/
	return true;
}

void logMetadata() {

}

/* Accessors */


/* Private Methods */

bool DataManager::loadFirmwareVersion() {
	// Set firmware version.
	this->firmwareVersion.major = kFirmwareVersionMajor;
	this->firmwareVersion.minor = kFirmwareVersionMinor;
	this->firmwareVersion.revision = kFirmwareVersionRevision;
	return true;
}

bool DataManager::loadMetadata() {
	// Load and set metadata.

	// Check for SOH
	if (Flash::readByte(kMetadataBaseAddress) != ASCII_SOH) {
		LOG("No SOH, write fresh metadata.");

		if (!writeMetadata(&this->metadata)) {
			ERROR(Errors::errorString());
			return false;
		}
	} else {
		// Data exists. Read it!
		LOG("SOH found, read metadata.");
		if (!readMetadata(&this->metadata)) {
			ERROR(Errors::errorString());
			return false;
		}
	}
	LOG("Firmware v%d.%d.%d", this->metadata.firmwareVer.major,
		this->metadata.firmwareVer.minor, this->metadata.firmwareVer.revision);

	return true;
}

bool DataManager::testMetadata() {
	Metadata metadata = {};
	_metaFlash->eraseAll();
	loadMetadata(&metadata);
	LOG("*** Should be empty ***");
	logMetadata(&metadata);
	setTestMetadata(&metadata);
	LOG("*** Should be filled in ***");
	logMetadata(&metadata);
	saveMetadata(&metadata);
	metadata = {};
	LOG("*** Should be empty ***");
	logMetadata(&metadata);
	loadMetadata(&metadata);
	LOG("*** Should be AWESOME ***");
	logMetadata(&metadata);

	return true;
}

void DataManager::setTestMetadata(Metadata *aMetadata) {
	aMetadata->soh = ASCII_SOH;

	aMetadata->firmwareVer.major = 9;
	aMetadata->firmwareVer.minor = 8;
	aMetadata->firmwareVer.revision = 76;

	// Flag 1
	// Set bit to 0
	aMetadata->flags.flag1 &= (~FLG1_OFFLINE);
	// Set bit to 1
	aMetadata->flags.flag1 |= FLG1_OFFLINE;
	aMetadata->flags.flag1 |= FLG1_SD;
	aMetadata->flags.flag1 |= FLG1_ARCADE;

	// Flag 2
	aMetadata->flags.flag2 |= FLG2_LOG_LOCAL;

	aMetadata->flags.flag3 = 0xAA;
	aMetadata->flags.flag4 = 0x55;
	aMetadata->flags.flag5 = 0xAA;
	aMetadata->flags.flag6 = 0x55;
	aMetadata->flags.flag7 = 0xAA;
	aMetadata->flags.flag8 = 0x55;

	aMetadata->values.coinsPerCredit = 2;
	aMetadata->values.coinDenomination = 25;
	aMetadata->values.value3 = 3;
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
	aMetadata->storySizes.push_back(1);
	aMetadata->storySizes.push_back(255);
	aMetadata->storySizes.push_back(65535);
	aMetadata->storySizes.push_back(2147483647);
}

void DataManager::logMetadata(Metadata *aMetadata) {
	LOG("METADATA");
	LOG("Firmware v%d.%d.%d", aMetadata->firmwareVer.major,
		aMetadata->firmwareVer.minor, aMetadata->firmwareVer.revision);
	LOG("Offline: %s", (IsBitSet(aMetadata->flags.flag1, 7) ? "on" : "off"));
	LOG("Demo: %s", (IsBitSet(aMetadata->flags.flag1, 6) ? "on" : "off"));
	LOG("SD: %s", (IsBitSet(aMetadata->flags.flag1, 5) ? "on" : "off"));
	LOG("Multi: %s", (IsBitSet(aMetadata->flags.flag1, 4) ? "on" : "off"));
	LOG("Arcade: %s", (IsBitSet(aMetadata->flags.flag1, 3) ? "on" : "off"));
	LOG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag1, 2) ? "on" : "off"));
	LOG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag1, 1) ? "on" : "off"));
	LOG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag1, 0) ? "on" : "off"));
	LOG("Logging: %s", (IsBitSet(aMetadata->flags.flag2, 7) ? "on" : "off"));
	LOG("Log Local: %s", (IsBitSet(aMetadata->flags.flag2, 6) ? "on" : "off"));
	LOG("Log Live: %s", (IsBitSet(aMetadata->flags.flag2, 5) ? "on" : "off"));
	LOG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag2, 4) ? "on" : "off"));
	LOG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag2, 3) ? "on" : "off"));
	LOG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag2, 2) ? "on" : "off"));
	LOG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag2, 1) ? "on" : "off"));
	LOG("Rsvd: %s", (IsBitSet(aMetadata->flags.flag2, 0) ? "on" : "off"));
	logBinary(aMetadata->flags.flag3);
	logBinary(aMetadata->flags.flag4);
	logBinary(aMetadata->flags.flag5);
	logBinary(aMetadata->flags.flag6);
	logBinary(aMetadata->flags.flag7);
	logBinary(aMetadata->flags.flag8);
	LOG("Coins Per Credit: %d", aMetadata->values.coinsPerCredit);
	LOG("Coin Denom: %d", aMetadata->values.coinDenomination);
	LOG("Value 3: %d", aMetadata->values.value3);
	LOG("Value 4: %d", aMetadata->values.value4);
	LOG("Value 5: %d", aMetadata->values.value5);
	LOG("Value 6: %d", aMetadata->values.value6);
	LOG("Value 7: %d", aMetadata->values.value7);
	LOG("Value 8: %d", aMetadata->values.value8);
	LOG("Value 9: %d", aMetadata->values.value9);
	LOG("Value 10: %d", aMetadata->values.value10);
	LOG("Value 11: %d", aMetadata->values.value11);
	LOG("Value 12: %d", aMetadata->values.value12);
	LOG("Value 13: %d", aMetadata->values.value13);
	LOG("Value 14: %d", aMetadata->values.value14);
	LOG("Value 15: %d", aMetadata->values.value15);
	LOG("Value 16: %d", aMetadata->values.value16);
	LOG("Story Count: %d", aMetadata->storyCount);
	for (int i = 0; i < aMetadata->storyCount; ++i) {
		LOG("Story Size %d: %d", (i + 1), aMetadata->storySizes[i]);
	}
}

void DataManager::logBinary(uint8_t aValue) {
	LOG("0x%c%c%c%c%c%c%c%c",
	    (IsBitSet(aValue, 7) ? '1' : '0'),
	    (IsBitSet(aValue, 6) ? '1' : '0'),
	    (IsBitSet(aValue, 5) ? '1' : '0'),
	    (IsBitSet(aValue, 4) ? '1' : '0'),
	    (IsBitSet(aValue, 3) ? '1' : '0'),
	    (IsBitSet(aValue, 2) ? '1' : '0'),
	    (IsBitSet(aValue, 1) ? '1' : '0'),
	    (IsBitSet(aValue, 0) ? '1' : '0'));
}

void DataManager::loadMetadata(Metadata *aMetadata) {
	if (_metaFlash->readByte(kMetadataBaseAddress) == ASCII_SOH) {
		LOG("*** Loading Data ***");
		bool result = _metaFlash->read(aMetadata, kMetadataBaseAddress, sizeof(*aMetadata));
		if (result) {
			LOG("*** Loaded Data ***");
			for (int i = 0; i < aMetadata->storyCount; ++i) {
				_metaFlash->read(&aMetadata->storySizes[i],
				                 sizeof(*aMetadata) + (i * 32),
				                 (uint32_t)32);
			}
		} else {
			LOG("*** Failed to Load Data ***");
		}
	} else {
		LOG("*** No SOH to Read ***");
	}
}

void DataManager::saveMetadata(Metadata *aMetadata) {
	if (_metaFlash->write(aMetadata, kMetadataBaseAddress, sizeof(*aMetadata))) {
		for (int i = 0; i < aMetadata->storyCount; ++i) {
			_metaFlash->write(&aMetadata->storySizes[i],
			                 sizeof(*aMetadata) + (i * 32),
			                 (uint32_t)32);
		}
		LOG("*** Wrote Data ***");
	} else {
		LOG("*** Failed to Write Data ***");
	}
}

bool DataManager::writeMetadata(Metadata *aMetadata) {
	// Flash not set (first start), or there is a problem.

	// Write the SOH byte
	if (!Flash::writeByte(kMetadataBaseAddress, ASCII_SOH)) {
		Errors::setError(E_METADATA_WRITE_SOH_FAIL);
		return false;
	}

	// Then read it again to verify
	if (Flash::readByte(kMetadataBaseAddress) != ASCII_SOH) {
		// Something is broken and we should abort.
		Errors::setError(E_METADATA_READ_FAIL);
		return false;
	}

	// Initialize the metadata to default values
	aMetadata->firmwareVer = this->firmwareVersion;

	aMetadata->flags.flag1 = 0;
	aMetadata->flags.flag2 = 0;
	aMetadata->flags.flag3 = 0;
	// Other flags are not used

	aMetadata->values.coinsPerCredit = 0;
	aMetadata->values.coinDenomination = 0;
	// Other values are not used

	aMetadata->storyCount = 0;

	if (!Flash::writeBytes((uint8_t *)aMetadata, kMetadataBaseAddress + kMetadataFirmwareOffset, (uint32_t)sizeof(aMetadata))) {
		Errors::setError(E_METADATA_WRITE_FAIL);
		return false;
	}

	return true;
}

bool DataManager::readMetadata(Metadata *aMetadata) {
	uint8_t fwBytes[3] = {};
	if (!Flash::readBytes(fwBytes, kMetadataBaseAddress + kMetadataFirmwareOffset, 3)) {
		Errors::setError(E_METADATA_READ_VERSION_FAIL);
		return false;
	}

	// Load firmware version
	aMetadata->firmwareVer.major = fwBytes[0];
	aMetadata->firmwareVer.minor = fwBytes[1];
	aMetadata->firmwareVer.revision = fwBytes[2];

	// Load flags
	uint8_t flagBytes[kMetadataFlagsCount] = {};
	if (!Flash::readBytes(flagBytes, kMetadataBaseAddress + kMetadataFlagsOffset, kMetadataFlagsCount)) {
		Errors::setError(E_METADATA_READ_FLAGS_FAIL);
		return false;
	}
	aMetadata->flags.flag1 = flagBytes[0];
	aMetadata->flags.flag2 = flagBytes[1];
	aMetadata->flags.flag3 = flagBytes[2];
	aMetadata->flags.flag4 = flagBytes[3];
	aMetadata->flags.flag5 = flagBytes[4];
	aMetadata->flags.flag6 = flagBytes[5];
	aMetadata->flags.flag7 = flagBytes[6];
	aMetadata->flags.flag8 = flagBytes[7];

	// Load values
	uint8_t valueBytes[kMetadataValuesCount] = {};
	if (!Flash::readBytes(valueBytes, kMetadataBaseAddress + kMetadataValuesOffset, kMetadataValuesCount)) {
		Errors::setError(E_METADATA_READ_VALUES_FAIL);
		return false;
	}
	aMetadata->values.coinsPerCredit = valueBytes[0];
	aMetadata->values.coinDenomination = valueBytes[1];
	aMetadata->values.value3 = valueBytes[2];
	aMetadata->values.value4 = valueBytes[3];
	aMetadata->values.value5 = valueBytes[4];
	aMetadata->values.value6 = valueBytes[5];
	aMetadata->values.value7 = valueBytes[6];
	aMetadata->values.value8 = valueBytes[7];
	aMetadata->values.value9 = valueBytes[8];
	aMetadata->values.value10 = valueBytes[9];
	aMetadata->values.value11 = valueBytes[10];
	aMetadata->values.value12 = valueBytes[11];
	aMetadata->values.value13 = valueBytes[12];
	aMetadata->values.value14 = valueBytes[13];
	aMetadata->values.value15 = valueBytes[14];
	aMetadata->values.value16 = valueBytes[15];

	// Load story count (failure returns 0 stories)
	aMetadata->storyCount = Flash::readByte(kMetadataStoryCountOffset);

	if (aMetadata->storyCount == 0) {
		Errors::setError(E_METADATA_READ_STORY_COUNT_FAIL);
		return false;
	}

	// Load story sizes (4 bytes each)
	for (int i = 0; i < aMetadata->storyCount; i++) {
		uint32_t address = kMetadataBaseAddress + kMetadataStorySizesOffset + 4 * i;
		uint8_t storySize[4] = {};
		if (!Flash::readBytes(storySize, address, 4)) {
			Errors::setError(E_METADATA_READ_STORY_SIZE_FAIL);
			return false;
		}
		uint32_t *storySize32 = (uint32_t *)storySize;
		aMetadata->storySizes.push_back(*storySize32);
	}

	// Check if saved firmware version is out of date
	if (aMetadata->firmwareVer.major != this->firmwareVersion.major ||
		aMetadata->firmwareVer.minor != this->firmwareVersion.minor ||
		aMetadata->firmwareVer.revision != this->firmwareVersion.revision) {
		// First run after firmware update
		LOG("Firmware has been upgraded from v%d.%d.%d to v%d.%d.%d",
		    aMetadata->firmwareVer.major, aMetadata->firmwareVer.minor,
		    aMetadata->firmwareVer.revision, this->firmwareVersion.major,
		    this->firmwareVersion.minor, this->firmwareVersion.revision);
		// Update to the current standard (in memory)
		// Wipe out data in flash
		// Save the new metadata
	}

	return true;
}

void DataManager::printMetadata() {

}

}