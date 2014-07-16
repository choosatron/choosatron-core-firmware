
#include "cdam_errors.h"

namespace cdam
{

struct ErrorDesc_t {
	ErrorCode code;
	const char *message;
} ErrorDesc[] = {
	{ E_NO_ERROR, "No error." },
	{ E_METADATA_READ_FAIL, "Failed to read from flash." },
	{ E_METADATA_READ_STORY_SIZE_FAIL, "Failed to read the size of a story." },
	{ E_METADATA_WRITE_FAIL, "Failed to write to flash." },
	{ E_METADATA_WRITE_STORY_SIZE_FAIL, "Failed to write the size of a story." },
	{ E_DATA_MODEL_UPGRADE_FAIL, "Failed to upgrade data model." },
	{ E_SERVER_CONNECTION_FAIL, "Failed to connect to server." },
	{ E_SERVER_ADD_STORY_LEN_FAIL, "add_story command isn't correct length of bytes." },
	{ E_SERVER_ADD_STORY_NO_SPACE, "Not enough free space for selected story." },
	{ E_SERVER_SOCKET_DATA_FAIL, "Either too few or too many bytes received from server." }
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