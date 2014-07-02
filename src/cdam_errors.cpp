
#include "cdam_errors.h"

namespace cdam
{

struct ErrorDesc_t {
	ErrorCode code;
	const char *message;
} ErrorDesc[] = {
	{ E_NO_ERROR, "No error." },
	{ E_METADATA_READ_FAIL, "Failed to read from flash." },
	{ E_METADATA_READ_VERSION_FAIL, "Failed to read firmware version." },
	{ E_METADATA_READ_FLAGS_FAIL, "Failed to read metadata flags." },
	{ E_METADATA_READ_VALUES_FAIL, "Failed to read metadata values." },
	{ E_METADATA_READ_STORY_COUNT_FAIL, "Failed to read story count." },
	{ E_METADATA_READ_STORY_SIZE_FAIL, "Failed to read the size of a story." },
	{ E_METADATA_WRITE_FAIL, "Failed to write to flash." },
	{ E_METADATA_WRITE_SOH_FAIL, "Failed to write 'start of header'." }
};

ErrorCode Errors::lastError = E_NO_ERROR;

void Errors::setError(ErrorCode aCode) {
	Errors::lastError = aCode;
}

const char *Errors::errorString() {
	return ErrorDesc[Errors::lastError].message;
}

const char *Errors::errorString(ErrorCode aCode) {
	return ErrorDesc[aCode].message;
}


}