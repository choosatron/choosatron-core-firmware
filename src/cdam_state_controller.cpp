#include "cdam_state_controller.h"
#include "cdam_manager.h"
#include "cdam_keypad.h"
#include "cdam_printer.h"

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
	{ STATE_AUTH, "auth" },
	{ STATE_ADMIN, "admin" }
};

/* Public Methods */

StateController::StateController() {
}

void StateController::initialize() {
	changeState(STATE_BOOTING);
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

	if (aState == STATE_BOOTING) {
		Manager::getInstance().initialize(this);
		_dataManager = Manager::getInstance().dataManager;
		_hardwareManager = Manager::getInstance().hardwareManager;
		_serverManager = Manager::getInstance().serverManager;
	} else if (aState == STATE_INIT) {
		if (_dataManager->metadata.flags.random) {
			Utils::shuffle(_dataManager->liveStoryOrder, kMaxStoryCount);
		}
	} else if (aState == STATE_CREDITS) {
		_hardwareManager->coinAcceptor()->active = true;
		_hardwareManager->printer()->printInsertCoin(_hardwareManager->coinAcceptor()->coins,
		                                             _hardwareManager->coinAcceptor()->coinsPerCredit);
	} else if (aState == STATE_WAITING) {
		_hardwareManager->printer()->printPressButton();
	} else if (aState == STATE_READY) {
		_hardwareManager->printer()->printTitle();
	} else if (aState == STATE_IDLE) {

	} else if (aState == STATE_SELECT) {

	} else if (aState == STATE_PLAY) {

	} else if (aState == STATE_AUTH) {

	} else if (aState == STATE_ADMIN) {

	} else if (aState == STATE_ERROR) {

	}
}

void StateController::loopState(GameState aState) {
	if (aState == STATE_BOOTING) {
		changeState(STATE_INIT);
	} else if (aState == STATE_INIT) {
		if (_dataManager->metadata.flags.arcade) {
			changeState(STATE_CREDITS);
		} else {
			changeState(STATE_WAITING);
		}
	} else if (aState == STATE_CREDITS) {
		if (_hardwareManager->keypad()->buttonEvent(BTN_UP_EVENT)) {
			DEBUG("Button Pressed!");
			if (_hardwareManager->coinAcceptor()->consumeCredit()) {
				changeState(STATE_READY);
			} else {
				_hardwareManager->printCoinInsertIntervalUpdate();
			}
		}
	} else if (aState == STATE_WAITING) {
		if (_hardwareManager->keypad()->buttonEvent(BTN_UP_EVENT)) {
			DEBUG("Button Pressed!");
			changeState(STATE_READY);
		}
	} else if (aState == STATE_READY) {
		if (_dataManager->metadata.storyCount > 0) {
			_hardwareManager->printer()->printStart();
			uint8_t storyCount = _dataManager->metadata.storyCount;
			if (_dataManager->metadata.flags.random) {
				storyCount = 4;
			}
			// Buffer for title, max title size + 4 for numbering (ex: "10. Story Title")
			char titleBuffer[kStoryTitleSize + 4] = "";
			// Print story titles up to storyCount.
			for (int i = 0; i < storyCount; ++i) {
				if (_dataManager->getNumberedTitle(titleBuffer, i)) {
					_hardwareManager->printer()->printWrapped(titleBuffer, kPrinterColumns);
					_hardwareManager->printer()->println(titleBuffer);
				}
			}
			_hardwareManager->printer()->feed(2);
			changeState(STATE_SELECT);
		} else { // No stories installed!
			_hardwareManager->printer()->printEmpty();
			changeState(STATE_IDLE);
		}
	} else if (aState == STATE_IDLE) {

	} else if (aState == STATE_SELECT) {
		// Wait for multi button up event for story selection.
		uint8_t total = _hardwareManager->keypad()->keypadEvent(KEYPAD_MULTI_UP_EVENT, _dataManager->metadata.storyCount);
		if (total) {
			// Story has been selected.
			_dataManager->currentStory = total;
			_dataManager->loadStoryHeader(_dataManager->currentStory);
			changeState(STATE_PLAY);
		}
	} else if (aState == STATE_PLAY) {

	} else if (aState == STATE_AUTH) {

	} else if (aState == STATE_ADMIN) {

	} else if (aState == STATE_ERROR) {

	}
}


void StateController::endState(GameState aState) {
	LOG("End State: %s", stateString());

	if (aState == STATE_BOOTING) {

	} else if (aState == STATE_INIT) {
		_hardwareManager->printer()->active = true;
		_hardwareManager->keypad()->active = true;
	} else if (aState == STATE_CREDITS) {

	} else if (aState == STATE_WAITING) {

	} else if (aState == STATE_READY) {

	} else if (aState == STATE_IDLE) {

	} else if (aState == STATE_SELECT) {

	} else if (aState == STATE_PLAY) {

	} else if (aState == STATE_AUTH) {

	} else if (aState == STATE_ADMIN) {

	} else if (aState == STATE_ERROR) {

	}
}

}