#include "cdam_data_manager.h"
#include "cdam_constants.h"
#include "cdam_flash.h"

namespace cdam
{

/* Public Methods */

DataManager::DataManager() {
}

bool DataManager::initialize() {
	this->gameCredits = 0;

	this->metadata = {};
	DEBUG("Page Size: %d", Flashee::Devices::userFlash().pageSize());
	DEBUG("Page Count: %d", Flashee::Devices::userFlash().pageCount());
	_metaFlash = Flashee::Devices::createAddressErase(0, 4*4096);
	_storyFlash = Flashee::Devices::createWearLevelErase(4*4096, 260*4096);


    //testMetadata();
	//_metaFlash->eraseAll();

	loadFirmwareVersion();

	if (!loadMetadata()) {
		return false;
	}
	return true;
}

/* Accessors */


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
		logMetadata(&this->metadata);
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

	//aMetadata->flags.flag1 |= FLG1_OFFLINE;
	//aMetadata->flags.flag1 |= FLG1_DEMO;
	//aMetadata->flags.flag1 |= FLG1_SD;
	//aMetadata->flags.flag1 |= FLG1_MULTI;
	//aMetadata->flags.flag1 |= FLG1_ARCADE;

	aMetadata->flags.flag2 |= FLG2_LOGGING;
	aMetadata->flags.flag2 |= FLG2_LOG_LOCAL;
	//aMetadata->flags.flag2 |= FLG2_LOG_LIVE;

	aMetadata->values.coinsPerCredit = 2;
	aMetadata->values.coinDenomination = 25;

	aMetadata->storyCount = 0;
	logMetadata(aMetadata);
	if (!writeMetadata(aMetadata)) {
		return false;
	}
	return true;
}

bool DataManager::readMetadata(Metadata *aMetadata) {
	if (_metaFlash->readByte(kMetadataBaseAddress) == ASCII_SOH) {
		bool result = _metaFlash->read(aMetadata, kMetadataBaseAddress, sizeof(*aMetadata));
		if (result) {
			DEBUG("*** Loaded Data ***");
			for (int i = 0; i < aMetadata->storyCount; ++i) {
				result = _metaFlash->read(&aMetadata->storySizes[i],
				                          sizeof(*aMetadata) + (i * sizeof(uint32_t)),
				                          sizeof(uint32_t));
				if (!result) {
					Errors::setError(E_METADATA_READ_STORY_SIZE_FAIL);
					ERROR(Errors::errorString());
					return false;
				}
				DEBUG("Story %d size: %d", i, aMetadata->storySizes[i]);
			}
		} else {
			Errors::setError(E_METADATA_READ_FAIL);
			ERROR(Errors::errorString());
			return false;
		}
	}
	return true;
}

bool DataManager::writeMetadata(Metadata *aMetadata) {
	bool result = _metaFlash->write(aMetadata, kMetadataBaseAddress, sizeof(*aMetadata));
	if (result) {
		DEBUG("*** Wrote Data ***");
		for (int i = 0; i < aMetadata->storyCount; ++i) {
			result = _metaFlash->write(&aMetadata->storySizes[i],
			                 sizeof(*aMetadata) + (i * sizeof(uint32_t)),
			                 sizeof(uint32_t));
			if (!result) {
				Errors::setError(E_METADATA_WRITE_STORY_SIZE_FAIL);
				ERROR(Errors::errorString());
				return false;
			}
		}
	} else {
		Errors::setError(E_METADATA_WRITE_FAIL);
		ERROR(Errors::errorString());
		return false;
	}
	return true;
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

bool DataManager::testMetadata() {
	Metadata metadata = {};
	_metaFlash->eraseAll();
	readMetadata(&metadata);
	LOG("*** Should be empty ***");
	logMetadata(&metadata);
	setTestMetadata(&metadata);
	LOG("*** Should be filled in ***");
	logMetadata(&metadata);
	writeMetadata(&metadata);
	metadata = {};
	LOG("*** Should be empty ***");
	logMetadata(&metadata);
	readMetadata(&metadata);
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

}