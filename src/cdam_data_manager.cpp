#include "cdam_data_manager.h"
#include "cdam_constants.h"
#include "cdam_flash.h"
#include "flashee-eeprom.h"

namespace cdam
{

/* Public Methods */

DataManager::DataManager() {
}

bool DataManager::initialize() {
	//Serial.begin(BAUD_RATE);
	Flash::init();

	if (!loadFirmwareVersion()) {
		return false;
	}
	if (!loadMetadata()) {
		return false;
	}
	return true;
}

String DataManager::firmwareVersionString() {
	String ver = String('v');
	ver += this->metadata.firmwareVer.major;
	ver += '.';
	ver += this->metadata.firmwareVer.minor;
	ver += '.';
	ver += this->metadata.firmwareVer.revision;
	return ver;
}

char *DataManager::firmwareVersionStr(char *aVersion) {
	aVersion[0] = 'v';
	aVersion[1] = this->metadata.firmwareVer.major;
	aVersion[2] = '.';
	aVersion[3] = this->metadata.firmwareVer.minor;
	aVersion[4] = '.';
	aVersion[5] = this->metadata.firmwareVer.revision;
	aVersion[6] = '\0';
}

/* Accessors */


/* Private Methods */

bool DataManager::loadFirmwareVersion() {
	// Set firmware version.
	this->firmwareVersion.major = FIRMWARE_VERSION_MAJ;
	this->firmwareVersion.minor = FIRMWARE_VERSION_MIN;
	this->firmwareVersion.revision = FIRMWARE_VERSION_REV;
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

		char version[7] = {};
		LOG("Firmware %s", firmwareVersionStr(version));
		free(version);
	}

	return true;
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

		// Update to the current standard (in memory)
		// Wipe out data in flash
		// Save the new metadata
	}

	return true;
}

void DataManager::printMetadata() {

}

}