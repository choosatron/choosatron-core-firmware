
#ifndef CDAM_ERROR_H
#define CDAM_ERROR_H

namespace cdam
{

typedef enum ErrorCode_t {
	E_NO_ERROR = 0,
	E_METADATA_READ_FAIL = 1,
	E_METADATA_READ_VERSION_FAIL = 2,
	E_METADATA_READ_FLAGS_FAIL = 3,
	E_METADATA_READ_VALUES_FAIL = 4,
	E_METADATA_READ_STORY_COUNT_FAIL = 5,
	E_METADATA_READ_STORY_SIZE_FAIL = 6,
	E_METADATA_WRITE_FAIL = 7,
	E_METADATA_WRITE_SOH_FAIL = 8
} ErrorCode;

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

class Errors {

public:
	~Errors();

	static void setError(ErrorCode aCode);
	static const char *errorString();
    static const char *errorString(ErrorCode aCode);

    static ErrorCode lastError;

protected:

private:
	ErrorCode* _errors;

};

}

#endif