
#ifndef CDAM_ERROR_H
#define CDAM_ERROR_H

namespace cdam
{

typedef enum ErrorCode_t {
	E_NO_ERROR = 0,
	E_METADATA_READ_FAIL,
	E_METADATA_READ_STORY_OFFSET_FAIL,
	E_METADATA_WRITE_FAIL,
	E_METADATA_WRITE_STORY_OFFSET_FAIL,
	E_DATA_MODEL_UPGRADE_FAIL,
	E_SERVER_CONNECTION_FAIL,
	E_SERVER_ADD_STORY_LEN_FAIL,
	E_SERVER_ADD_STORY_NO_SPACE,
	E_SERVER_SOCKET_DATA_FAIL
} ErrorCode;

class Errors {

public:
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