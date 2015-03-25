
#ifndef CDAM_ERROR_H
#define CDAM_ERROR_H

namespace cdam
{

typedef enum ErrorCode_t {
	E_NO_ERROR = 0,
	E_ERROR,
	E_METADATA_READ_FAIL,
	E_HEADER_READ_FAIL,
	E_VARS_READ_FAIL,
	E_METADATA_WRITE_FAIL,
	E_DATA_MODEL_UPGRADE_FAIL,
	E_SERVER_CONNECTION_FAIL,
	E_SERVER_ADD_STORY_LEN_FAIL,
	E_SERVER_ADD_STORY_NO_SPACE,
	E_SERVER_SOCKET_DATA_FAIL,
	E_INVALID_VARIABLE,
	E_STORY_READ_FAIL,
	E_STORY_PARSE_FAIL,
	E_DATA_TOO_LARGE_FAIL
} ErrorCode;

class Errors {

public:
	Errors();
	~Errors();

	static void setError(ErrorCode aCode);
	static void clearError();
	static const char *errorString();
    static const char *errorString(ErrorCode aCode);

    static ErrorCode lastError;
protected:

private:
	ErrorCode* _errors;

};

}

#endif