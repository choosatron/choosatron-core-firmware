// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC.
// All rights reserved.

#include "choosatron.h"
//#if (OFFLINE == 1)
//#include "spark_disable_wlan.h"
//#include "spark_disable_cloud.h"
//#endif

#include "memory_free.h"
#include "SD.h"

#include "cdam_story_load.h"
//#include "cdam_coin_acceptor.h"
#include "cdam_printer.h"
#include "cdam_keypad.h"
//#include "cdam_logo.h"

/* Function prototypes -------------------------------------------------------*/
void setup();
void loop();
void storyEnding();
void resetGame();
void enableTimerInterrupt();

// Currently game state.
GameState _state;
uint32_t lasttime = 0;
int seconds = 0;
int _currentLed = RED_PIN;
LED_STATE _ledState = LED_NOT_READY;
LED_STATE _lastLedState = LED_NOT_READY;

// Our version of the Konami Code
//Password _password = Password("2288464697");
//Password _password = Password("1122334455");

cdam::Keypad *_keypad;
cdam::Printer *_printer;

#if (COIN_MODE >= 1)
cdam::CoinAcceptor _coinAcceptor;
#endif

cdam::StoryLoad *_storyLoad;

Choosatron::Choosatron() {
}

bool Choosatron::setup()
{
	Serial.begin(BAUD_RATE);

	pinMode(PIEZO_PIN, OUTPUT);

	pinMode(RED_PIN, OUTPUT);
	pinMode(GREEN_PIN, OUTPUT);
	pinMode(BLUE_PIN, OUTPUT);
	digitalWrite(RED_PIN, LOW);
	digitalWrite(GREEN_PIN, HIGH);
	digitalWrite(BLUE_PIN, HIGH);

	// Coin Acceptor Setup
#if (COIN_MODE >= 1)
	// First run set the EEPROM values, or when manually indicated.
	if ((EEPROM.read(0) != 1) || WRITE_EEPROM) {
		// 0 = Value 1 if not first run, 1 = COINS_PER_CREDIT, 2 = COINS_TO_CONTINUE
		slogln("Writing to EEPROM");
		EEPROM.write(0, 1);
		EEPROM.write(1, COINS_PER_CREDIT);
		EEPROM.write(2, COINS_TO_CONTINUE);
		EEPROM.write(3, LOG_PLAY);
	}
	_coinAcceptor.coinsPerCredit = EEPROM.read(1);
	_coinAcceptor.coinsToContinue = EEPROM.read(2);
#endif

	// SD Setup
	if (!SD.begin()) {
		slogln(PSTR("FAIL: SD Card"));
		return false;
	}

	// Printer Setup
	_printer = new cdam::Printer();
	_printer->init();
	_printer->begin(160);
	_printer->setTimes(7000, 300);
	_printer->setABS(true);

	// Story Data Setup
	_storyLoad = new cdam::StoryLoad(_printer);

	// Keypad Setup
	_keypad = new cdam::Keypad();
	_keypad->init();
	_keypad->active = false;

	// Initialize Game State
	resetGame();

	// Keypad Timer Interrupt Setup
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 5; // Timed to 5ms
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 59999; // Times to 5ms
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);
	TIM_Cmd(TIM2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE); //TIM CCR2 IT enable
	enableTimerInterrupt();

	return true;
}

void Choosatron::enableTimerInterrupt() {
		NVIC_InitTypeDef nvicStructure;
		nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
		nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
		nvicStructure.NVIC_IRQChannelSubPriority = 1;
		nvicStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicStructure);
}

extern "C" void TIM2_IRQHandler() {
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
				TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
				_keypad->updateKeypad();
		}
}

