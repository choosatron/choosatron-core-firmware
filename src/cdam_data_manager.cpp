#include "cdam_data_manager.h"

void DataManager::DataManager() {
}

bool DataManager::initilize() {
	Serial.begin(BAUD_RATE);
	setupHardwarePins();
}

bool DataManager::loadFirmwareVersion() {
	// Load and set firmware version.
	this->metadata.firmwareVer.major = 1; // DEBUG
	this->metadata.firmwareVer.minor = 2; // DEBUG
	this->metadata.firmwareVer.revision = 3; // DEBUG
}

bool DataManager::loadMetadata() {
	// Load and set metadata.

	this->metadata.storyCount = 1; // DEBUG
	this->metadata.storySizes[0] = 500; // DEBUG
}

String DataManager::firmwareVersion() {
	String ver = String('v');
	ver += this->metadata.firmwareVer.major;
	ver += '.';
	ver += this->metadata.firmwareVer.minor;
	ver += '.';
	ver += this->metadata.firmwareVer.revision;
	return ver;
}




// Private Methods

void DataManager::setupHardwarePins() {
	pinMode(PIN_PIEZO, OUTPUT);
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_GREEN, OUTPUT);
	pinMode(PIN_BLUE_, OUTPUT);
}