
namespace cdam
{

lastError = E_NO_ERROR;

Errors::~ErrorManager() {
	//delete _errors [];
}

void Errors::setError(ErrorCode aCode) {
	_lastError = aCode;
}

const char *Errors::errorString() {
	return ErrorDesc[_lastError];
}

const char *Errors::errorString(ErrorCode aCode) {
	return ErrorDesc[aCode];
}


}