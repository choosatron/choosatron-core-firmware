// Choosatron Deluxe Adventure Matrix
// Copyright (c) 2014 Monkey with a Mustache Entertainment, LLC
// All rights reserved.

#include "cdam_coin_acceptor.h"

namespace cdam
{

	CoinAcceptor::CoinAcceptor() {
	}

	void CoinAcceptor::initialize() {
		_credits = 0;
		_coins = 0;
		this->nextCredit = 0;
		this->insertCoinTime = 0;
		this->active = true;
	}

	void CoinAcceptor::updateState() {
		if (this->active) {

		}
	}
/*void CoinAcceptor::waitForCoin(const Keypad *aKeypad) {
while (!digitalRead(COIN_PIN)) {
  char key = aKeypad->getKey();
  if ((key != NO_KEY) && (key != RESET_PASS)) {
    slogln(key);
    unsigned long timeDelay = millis();
    if (INSERT_COIN_DELAY > timeDelay) {
      timeDelay = INSERT_COIN_DELAY;
    }
    if ((_insertCoinTime == 0) || ((timeDelay - INSERT_COIN_DELAY) >= _insertCoinTime)) {
      printInsertCoin();
      _insertCoinTime = timeDelay;
    }
    delay(1);
  }
}
}*/

/*boolean senseCoin() {
if (!digitalRead(COIN_PIN)) {
  char key = _keypad.getKey();
  if (key != NO_KEY) {
    unsigned long timeDelay = millis();
    if (INSERT_COIN_DELAY > timeDelay) {
      timeDelay = INSERT_COIN_DELAY;
    }
    if ((_insertCoinTime == 0) || ((timeDelay - INSERT_COIN_DELAY) >= _insertCoinTime)) {
      printInsertCoin();
      _insertCoinTime = timeDelay;
    }
  }
  return false;
}
return true;
}*/

boolean CoinAcceptor::checkForCoin() {
// When coin pin is high, track the length with a counter.
	uint8_t coinSenseCounter = 0;
	while (digitalRead(COIN_PIN)) {
		delay(1);
		coinSenseCounter++;
	}
	if ((coinSenseCounter <= 60) && (coinSenseCounter >= 15)) {
		slogln(coinSenseCounter);
		coinInserted();
		return true;
	}
	return false;
}

void CoinAcceptor::coinInserted() {
	_coins++;
	slog("Total Coins: ");
	slogln(_coins);
	this->nextCredit++;
	if (this->nextCredit == this->coinsPerCredit) {
		_credits++;
		this->nextCredit = 0;
	}
}

boolean CoinAcceptor::consumeCredit() {
	if (_credits > 0) {
		_credits--;
		return true;
	}
	return false;
}

boolean CoinAcceptor::consumeContinue() {
#if (COIN_MODE < 2)
	return false;
#endif
	if (this->nextCredit >= this->coinsToContinue) {
		this->nextCredit -= this->coinsToContinue;
		return true;
	} else if (_credits > 0) {
		_credits--;
		this->nextCredit += (this->coinsPerCredit - this->coinsToContinue);
		return true;
	}
	return false;
}

}