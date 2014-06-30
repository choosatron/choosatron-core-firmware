#include "cdam_data_manager.h"
#include "cdam_constants.h"
#include "cdam_flash.h"

namespace cdam
{

/* Public Methods */

DataManager::DataManager() {
}

bool DataManager::initialize() {
	Serial.begin(BAUD_RATE);

	if (!loadFirmwareVersion()) {
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
	if (Flash::readByte(METADATA_BASE_ADDRESS) != ASCII_SOH) {
		// Flash not set (first start), or there is a problem.

		// Write the SOH byte
		if (!Flash::writeByte(METADATA_BASE_ADDRESS, ASCII_SOH)) {
			Errors::setError(E_METADATA_WRITE_SOH_FAIL);
			return false;
		}

		// Then read it again to verify
		if (Flash::readByte(METADATA_BASE_ADDRESS) != ASCII_SOH) {
			// Something is broken and we should abort.
			Errors::setError(E_METADATA_READ_FAIL);
			return false;
		}

		// Initialize the metadata to default values
		this->metadata.firmwareVer = this->firmwareVersion;

		this->metadata.flags.flag1 = 0;
		this->metadata.flags.flag2 = 0;
		this->metadata.flags.flag3 = 0;
		// Other flags are not used

		this->metadata.values.coinsPerCredit = 0;
		this->metadata.values.coinDenomination = 0;
		// Other values are not used

		this->metadata.storyCount = 0;

		// TODO: WRITE TO FLASH

	} else {
		// Data exists.  Read it!
		uint8_t fwBytes[3] = {};
		if (!Flash::readBytes(fwBytes, METADATA_BASE_ADDRESS + METADATA_FIRMWARE_OFFSET, 3)) {
			Errors::setError(E_METADATA_READ_VERSION_FAIL);
			return false;
		}

		// Load firmware version
		this->metadata.firmwareVer.major = fwBytes[0];
		this->metadata.firmwareVer.minor = fwBytes[1];
		this->metadata.firmwareVer.revision = fwBytes[2];

			// Load flags
			uint8_t flagBytes[METADATA_FLAGS_COUNT] = {};
			if (!Flash::readBytes(flagBytes, METADATA_BASE_ADDRESS + METADATA_FLAGS_OFFSET, METADATA_FLAGS_COUNT)) {
				Errors::setError(E_METADATA_READ_FLAGS_FAIL);
				return false;
			}
			this->metadata.flags.flag1 = flagBytes[0];
			this->metadata.flags.flag2 = flagBytes[1];
			this->metadata.flags.flag3 = flagBytes[2];
			this->metadata.flags.flag4 = flagBytes[3];
			this->metadata.flags.flag5 = flagBytes[4];
			this->metadata.flags.flag6 = flagBytes[5];
			this->metadata.flags.flag7 = flagBytes[6];
			this->metadata.flags.flag8 = flagBytes[7];

			// Load values
			uint8_t valueBytes[METADATA_VALUES_COUNT] = {};
			if (!Flash::readBytes(valueBytes, METADATA_BASE_ADDRESS + METADATA_VALUES_OFFSET, METADATA_VALUES_COUNT)) {
				Errors::setError(E_METADATA_READ_VALUES_FAIL);
				return false;
			}
			this->metadata.values.coinsPerCredit = valueBytes[0];
			this->metadata.values.coinDenomination = valueBytes[1];
			this->metadata.values.value3 = valueBytes[2];
			this->metadata.values.value4 = valueBytes[3];
			this->metadata.values.value5 = valueBytes[4];
			this->metadata.values.value6 = valueBytes[5];
			this->metadata.values.value7 = valueBytes[6];
			this->metadata.values.value8 = valueBytes[7];
			this->metadata.values.value9 = valueBytes[8];
			this->metadata.values.value10 = valueBytes[9];
			this->metadata.values.value11 = valueBytes[10];
			this->metadata.values.value12 = valueBytes[11];
			this->metadata.values.value13 = valueBytes[12];
			this->metadata.values.value14 = valueBytes[13];
			this->metadata.values.value15 = valueBytes[14];
			this->metadata.values.value16 = valueBytes[15];

			// Load story count (failure returns 0 stories)
			this->metadata.storyCount = Flash::readByte(METADATA_STORYCOUNT_OFFSET);

			if (this->metadata.storyCount == 0) {
				Errors::setError(E_METADATA_READ_STORY_COUNT_FAIL);
				return false;
			}

			// Load story sizes (4 bytes each)
			for (int i = 0; i < this->metadata.storyCount; i++) {
				uint32_t address = METADATA_BASE_ADDRESS + METADATA_STORYSIZES_OFFSET + 4 * i;
				uint8_t storySize[4] = {};
				if (!Flash::readBytes(storySize, address, 4)) {
					Errors::setError(E_METADATA_READ_STORY_SIZE_FAIL);
					return false;
				}
				uint32_t *storySize32 = (uint32_t *)storySize;
				this->metadata.storySizes.push_back(*storySize32);
			}

		// Check if saved firmware version is out of date
		if (this->metadata.firmwareVer.major != this->firmwareVersion.major ||
			this->metadata.firmwareVer.minor != this->firmwareVersion.minor ||
			this->metadata.firmwareVer.revision != this->firmwareVersion.revision) {
			// First run after firmware update

			// Update to the current standard (in memory)
			// Wipe out data in flash
			// Save the new metadata
		}
	}

	return true;
}

}