
#ifndef CDAM_STATE_CONTROLLER_H
#define CDAM_STATE_CONTROLLER_H

#include "cdam_constants.h"
#include "elapsed_time.h"

namespace cdam
{

// Game States
typedef enum GameState_t {
	STATE_NONE = 0,
	STATE_ERROR,
	STATE_BOOTING,
	STATE_INIT,
	STATE_CREDITS,
	STATE_WAITING,
	STATE_READY,
	STATE_IDLE,
	STATE_SELECT,
	STATE_PLAY,
	STATE_CONTINUE,
	STATE_AUTH,
	STATE_ADMIN
} GameState;

class DataManager;
class HardwareManager;
class ServerManager;
class Parser;

class StateController {

public:
/* Public Methods */
	StateController();

	void initialize();
	GameState getState();
	const char* stateString();
	void changeState(GameState aState);
	void updateState();

/* Public Variables */

protected:

private:
/* Private Methods */
	void initState(GameState aState);
	void loopState(GameState aState);
	void endState(GameState aState);

/* Private Variables */
	DataManager* _dataManager;
	HardwareManager* _hardwareManager;
	ServerManager* _serverManager;
	Parser* _parser;
	GameState _state;
	ElapsedMillis _resetElapsed;
	uint32_t _seed;
};

}

#endif