int Choosatron::loop()
{
	if (_ledState == LED_NOT_READY) {
		if (_lastLedState != LED_NOT_READY) {
			digitalWrite(RED_PIN, LOW);
			digitalWrite(GREEN_PIN, HIGH);
			digitalWrite(BLUE_PIN, HIGH);
		}
		if (_printer->isReady()) {
			_lastLedState = _ledState;
			_ledState = LED_READY;
		}
	} else if (_ledState == LED_READY) {
		if (_lastLedState != LED_READY) {
			digitalWrite(RED_PIN, HIGH);
			digitalWrite(GREEN_PIN, LOW);
			digitalWrite(BLUE_PIN, HIGH);
			_keypad->active = true;
		}
		if (_printer->isPrinting()) {
			_lastLedState = _ledState;
			_ledState = LED_PRINTING;
		} else if (_keypad->buttonsDown()) {
			_lastLedState = _ledState;
			_ledState = LED_INPUT;
		}
	} else if (_ledState == LED_PRINTING) {
		if (_lastLedState != LED_PRINTING) {
			_keypad->active = false;
			digitalWrite(RED_PIN, LOW);
			digitalWrite(GREEN_PIN, HIGH);
			digitalWrite(BLUE_PIN, HIGH);
		}
		if (!_printer->isPrinting()) {
			_lastLedState = _ledState;
			_ledState = LED_READY;
			_keypad->active = true;
		}
	} else if (_ledState == LED_INPUT) {
		if (_lastLedState != LED_INPUT) {
			digitalWrite(RED_PIN, HIGH);
			digitalWrite(GREEN_PIN, HIGH);
			digitalWrite(BLUE_PIN, LOW);
		}
		if (!_keypad->buttonsDown()) {
			_lastLedState = _ledState;
			_ledState = LED_READY;
		}
	}

	/*
	if (millis() < lasttime) {
		lasttime = millis();
	}
	if ((lasttime + 1000) <= millis()) {
	}
	*/
	// In WAIT state, need coins, keypad prints message.
	if (_state == WAIT_STATE) {
#if (COIN_MODE == 0)
		if (_keypad->buttonEvent(cdam::BTN_UP_EVENT)) {
			_keypad->clearEvents();
			_printer->logProgStr(STATE_SELECT);
			_state = SELECT_STATE;
		}
#elif (COIN_MODE >= 1)
		// Hold in loop until coin is detected.
		// Print message when key pressed, within reason.
		if (digitalRead(COIN_PIN)) {
			if (_coinAcceptor.checkForCoin()) {
				// Attempt to consume a play credit.
				if (_coinAcceptor.consumeCredit()) {
					_printer->logProgStr(STATE_SELECT);
					_state = SELECT_STATE;
				} else {
					delay(1);
				}
			}
		} else {

			if (_keypad->buttonEvent(cdam::BTN_UP_EVENT)) {
				_keypad->clearEvents();
				//if (key == RESET_PASS) {
				//  _state = AUTH;
				//} else {
					unsigned long timeDelay = millis();
					if (INSERT_COIN_DELAY > timeDelay) {
						timeDelay = INSERT_COIN_DELAY;
					}
					if ((_coinAcceptor.insertCoinTime == 0) || ((timeDelay - INSERT_COIN_DELAY) >= _coinAcceptor.insertCoinTime)) {
						_printer->printInsertCoin(_coinAcceptor.nextCredit);
						_coinAcceptor.insertCoinTime = timeDelay;
					}
				//}
			}
		}
#endif
	} else if (_state == SELECT_STATE) {
		//slogln("SELECT");
		if (!_storyLoad->manLoaded) {
			char *stories;
			//_printer->printBitmap(384, 115, logo, false);

			stories = _storyLoad->loadManifest();
			_printer->printTitleMenu(stories);
			slogln(stories);

			free(stories);

			_storyLoad->manLoaded = true;
		} else {
			char key = ANALOGUE_NO_KEY;
			if (_keypad->keypadEvent(cdam::KEYPAD_MULTI_UP_EVENT, _storyLoad->storyCount)) {
				key = _keypad->keypadEventChar(cdam::KEYPAD_MULTI_UP_EVENT);
				_keypad->clearEvents();
			}

			if (key != ANALOGUE_NO_KEY) {
				_storyLoad->storyId = key;
				// Force printing of story choice.
				slogln(_storyLoad->storyId);
				_storyLoad->choice = key - 1;
				_storyLoad->choiceCount = 2;
				_storyLoad->manLoaded = false;
				_printer->logProgStr(STATE_PLAY);
				_state = PLAY_STATE;
				_storyLoad->loadPassage();
				//_printer->setDefault();
			}
		}
	} else if (_state == PLAY_STATE) {
		//slogln("PLAYING");
		//_storyLoad->loadPassage();

#if (DEBUG && MEMORY)
		slog(PSTR("\t* Stable Free Memory: "));
		slogln(freeMemory());
#endif

		//slog(PSTR("Current Points: "));
		//slogln(_storyLoad->totalPoints);

		if (_storyLoad->ending == 0) {
			if (_storyLoad->autoJump) {
				slogln("autojump");
				_storyLoad->autoJump = false;
				_storyLoad->choiceNum = 1;
				_storyLoad->choice = '1';
				_storyLoad->loadPassageStr();
				_storyLoad->loadPassage();
			} else {
				char key = ANALOGUE_NO_KEY;
				if (_keypad->keypadEvent(cdam::KEYPAD_MULTI_UP_EVENT, _storyLoad->choiceCount)) {
					key = _keypad->keypadEventChar(cdam::KEYPAD_MULTI_UP_EVENT);
					_keypad->clearEvents();
				}

				if (key != ANALOGUE_NO_KEY) {
					_storyLoad->choice = key - 1;
					_storyLoad->totalChoices++;
					_storyLoad->loadPassageStr();
					_storyLoad->loadPassage();
				}
			}
		} else if (_storyLoad->hasEnded == 1) {
			// Any key to continue on to the extro.
			 //_input.waitForInputInRange(10);
			if (_keypad->buttonEvent(cdam::BTN_UP_EVENT)) {
				_keypad->clearEvents();
				slogln("ending pushed");
				_storyLoad->loadPassage();
			}
		} else if (_storyLoad->ending == 1) {
#if ((COIN_MODE == 1) && (CONTINUES == 1))
			if ((_storyLoad->canContinue == 1) && (_storyLoad->quality < 5) && CONTINUES) {
				_printer->logProgStr(STATE_CONTINUE);
				_printer->printPoints(_storyLoad->totalPoints, _storyLoad->perfectScore);
				_printer->println("");
				_printer->printCoinToContinue();
				_state = CONTINUE;
			} else
#elif ((COIN_MODE == 0) && (CONTINUES == 1))
			if ((_storyLoad->canContinue == 1) && (_storyLoad->quality < 5) && CONTINUES) {
				_state = CONTINUE;
			} else
#endif
			{
				slogln("Story end!");
				storyEnding();
				resetGame();
			}
		} else {
			_printer->println(PSTR("[ERROR] Invalid ending state."));
			_state = ERROR_STATE;
		}
	} else if (_state == CONTINUE_STATE) {
#if (COIN_MODE == 1)
		unsigned long countdown = millis();
		while (1) {
			// Check for inserted coin.
			if (_coinAcceptor.checkForCoin()) {
				// Attempt to consume a continue.
				if (_coinAcceptor.consumeContinue()) {
#endif
					// Set choice range.
					byte range = (_storyLoad->totalChoices > MAX_JUMP_BACK) ? MAX_JUMP_BACK : _storyLoad->totalChoices;
					// Ask how many choices back to move.
					char c = range + '0';
					_printer->print(PSTR("Go back how many choices? (1-"));
					_printer->print(c);
					_printer->println(")");
					_printer->feed(2);
					// Wait for choice jump input.
					char key = 0;
					if (_keypad->keypadEvent(cdam::KEYPAD_MULTI_DOWN_EVENT, _storyLoad->choiceCount)) {
						key = _keypad->keypadEventValue(cdam::KEYPAD_MULTI_DOWN_EVENT);
						_keypad->clearEvents();
					}

					if (key != ANALOGUE_NO_KEY) {
						_storyLoad->choiceNum = key - '0' - 1;
						_storyLoad->choice = key - 1;

						// Setup for jumping back.
						//byte index = _storyLoad->totalChoices - _storyLoad->choiceNum;
						_storyLoad->passageId[0] = _storyLoad->continuePath[(_storyLoad->totalChoices - _storyLoad->choiceNum) % MAX_JUMP_BACK][0];
						_storyLoad->passageId[1] = _storyLoad->continuePath[(_storyLoad->totalChoices - _storyLoad->choiceNum) % MAX_JUMP_BACK][1];
						_storyLoad->passageId[2] = _storyLoad->continuePath[(_storyLoad->totalChoices - _storyLoad->choiceNum) % MAX_JUMP_BACK][2];
						slog(_storyLoad->passageId[0]);
						slog(_storyLoad->passageId[1]);
						slogln(_storyLoad->passageId[2]);
						_storyLoad->ending = 0;
						_storyLoad->autoJump = false;
						_storyLoad->choiceCount = 0;

						// Remove any points received from passages skipped.
						_storyLoad->totalPoints -= _storyLoad->points;
						byte choiceJump = _storyLoad->choiceNum;
						while (choiceJump > 0) {
							_storyLoad->totalPoints -= _storyLoad->continuePath[(_storyLoad->totalChoices - choiceJump) % MAX_JUMP_BACK][2];
							choiceJump--;
						}
						_storyLoad->totalPoints -= _storyLoad->continuePenalty;
						_storyLoad->totalChoices -= _storyLoad->choiceNum;

						_printer->println(PSTR("Back to reality, daydreamer..."));
						_printer->feed(1);
						_printer->logProgStr(STATE_PLAY);
						_state = PLAY_STATE;
					}
					return 0;
#if (COIN_MODE == 1)
				}
			}

			if (_keypad->buttonEvent(cdam::BTN_UP_EVENT) || ((countdown + TIME_TO_CONTINUE) <= countdown)) {
				// Canceling continue.
				storyEnding();
				resetGame();
			}
		}
#endif
	} else if (_state == AUTH_STATE) {
		//_printer->logProgStr(STATE_AUTH);
		//_input.getKey();
	} else if (_state == ADMIN_STATE) {
		_printer->logProgStr(STATE_ADMIN);
		_printer->println("1.Set Datetime\n2.");
		_state = WAIT_STATE;
	} else if (_state == ERROR_STATE) {
		resetGame();
	} else {
		_printer->println(PSTR("[ERROR] Invalid State"));
		_state = ERROR_STATE;
	}

	_keypad->clearEvents();
	return 0;
}

