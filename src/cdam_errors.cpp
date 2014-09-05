
#include "cdam_errors.h"

namespace cdam
{

/* TODO - REMOVE ALL ERROR STUFF WHEN NOT USING DEBUG_BUILD */

struct ErrorDesc_t {
	ErrorCode code;
	const char *message;
} ErrorDesc[] = {
	{ E_NO_ERROR, "No error." },
	{ E_ERROR, "Error" },
	{ E_METADATA_READ_FAIL, "Failed to read from flash." },
	{ E_HEADER_READ_FAIL, "Failed to read the header of a story." },
	{ E_VARS_READ_FAIL, "Failed to read variables." },
	{ E_METADATA_WRITE_FAIL, "Failed to write to flash." },
	{ E_DATA_MODEL_UPGRADE_FAIL, "Failed to upgrade data model." },
	{ E_SERVER_CONNECTION_FAIL, "Failed to connect to server." },
	{ E_SERVER_ADD_STORY_LEN_FAIL, "add_story command isn't correct length of bytes." },
	{ E_SERVER_ADD_STORY_NO_SPACE, "Not enough free space for selected story." },
	{ E_SERVER_SOCKET_DATA_FAIL, "Either too few or too many bytes received from server." },
	{ E_INVALID_SMALL_VARIABLE, "Invalid index provided for small variable." },
	{ E_INVALID_BIG_VARIABLE, "Invalid index provided for big variable." },
	{ E_STORY_READ_FAIL, "Failed to read from story data." },
	{ E_STORY_PARSE_FAIL, "Failed to parse a story." },
	{ E_DATA_TOO_LARGE_FAIL, "Data is too large for buffer." }
};

ErrorCode Errors::lastError = E_NO_ERROR;

Errors::Errors() {}

void Errors::setError(ErrorCode aCode) {
	Errors::lastError = aCode;
}

void Errors::clearError() {
	Errors::lastError = E_NO_ERROR;
}

const char *Errors::errorString() {
	return ErrorDesc[Errors::lastError].message;
}

const char *Errors::errorString(ErrorCode aCode) {
	return ErrorDesc[aCode].message;
}


}