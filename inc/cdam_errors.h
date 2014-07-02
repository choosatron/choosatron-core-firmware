
#ifndef CDAM_ERROR_H
#define CDAM_ERROR_H

namespace cdam
{

typedef enum ErrorCode_t {
	E_NO_ERROR = 0,
	E_METADATA_READ_FAIL,
	E_METADATA_READ_VERSION_FAIL,
	E_METADATA_READ_FLAGS_FAIL,
	E_METADATA_READ_VALUES_FAIL,
	E_METADATA_READ_STORY_COUNT_FAIL,
	E_METADATA_READ_STORY_SIZE_FAIL,
	E_METADATA_WRITE_FAIL,
	E_METADATA_WRITE_SOH_FAIL
} ErrorCode;

class Errors {

public:
	~Errors();

	static void setError(ErrorCode aCode);
	static const char *errorString();
    static const char *errorString(ErrorCode aCode);

protected:

private:
	static ErrorCode _lastError;

	ErrorCode* _errors;

};

}

#endif