void Choosatron::storyEnding() {
	// Was only needed when printer isn't active.
	_printer->wake();
	//
	_printer->justify('C');
	if (_storyLoad->totalPoints > 0) {
		//_printer->printPoints(_storyLoad->totalPoints, _storyLoad->perfectScore);
		if (_storyLoad->quality == 5) {
			_printer->printPoints(_storyLoad->totalPoints, _storyLoad->perfectScore);
			if (_storyLoad->totalPoints == _storyLoad->perfectScore) {
				_printer->underlineOn();
				_printer->println(PSTR("You are a master of choice!"));
				_printer->underlineOff();
			} else {
				_printer->println(PSTR(" - Great work!"));
			}
		} else {
			if (!CONTINUES) {
				_printer->printPoints(_storyLoad->totalPoints, _storyLoad->perfectScore);
			}
			_printer->println(PSTR("Good work. Play again soon!"));
		}
	} else {
		_printer->println(PSTR("Great work! Thanks for playing."));
	}
	_printer->printAuthor(_storyLoad->storyId);
	_printer->println(PSTR("Visit choosatron.com and contact\nJerry at @choosatron or\njerry@choosatron.com"));
	_printer->justify('L');
	_printer->feed(2);
}

void Choosatron::resetGame() {
	//_password.reset();
	_storyLoad->totalChoices = 0;
	_storyLoad->totalPoints = 0;
	_storyLoad->points = 0;
	_storyLoad->perfectScore = 0;
	_storyLoad->continuePenalty = 0;
	_storyLoad->jumpPoints = 0;
	_storyLoad->manLoaded = false;
	_storyLoad->autoJump = false;
	_storyLoad->jumpIndent = 0;
	_storyLoad->storyCount = 0;
	_storyLoad->storyId = 0;
	_storyLoad->ending = 0;
	_storyLoad->choiceNum = 0;
	_storyLoad->choiceCount = 0;
	_storyLoad->choice = '\0';
	_storyLoad->hasEnded = 0;
	_storyLoad->passageId[0] = '0';
	_storyLoad->passageId[1] = '\0';
	_storyLoad->passageId[2] = '\0';
	_keypad->clearEvents();
#if (COIN_MODE >= 1)
	// If credits, use one and go to story selection.
	if (_coinAcceptor.consumeCredit()) {
		_printer->logProgStr(STATE_SELECT);
		_state = SELECT_STATE;
	} else
#endif
	{
		_printer->logProgStr(STATE_WAIT);
		_state = WAIT_STATE;
	}
}

/*void keypadEvent(KeypadEvent aKey) {
	if (_state == AUTH) {
		switch (_keypad->getState()){
			case PRESSED:
				switch (aKey){
			case RESET_PASS: _password.reset(); break;
			default:
					if (!_password.append(aKey)) {
						_password.reset();
					}
					checkPassword();
			}
		}
	}
}*/

/*void checkPassword() {
	if (_password.evaluate()) {
		_password.reset();
		slogln(PSTR("Success"));
		_state = ADMIN;
	}
}*/