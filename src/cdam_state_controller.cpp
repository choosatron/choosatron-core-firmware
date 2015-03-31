#include "cdam_state_controller.h"
#include "cdam_manager.h"
#include "cdam_parser.h"

namespace cdam {

struct GameStateStr_t {
	GameState state;
	const char *stateDesc;
} GameStateDesc[] = {
	{ STATE_NONE, "none" }, // Only before actual state is set.
	{ STATE_ERROR, "error" }, // Problem, report if possible.
	{ STATE_BOOTING, "booting" }, // Only occurs during power cycle.
	{ STATE_INIT, "init" }, // Initializes unit for new play.
	{ STATE_CREDITS, "credits" }, // Waiting for play credits.
	{ STATE_WAITING, "waiting" }, // Waiting for interaction.
	{ STATE_READY, "ready" }, //
	{ STATE_IDLE, "idle" },
	{ STATE_SELECT, "select" },
	{ STATE_PLAY, "play" },
	{ STATE_CONTINUE, "continue" },
	{ STATE_AUTH, "auth" },
	{ STATE_ADMIN, "admin" }
};

/* Public Methods */

StateController::StateController() {
}

void StateController::initialize() {
	_state = STATE_BOOTING;
	initState(_state);
}

GameState StateController::getState() {
	return _state;
}

const char* StateController::stateString() {
	return GameStateDesc[_state].stateDesc;
}

void StateController::changeState(GameState aState) {
	endState(_state);
	_state = aState;
	initState(_state);
}

void StateController::updateState() {
	loopState(_state);
}

/* Private Methods */

void StateController::initState(GameState aState) {
	LOG("Init State: %s", stateString());

	if (aState == STATE_ERROR) {
		ERROR("*** ERROR STATE ***");
	} else if (aState == STATE_BOOTING) {
		Manager::getInstance().initialize(this);
		_dataManager = Manager::getInstance().dataManager;
		_hardwareManager = Manager::getInstance().hardwareManager;
		_serverManager = Manager::getInstance().serverManager;
		_parser = new Parser();
		_parser->initialize();
		_hardwareManager->keypad()->active = true;
		_resetElapsed = kIntervalPressAnyButton;
	} else if (aState == STATE_INIT) {
		_dataManager->unloadStory();
		if (Spark.connected()) {
			Spark.syncTime();
			//Time.zone(_dataManager->metadata.values.timezone);
		}
	} else if (aState == STATE_CREDITS) {
		_hardwareManager->coinAcceptor()->active = true;
		_hardwareManager->printer()->printInsertCoin(_hardwareManager->coinAcceptor()->coins,
		                                             _hardwareManager->coinAcceptor()->coinsPerCredit);
	} else if (aState == STATE_WAITING) {
		_hardwareManager->printer()->printPressButton();
	} else if (aState == STATE_READY) {
		_hardwareManager->printer()->printTitle();
		if (_dataManager->metadata.storyCount > 4) {
			if (_dataManager->metadata.flags.random) {
				Utils::shuffle(_dataManager->liveStoryOrder, _dataManager->metadata.storyCount);
			} else {
				_hardwareManager->printer()->printBigNumbers();
			}
		}
	} else if (aState == STATE_IDLE) {
		_resetElapsed = 0;
	}/* else if (aState == STATE_SELECT) {

	} else if (aState == STATE_PLAY) {

	} else if (aState == STATE_AUTH) {

	} else if (aState == STATE_ADMIN) {
		DEBUG("Hello from admin!");
	}*/
}

void StateController::loopState(GameState aState) {
	// Handle special case where user sets up WiFi credentials
	// while firmware is running, so gameplay isn't disrupted.
	if (!_dataManager->metadata.flags.offline &&
	    !_dataManager->hasCredentials &&
	    WiFi.hasCredentials() &&
	    (aState != STATE_BOOTING)) {
		_dataManager->hasCredentials = true;
		if (!Spark.connected()) {
			Spark.connect();
		}
	}

	if (aState == STATE_BOOTING) {
		// If button 1 held (or hardset to Offline), disable WiFi.
		if (_hardwareManager->keypad()->buttonDown(1)) {
			_dataManager->metadata.flags.offline = !_dataManager->metadata.flags.offline;
		}
		if (!_dataManager->metadata.flags.offline) {
			WiFi.on();
			if (WiFi.hasCredentials()) {
				_dataManager->hasCredentials = true;
				if (!Spark.connected()) {
					Spark.connect();
				}
			} else {
				// Can use this if we handle WiFi setup ourselves!
				//WiFi.off();
			}
		}
		// If button 3 held, don't print, just serial output.
		/*if (_hardwareManager->keypad()->buttonDown(3) ||
			_dataManager->metadata.flags.logPrint) {
			LOG("* PRINT TO SERIAL *");
			// TODO - doesn't work
			_dataManager->logPrint = true;
		}*/
		if (_hardwareManager->keypad()->buttonDown(2)) {
			_dataManager->metadata.flags.sdCard = !_dataManager->metadata.flags.sdCard;
		}
		// Override has had a chance to get set, now setup storage.
		_dataManager->initStorage();
		// Admin mode, if pass required change to STATE_AUTH
		if (_hardwareManager->keypad()->buttonDown(4)) {
			if (_dataManager->metadata.flags.auth) {
				changeState(STATE_AUTH);
			} else {
				changeState(STATE_ADMIN);
			}
		} else {
			changeState(STATE_INIT);
		}
	} else if (aState == STATE_INIT) {
		if (_dataManager->metadata.flags.arcade) {
			changeState(STATE_CREDITS);
		} else {
			changeState(STATE_WAITING);
		}
	} else if (aState == STATE_CREDITS) {
		if (_hardwareManager->coinAcceptor()->consumeCredit()) {
			_seed = millis();
			randomSeed(_seed);
			//_hardwareManager->coinAcceptor()->active = false;
			changeState(STATE_READY);
		} else if (_hardwareManager->keypad()->buttonEvent(BTN_UP_EVENT)) {
			_hardwareManager->printCoinInsertIntervalUpdate();
		}
	} else if (aState == STATE_WAITING) {
		uint8_t total = _hardwareManager->keypad()->keypadEvent(KEYPAD_MULTI_UP_EVENT, 0);
		if (total) {
			_seed = millis();
			randomSeed(_seed);
			if (_dataManager->liveStoryCount && (total == 10)) {
				_dataManager->randomPlay = true;
				uint8_t value = random(1, _dataManager->liveStoryCount + 1);
				_hardwareManager->keypad()->setKeypadEvent(KEYPAD_MULTI_UP_EVENT, value);
				changeState(STATE_SELECT);
			} else {
				changeState(STATE_READY);
			}
		}
	} else if (aState == STATE_READY) {
		if (_dataManager->liveStoryCount > 0) {
			uint8_t storyCount = _dataManager->liveStoryCount;
			//DEBUG("Story Count: %d", _dataManager->metadata.storyCount);
			if (_dataManager->metadata.flags.random && (_dataManager->metadata.storyCount > 4)) {
				storyCount = 4;
			}
			if (storyCount <= 4) {
				_hardwareManager->printer()->printStart();
			}
			// Buffer for title, max title size + 4 for numbering (ex: "10. Story Title")
			char titleBuffer[kStoryTitleSize + 4] = "";
			// Print story titles up to storyCount.

			for (int i = 0; i < storyCount; ++i) {
				memset(&titleBuffer[0], 0, sizeof(titleBuffer));
				if (_dataManager->getNumberedTitle(titleBuffer, i)) {
					_hardwareManager->printer()->wrapText(titleBuffer, kPrinterColumns);
					_hardwareManager->printer()->println(titleBuffer);
					//DEBUG("%s", titleBuffer);
				} else {
					changeState(STATE_ERROR);
					return;
				}
			}
			_hardwareManager->printer()->feed(2);
			changeState(STATE_SELECT);
		} else { // No stories installed!
			_hardwareManager->printer()->printEmpty();
			changeState(STATE_IDLE);
		}
	} else if (aState == STATE_IDLE) {
		if ((_resetElapsed > kIntervalPressAnyButton) ||
		    _hardwareManager->keypad()->buttonEvent(BTN_UP_EVENT)) {
			if (_dataManager->metadata.storyCount > 0) {
				changeState(STATE_INIT);
			}
		}
	} else if (aState == STATE_SELECT) {
		// Wait for multi button up event for story selection.
		uint8_t total = _hardwareManager->keypad()->keypadEvent(KEYPAD_MULTI_UP_EVENT, _dataManager->liveStoryCount);
		if (total) {
			// Story has been selected, initialize the parser with story index (not number).
			if (_parser->initStory(total - 1)) {
				changeState(STATE_PLAY);
			} else {
				changeState(STATE_ERROR);
			}
		}
	} else if (aState == STATE_PLAY) {
		ParseState state = _parser->parsePassage();
		if (state == PARSE_ENDING) {
			_resetElapsed = 0;
		} else if (state == PARSE_IDLE) {
			uint8_t total = _hardwareManager->keypad()->keypadEvent(KEYPAD_MULTI_UP_EVENT, 0);
			if (_resetElapsed > kIntervalPressAnyButton) {
				if (_dataManager->metadata.flags.arcade) {
					changeState(STATE_CREDITS);
				} else {
					changeState(STATE_WAITING);
				}
			} else if (total) {
				if (_dataManager->metadata.flags.arcade) {
					changeState(STATE_CREDITS);
				} else {
					if (_dataManager->liveStoryCount && (total == 10)) {
						_dataManager->randomPlay = true;
						uint8_t value = random(1, _dataManager->liveStoryCount + 1);
						_hardwareManager->keypad()->setKeypadEvent(KEYPAD_MULTI_UP_EVENT, value);
						changeState(STATE_SELECT);
					} else {
						changeState(STATE_READY);
					}
				}
			}
		} else if (state == PARSE_ERROR) {
			changeState(STATE_ERROR);
		}
	}/* else if (aState == STATE_AUTH) {

	} else if (aState == STATE_ADMIN) {

	} else if (aState == STATE_ERROR) {

	}*/
}


void StateController::endState(GameState aState) {
	LOG("End State: %s", stateString());

	if (aState == STATE_BOOTING) {
		_dataManager->logMetadata();
	} else if (aState == STATE_INIT) {
		_hardwareManager->printer()->active = true;
	}/* else if (aState == STATE_CREDITS) {

	} else if (aState == STATE_WAITING) {

	} else if (aState == STATE_READY) {

	} else if (aState == STATE_IDLE) {

	} else if (aState == STATE_SELECT) {

	} else if (aState == STATE_PLAY) {

	} else if (aState == STATE_AUTH) {

	} else if (aState == STATE_ADMIN) {

	} else if (aState == STATE_ERROR) {

	}*/
}

}